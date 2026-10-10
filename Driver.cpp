
#include "Descriptor.h"


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

	return STATUS_SUCCESS;
}