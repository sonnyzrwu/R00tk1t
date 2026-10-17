#include <ntddk.h>
#include <wdf.h>
#include <intrin.h>

#pragma pack(push, 1)
typedef struct _SEG_DESCRIPTOR {
	USHORT size_00_15; // Limit 0-15
	USHORT baseAddress_00_15; // 16-31

	USHORT baseAddress_16_23 : 8; // 32-39
	USHORT type : 4; // 40-43
	USHORT sFlag : 1; // 44
	USHORT dpl : 2; // 45-46
	USHORT pFlag : 1; // 47
	USHORT size_16_19 : 4; // 48-51

	USHORT notUsed : 1; // 52

	USHORT lFlag : 1; // 53
	USHORT DB : 1; // 54
	USHORT gFlag : 1; // 55
	USHORT baseAddress_24_31 : 8; // 56-63
    // Constructor
    _SEG_DESCRIPTOR(const void* address)
    {
        ULONGLONG raw = 0;

        RtlCopyMemory(&raw, address, sizeof(raw));

        size_00_15 = (USHORT)(raw & 0xFFFF);
        baseAddress_00_15 = (USHORT)((raw >> 16) & 0xFFFF);

        baseAddress_16_23 = (USHORT)((raw >> 32) & 0xFF);
        type = (USHORT)((raw >> 40) & 0xF);
        sFlag = (USHORT)((raw >> 44) & 0x1);
        dpl = (USHORT)((raw >> 45) & 0x3);
        pFlag = (USHORT)((raw >> 47) & 0x1);
        size_16_19 = (USHORT)((raw >> 48) & 0xF);

        notUsed = (USHORT)((raw >> 52) & 0x1);
        lFlag = (USHORT)((raw >> 53) & 0x1);
        DB = (USHORT)((raw >> 54) & 0x1);
        gFlag = (USHORT)((raw >> 55) & 0x1);
        baseAddress_24_31 = (USHORT)((raw >> 56) & 0xFF);
    }
} SEG_DESCRIPTOR, * PSEG_DESCRIPTOR;


//  THIS IS WRONG, CALL GATE DESRIPTORS ARE EXPANDED to 128 bits.
typedef struct _CALL_GATE_DESCRIPTOR {
	USHORT offset_00_15; // Offset 0-15
	USHORT selector; // Segment 16-31

	USHORT argCount : 5; // 32-36

	USHORT zeros : 3; // 37-39
	USHORT type : 4; // 40-43
	USHORT sFlag : 1; // 44
	USHORT dpl : 2; // 45-46
	USHORT pFlag : 1; // 47 ** THIS IS CRUCIAL

	USHORT offset_16_31; // 48 - 63

} CALL_GATE_DESCRIPTOR, * PCALL_GATE_DESCRIPTOR;


typedef struct _GDTR {
	USHORT limit; // 16 bits
	ULONG64 baseAddress; //  64 bits
} GDTR, * PGDTR;
#pragma pack(pop)

extern "C" void ReadGdtrAsm(GDTR* output);
PSEG_DESCRIPTOR getGDTBaseAddress() {
	GDTR gdtr;

	ReadGdtrAsm(&gdtr);

	return (PSEG_DESCRIPTOR)gdtr.baseAddress;
}


USHORT getGDTSize() {
	GDTR gdtr;
	ReadGdtrAsm(&gdtr);
	return ( (USHORT) gdtr.limit  / 8);
}


static const char* GdtTypeName(ULONG s, ULONG type)
{
    if (s) {
        return (type & 0x8) ? "Code" : "Data";
    }

    switch (type) {
    case 0x2: return "LDT";
    case 0x9: return "TSS-Avail";
    case 0xB: return "TSS-Busy";
    case 0xC: return "CallGate";
    default:  return "System";
    }
}

