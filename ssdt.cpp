#include "ssdt.h"


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

PULONGLONG FindSSDT() {
	/*
	IA32_LSTAR is a 64-bit model-specific register (MSR), at index 0xC0000082. When a program executes the x64 SYSCALL instruction,
	the CPU loads the instruction pointer (RIP) from this register and begins executing at that kernel entry address.*/
	ULONGLONG  KiSystemCall64 = __readmsr(0xC0000082);	// Get the address of nt!KeSystemCall64
	ULONGLONG  KiSystemServiceRepeat = 0;
	INT64 Limit = 4096; // 2^12 Page Size, roughly estimated. INCREASE if it doesn't work.

	for (INT64 i = 0; i < Limit; i++) {		        // Increase that address until you hit "0x4c/0x8d/0x15"
		if (*(PUINT8)(KiSystemCall64 + i) == 0x4C
			&& *(PUINT8)(KiSystemCall64 + i + 1) == 0x8D
			&& *(PUINT8)(KiSystemCall64 + i + 2) == 0x15)
		{
			KiSystemServiceRepeat = KiSystemCall64 + i;
			DbgPrint("KiSystemCall64           %p \r\n", (PULONGLONG) KiSystemCall64);
			DbgPrint("KiSystemServiceRepeat    %p \r\n", (PULONGLONG) KiSystemServiceRepeat);

			// Convert relative address to absolute address
			return (PULONGLONG)(*(PINT32)(KiSystemServiceRepeat + 3) + KiSystemServiceRepeat + 7);
		}
	}

	return nullptr;
}