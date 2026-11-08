#include "ssdt.h"
/*
	------------------
	kd> u nt!KeAddSystemServiceTable+0xbd L1
	nt!KeAddSystemServiceTable+0xbd:
	fffff800`aae7bdcd 48391d0c5b7800  cmp     qword ptr [nt!KeServiceDescriptorTable+0x20 (fffff800`ab6018e0)],rbx
	------------------

	We will dynamically obtain the address of the SSDT by reading the opcodes at this address by the KeAddSystemServiceTable address.

*/

/*
PVOID GetSSDTAddress(PVOID KeAddSystemServiceAddr) {

PVOID AddressToRead = NULL;
PVOID NewAddress = NULL;
PBYTE Instruction = (PBYTE)KeAddSystemServiceAddr + 0xbd;

// Get Offset Address 
DWORD OffsetAddress = *(PDWORD)(Instruction + 3);

// Calculate RIP 
UINT_PTR rip = (UINT_PTR)Instruction + 7;

// Add Offset Address to RIP 
AddressToRead = (PVOID)(rip + (INT32)OffsetAddress);

// Get Absolute Address 
AddressToRead = (PVOID)((PBYTE)AddressToRead - 0x20);

NewAddress = GetAddress(AddressToRead);

return NewAddress;
}


/*
0: kd> u nt!KiSystemServiceRepeat
nt!KiSystemServiceRepeat:
fffff802`72820384 4c8d1535059f00  lea     r10,[nt!KeServiceDescriptorTable (fffff802`732108c0)]
fffff802`7282038b 4c8d1daeb68e00  lea     r11,[nt!KeServiceDescriptorTableShadow (fffff802`7310ba40)]
fffff802`72820392 f7437880000000  test    dword ptr [rbx+78h],80h
fffff802`72820399 7413            je      nt!KiSystemServiceRepeat+0x2a (fffff802`728203ae)
fffff802`7282039b f7437800002000  test    dword ptr [rbx+78h],200000h
fffff802`728203a2 7407            je      nt!KiSystemServiceRepeat+0x27 (fffff802`728203ab)
fffff802`728203a4 4c8d1d55b88e00  lea     r11,[nt!KeServiceDescriptorTableFilter (fffff802`7310bc00)]
fffff802`728203ab 4d8bd3          mov     r10,r11

kd> dq nt!KeServiceDescriptorTable L4
fffff802`732108c0  fffff802`724d6a10 00000000`00000000
fffff802`732108d0  00000000`000001d9 fffff802`724d7178

0: kd> dq fffff802`724d6a10
fffff802`724d6a10  02496900`027fde04 080f5000`05d36e02
fffff802`724d6a20  033a4800`05765400 0583cb06`06021d05
fffff802`724d6a30  051c5601`06017d05 05231b00`0599ea00
fffff802`724d6a40  0582b900`05169700 05766900`059ab000
fffff802`724d6a50  05334901`06480001 052cee02`05afec00
fffff802`724d6a60  05bc5f00`05537200 057d7f02`057e2801
fffff802`724d6a70  0559cb01`0532d302 05b70305`05b08501
fffff802`724d6a80  06209503`05901300 07fa5400`05732400

*/

PVOID GetKeAddSystemServiceTableAddress() {
	UNICODE_STRING KeAddSystemServiceTable;
	RtlInitUnicodeString(&KeAddSystemServiceTable, L"KeAddSystemServiceTable");
	PVOID KeAddSystemServiceTableAddress = MmGetSystemRoutineAddress(&KeAddSystemServiceTable);
	if (KeAddSystemServiceTableAddress == NULL) {
		return NULL;
	}
	return KeAddSystemServiceTableAddress;
}

PVOID GetSSDT( PVOID KeAddSystemServiceTable  ) {
	//PVOID KeServiceDescriptorTableAddress = NULL;

	// We wish to advance memory by 0xbd BYTEs. 
	// Pointer arithmetic is determined by the pointer's type.
	// Therefore, we cast to BYTE *
	BYTE * TargetInstructionAddress = (BYTE *) KeAddSystemServiceTable + 0xbd;

	// 48 38 1D 0D 5B 78 00
	// 0  1  2  3			<---- 0xfff..... Memory offset/indicies
	// We wish to obtain 0D 5B 78 00 which occurs at Byte 3
	// Offset in Little Endian, -> [Offset] = 00785B0D, a SIGNED value
	INT32 Offset;
	memcpy(&Offset, TargetInstructionAddress + 3, sizeof(Offset));

	// In x64, RIP-relative addressing calculates the effective address using:
	// RIP_next + Offset = EffectiveAddress
	// RIP_next = RIP_curr + 7 = TargetInstructionAddress + 7
	// 48 38 1D 0D 5B 78 00 [RIP_next]
	// 0  1  2  3  4  5  6  7			<---- 0xfff..... Memory offset/indicies
	UINT_PTR EffectiveAddress = (UINT_PTR) (TargetInstructionAddress + 7) ; // UINT_PTR is an Integer designed to hold a pointer.

	// nt!KeServiceDescriptorTable+0x20 (fffff806`a3c018e0) <------ Debugger Recognizes This Symbol
	// Integer arithmetic  was chosen due to the Little Endian format.
	PVOID AddressOfSSDTPlus20 = (PVOID) (EffectiveAddress + (INT32)Offset); // If this doesn't work, try: (INT_PTR)(INT32)Offset

	PVOID AddressOfSSDT = PVOID( (BYTE*)AddressOfSSDTPlus20 - 0x20);

	return AddressOfSSDT;
	
}