void walkGDT()
{
    GDTR gdtr = {};
    ReadGdtrAsm(&gdtr);

    // sizeof(UCHAR) is 1 byte = 8 bits.
    // sizeof(UCHAR*) is typically 8 bytes = 64 bits on 64-bit Windows.
    // Convert the GDTR base address value to a pointer-sized integer,
    // then reinterpret that value as a pointer to individual bytes.
    const UCHAR* table =
        reinterpret_cast<const UCHAR*>(
            static_cast<ULONG_PTR>(gdtr.baseAddress));

    // ULONG is 32 bits, USHORT is 16 bits
    const ULONG slotCount =
        (static_cast<ULONG>(gdtr.limit) + 1) / 8; //

    DbgPrint("\n[GDT] Base: 0x%016llX Limit: 0x%04X\n",
        gdtr.baseAddress, (ULONG)gdtr.limit);

    DbgPrint("Sel     Base                Limit      Type        P L DB G DPL S\n");
    DbgPrint("------  ------------------  ---------- ----------  - - -- - --- -\n");

    for (ULONG i = 0; i < slotCount; ++i)
    {
        ULONGLONG raw = 0; // 64 bits

        // size of each entry in GDT is 8 bytes. By pointer arithmetic, we can loop and store in raw.
        RtlCopyMemory(
            &raw,
            table + (i * 8), 
            sizeof(raw));

        // Decode descriptor attributes
        ULONG type = (ULONG)((raw >> 40) & 0xF);
        ULONG s = (ULONG)((raw >> 44) & 0x1);
        ULONG dpl = (ULONG)((raw >> 45) & 0x3);
        ULONG p = (ULONG)((raw >> 47) & 0x1);
        ULONG l = (ULONG)((raw >> 53) & 0x1);
        ULONG db = (ULONG)((raw >> 54) & 0x1);
        ULONG g = (ULONG)((raw >> 55) & 0x1);

        // Decode the 20-bit segment limit
        ULONG limit =
            (ULONG)(raw & 0xFFFF) |
            (ULONG)(((raw >> 48) & 0xF) << 16);

        if (g) {
            limit = (limit << 12) | 0xFFF;
        }

        // Decode the 32-bit segment base
        ULONGLONG base =
            ((raw >> 16) & 0xFFFFULL) |
            (((raw >> 32) & 0xFFULL) << 16) |
            (((raw >> 56) & 0xFFULL) << 24);

        // Identify 16-byte x64 system descriptors 
        bool twoSlots = !s &&
            (type == 0x2 || type == 0x9 ||
                type == 0xB || type == 0xC);

        ULONGLONG upper = 0;

        if (twoSlots) {
            if (i + 1 >= slotCount) {
                DbgPrint("[GDT] Truncated system descriptor\n");
                break;
            }

            RtlCopyMemory(
                &upper,
                table + (i + 1) * 8,
                sizeof(upper));

            // TSS/LDT descriptors have a 64-bit base
            if (type != 0xC) {
                base |= (upper & 0xFFFFFFFFULL) << 32;
            }
        }

        if (!s && type == 0xC) {
            // Call gates have an offset, not a segment base
            DbgPrint(
                "0x%04X  %-18s  %-10s %-10s  %lu %lu %lu  %lu %lu   %lu\n",
                i * 8, "-", "-", "CallGate",
                p, l, db, g, dpl, s);
        }
        else {
            DbgPrint(
                "0x%04X  0x%016llX  0x%08X %-10s  %lu %lu %lu  %lu %lu   %lu\n",
                i * 8,
                base,
                limit,
                GdtTypeName(s, type),
                p, l, db, g, dpl, s);
        }

        // Skip the second half of a 16-byte descriptor
        if (twoSlots) {
            ++i;
        }
    }
}



const UCHAR * findEmptySlot(){
    GDTR gdtr = {};
    ReadGdtrAsm(&gdtr);

    const UCHAR* table =
        reinterpret_cast<const UCHAR*>(
            static_cast<ULONG_PTR>(gdtr.baseAddress));
    /*
    For example, suppose the GDT has 3 entries, each 8 bytes:
    Entry	Byte offsets
        0	0–7
        1	8–15
        2	16–23
        The GDTR.limit would be 23, because byte offset 23 is the last valid byte.
        - Correct: (23 + 1) / 8 = 3 slots.
        - Without adding 1 : 23 / 8 = 2 slots, because integer division truncates the remainder.
     */
    const ULONG slotCount = // ULONG is 32 bits.
        (static_cast<ULONG>(gdtr.limit) + 1u) / 8u; // gdtr.limit is the maximum byte offset.
    DbgPrint("[GDT] Size of the GDT is %d", slotCount);
    
    // i+1 is important as it stops and prevents overstepping the last entry. Protect against byte overflow, do slotCount-1
    for (ULONG i = 0; i < slotCount - 1; ++i) { 


        ULONGLONG raw = 0;
        
        RtlCopyMemory(
            &raw,
            table + (i * 8),
            sizeof(raw)
        );

        // const ULONG type = static_cast<ULONG>((raw >> 40) & 0xFu);
        /*
        S (descriptor type) flag
        Specifies whether the segment descriptor is for a system segment (S flag is clear) or a code or data
        segment (S flag is set).
        */
        const bool  s = ((raw >> 44) & 1u) != 0; // If seg desc is system seg-> system descritpr bit = 0 -> s= (0!=0) -> False
        const bool  p = ((raw >> 47) & 1u) != 0; // present bit=0 -> p= (0!=0) -> False
        if (!s){ // All we care about is it's a system descriptor, we don't care about its type.
            ++i;
            continue;
        }
        
        if (!p) {
            ULONGLONG nextRaw = 0;
            RtlCopyMemory(
                &nextRaw, 
                table + ((i + 1) * 8),
                sizeof(nextRaw)
            );
            USHORT pNext = (USHORT)((nextRaw >>47) & 0x1); // the next slot
            if (pNext == 0) {
                return table + (static_cast<SIZE_T>(i) * 8); // returns the memory address of the free slot.
            }
            
        }

    }

    DbgPrint("[GDT] No open slot was found in GDT.");
    return nullptr;

}
