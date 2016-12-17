#include"struct.h"
#include"filter.h"
UNICODE_STRING devname = RTL_CONSTANT_STRING(DEVICE_NAME);
UNICODE_STRING symname = RTL_CONSTANT_STRING(SYM_NAME);

NTSTATUS MyDeviceIoControl(PDEVICE_OBJECT dev, PIRP irp);
VOID Unload(PDRIVER_OBJECT driver)
{
	DbgBreakPoint();
	if (Global.DriverHandle)
	{
		NdisFDeregisterFilterDriver(Global.DriverHandle);
	}
	if (Global.FilterDev)
	{
		IoDeleteSymbolicLink(&symname);
		IoDeleteDevice(Global.FilterDev);
	}
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	NDIS_FILTER_DRIVER_CHARACTERISTICS      FChars;
	NDIS_STATUS sta;
	NDIS_STRING FriendlyName = RTL_CONSTANT_STRING(L"Zlz NDIS KernelMode Driver");
	NDIS_STRING UniqueName = RTL_CONSTANT_STRING(NETCFGGUID);
	NDIS_STRING ServiceName = RTL_CONSTANT_STRING(SERVICENAME);
	Global.contextnum = 0;
	Global.DriverHandle = NULL;
	Global.FilterDev = NULL;
	driver->DriverUnload = Unload;
	devcon = driver->MajorFunction[IRP_MJ_DEVICE_CONTROL];

	NdisZeroMemory(&FChars, sizeof(NDIS_FILTER_DRIVER_CHARACTERISTICS));
	FChars.Header.Type = NDIS_OBJECT_TYPE_FILTER_DRIVER_CHARACTERISTICS;
	FChars.Header.Size = sizeof(NDIS_FILTER_DRIVER_CHARACTERISTICS);
#if NDIS_SUPPORT_NDIS61
	FChars.Header.Revision = NDIS_FILTER_CHARACTERISTICS_REVISION_2;
#else
	FChars.Header.Revision = NDIS_FILTER_CHARACTERISTICS_REVISION_1;
#endif
	FChars.MajorNdisVersion = FILTER_MAJOR_NDIS_VERSION;
	FChars.MinorNdisVersion = FILTER_MINOR_NDIS_VERSION;
	FChars.FriendlyName = FriendlyName;
	FChars.UniqueName = UniqueName;
	FChars.ServiceName = ServiceName;
	FChars.CancelSendNetBufferListsHandler = FilterCancelSendNetBufferLists;
	FChars.NetPnPEventHandler = FilterNetPnPEvent;
	FChars.AttachHandler = FilterAttach;
	FChars.CancelDirectOidRequestHandler = NULL;
	FChars.CancelOidRequestHandler = NULL;
	FChars.DetachHandler = FilterDetach;
	FChars.DevicePnPEventNotifyHandler = NULL;
	FChars.DirectOidRequestCompleteHandler = NULL;
	FChars.DirectOidRequestHandler = NULL;
	FChars.Flags = 0;
	FChars.OidRequestCompleteHandler = NULL;
	FChars.OidRequestHandler = NULL;
	FChars.PauseHandler = FilterPause;
	FChars.ReceiveNetBufferListsHandler = FilterReceiveNetBufferLists;
	FChars.RestartHandler = FilterRestart;
	FChars.ReturnNetBufferListsHandler = FilterReturnNetBufferLists;
	FChars.SendNetBufferListsCompleteHandler = FilterSendNetBufferListsComplete;
	FChars.SendNetBufferListsHandler = FilterSendNetBufferLists;
	FChars.SetFilterModuleOptionsHandler = FilterSetModuleOptions;
	FChars.SetOptionsHandler = FilterSetOptions;
	FChars.StatusHandler = NULL;
	sta = NdisFRegisterFilterDriver(driver, NULL, &FChars, &Global.DriverHandle);
	DbgPrint("0x%x\n", sta);
	if (!NT_SUCCESS(sta))
	{
		return STATUS_UNSUCCESSFUL;
	}
	IoCreateDevice(driver, 0, &devname, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &Global.FilterDev);

	if (Global.FilterDev)
	{
		IoCreateSymbolicLink(&symname, &devname);
	}
	return 0;
}
NTSTATUS MyDeviceIoControl(PDEVICE_OBJECT dev, PIRP irp)
{
	if (dev == Global.FilterDev)
	{
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		switch (sa->Parameters.DeviceIoControl.IoControlCode)
		{
		default:
			break;
		}
		return STATUS_SUCCESS;
	}
	else
	{
		return devcon(dev, irp);
	}
}