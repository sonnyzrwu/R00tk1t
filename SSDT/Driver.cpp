
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

	SERVICE_DESCRIPTOR_TABLE* ServiceDescriptorTable = (SERVICE_DESCRIPTOR_TABLE*) FindSSDT();

	DbgPrint("KeServiceDescriptorTable ServiceTable  %p \r\n", ServiceDescriptorTable->nt);
	DbgPrint("KeServiceDescriptorTable Count         %p \r\n", ServiceDescriptorTable->win32k);
	DbgPrint("KeServiceDescriptorTable Limit         %016x \r\n", ServiceDescriptorTable->sst3);
	DbgPrint("KeServiceDescriptorTable ArgumentTable %p \r\n", ServiceDescriptorTable->sst4);

	return STATUS_SUCCESS;
}
