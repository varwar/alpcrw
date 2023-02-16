#pragma once
#include "common.h"

NTSTATUS SendMsg(IN HANDLE hConn);

typedef struct _ALPC_PORT_ATTRIBUTES
{
    ULONG Flags;
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    SIZE_T MaxMessageLength;
    SIZE_T MemoryBandwidth;
    SIZE_T MaxPoolUsage;
    SIZE_T MaxSectionSize;
    SIZE_T MaxViewSize;
    SIZE_T MaxTotalSectionSize;
    ULONG DupObjectTypes;
    ULONG Reserved;
} ALPC_PORT_ATTRIBUTES, *PALPC_PORT_ATTRIBUTES;

constexpr auto ALPC_MESSAGE_SECURITY_ATTRIBUTE = 	0x80000000;
constexpr auto ALPC_MESSAGE_VIEW_ATTRIBUTE = 		0x40000000;
constexpr auto ALPC_MESSAGE_CONTEXT_ATTRIBUTE = 	0x20000000;
constexpr auto ALPC_MESSAGE_HANDLE_ATTRIBUTE = 		0x10000000;

constexpr auto ALPC_ATTRFLG_ALLOCATEDATTR = 		0x20000000;
constexpr auto ALPC_ATTRFLG_VALIDATTR = 			0x40000000;
constexpr auto ALPC_ATTRFLG_KEEPRUNNINGATTR = 		0x60000000;

typedef struct _ALPC_MESSAGE_ATTRIBUTES
{
    ULONG AllocatedAttributes;
    ULONG ValidAttributes;
} ALPC_MESSAGE_ATTRIBUTES, *PALPC_MESSAGE_ATTRIBUTES;

constexpr auto ALPC_MSGFLG_REPLY_MESSAGE = 			0x1;
constexpr auto ALPC_MSGFLG_LPC_MODE = 				0x2;
constexpr auto ALPC_MSGFLG_RELEASE_MESSAGE = 		0x10000; // dbg
constexpr auto ALPC_MSGFLG_SYNC_REQUEST = 			0x20000; // dbg;
constexpr auto ALPC_MSGFLG_TRACK_PORT_REFERENCES =	0x40000;
constexpr auto ALPC_MSGFLG_WAIT_USER_MODE = 		0x100000;
constexpr auto ALPC_MSGFLG_WAIT_ALERTABLE = 		0x200000;
constexpr auto ALPC_MSGFLG_WOW64_CALL = 			0x80000000; // dbg;

using NtAlpcConnectPort = NTSTATUS(WINAPI*)(
	_Out_ PHANDLE PortHandle,
	_In_ PUNICODE_STRING PortName,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
	_In_ ULONG Flags,
	_In_opt_ PSID RequiredServerSid,
	_Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) _PORT_MESSAGE* ConnectionMessage,
	_Inout_opt_ PULONG BufferLength,
	_Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
	_Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
	_In_opt_ PLARGE_INTEGER Timeout
	);


using NtAlpcSendWaitReceivePort = NTSTATUS(WINAPI*)(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_opt_(SendMessage->u1.s1.TotalLength) _PORT_MESSAGE *SendMessage,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
    _Out_writes_bytes_to_opt_(*BufferLength, *BufferLength) _PORT_MESSAGE *ReceiveMessage,
    _Inout_opt_ PULONG BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
    );

using AlpcInitializeMessageAttribute = NTSTATUS(WINAPI*)(
    _In_ ULONG AttributeFlags,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ ULONG BufferSize,
    _Out_ PULONG RequiredBufferSize
    );

// This is a size of user-mode buffer which is dereferencing later
// in a kernel _KALPC_MESSAGE->DataUserVa 
constexpr auto TEST_MESSAGE_SIZE = 0x1;

// Test ALPC client message
typedef struct _TALPC_PORTMSG {
	PORT_MESSAGE Header;
	ULONG64 Data[TEST_MESSAGE_SIZE];
}TALPC_PORTMSG, *PTALPC_PORTMSG;

