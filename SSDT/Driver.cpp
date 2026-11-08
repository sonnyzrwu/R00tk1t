
#include "ssdt.h"

DRIVER_UNLOAD DriverUnload;
VOID DriverUnload (
	_In_ _DRIVER_OBJECT *DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("[SSDT] Driver unloading. \n");
}

extern "C"
NTSTATUS DriverEntry( 
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;
	PVOID KeAddSystemServiceTableAddress = GetKeAddSystemServiceTableAddress();

	SERVICE_DESCRIPTOR_TABLE* ServiceDescriptorTable = (SERVICE_DESCRIPTOR_TABLE*) GetSSDT( KeAddSystemServiceTableAddress);

	DbgPrint("KeServiceDescriptorTable ServiceTable  %p \r\n", ServiceDescriptorTable->nt);
	DbgPrint("KeServiceDescriptorTable Count         %p \r\n", ServiceDescriptorTable->win32k);
	DbgPrint("KeServiceDescriptorTable Limit         %016x \r\n", ServiceDescriptorTable->sst3);
	DbgPrint("KeServiceDescriptorTable ArgumentTable %p \r\n", ServiceDescriptorTable->sst4);
	/*
	0: kd> g
	KeServiceDescriptorTable ServiceTable  FFFFF803E54DADC0 
	KeServiceDescriptorTable Count         0000000000000000 
	KeServiceDescriptorTable Limit         00000000000001e9 
	KeServiceDescriptorTable ArgumentTable FFFFF803E54DB568 
	Break instruction exception - code 80000003 (first chance)

	0: kd> dq nt!KeServiceDescriptorTable L4
	fffff803`e66018c0  fffff803`e54dadc0 00000000`00000000
	fffff803`e66018d0  00000000`000001e9 fffff803`e54db568
	
	*/

	return STATUS_SUCCESS;
}
