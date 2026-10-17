
#include "Descriptor.h"
/*
=========================OUTPUT=========================
1: kd> g
[GDT] Driver Loaded.

[GDT] Base: 0xFFFFF80378214FB0 Limit: 0x0057
Sel     Base                Limit      Type        P L DB G DPL S
------  ------------------  ---------- ----------  - - -- - --- -
0x0000  0x0000000000000000  0x00000000 System      0 0 0  0 0   0
0x0008  0x0000000000000000  0x00000000 System      0 0 0  0 0   0
0x0010  0x0000000000000000  0x00000000 Code        1 1 0  0 0   1
0x0018  0x0000000000000000  0x00000000 Data        1 0 1  0 0   1
0x0020  0x0000000000000000  0xFFFFFFFF Code        1 0 1  1 3   1
0x0028  0x0000000000000000  0xFFFFFFFF Data        1 0 1  1 3   1
0x0030  0x0000000000000000  0x00000000 Code        1 1 0  0 3   1
0x0038  0x0000000000000000  0x00000000 System      0 0 0  0 0   0
0x0040  0xFFFFF80378213000  0x00000067 TSS-Busy    1 0 0  0 0   0
0x0050  0x0000000000000000  0x00003C00 Data        1 0 1  0 3   1
[GDT] Size of the GDT is 11[GDT] No open slot was found in GDT.[GDT] No empty slot was found.
*/

DRIVER_UNLOAD DriverUnload;

VOID DriverUnload(
	_DRIVER_OBJECT* DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("[GDT] Driver unloading. \n");
	
}


extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;
	DbgPrint("[GDT] Driver Loaded.\n");
	
	walkGDT();
	const UCHAR* emptySlot = findEmptySlot();
	if (emptySlot == nullptr){
		DbgPrint("[GDT] No empty slot was found.");
		return STATUS_SUCCESS;
	}
	DbgPrint("[GDT] Empty slot found: 0x%016llX\n", emptySlot);

	return STATUS_SUCCESS;
}