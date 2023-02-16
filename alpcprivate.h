#pragma once
#include <windef.h>
#include <winternl.h>

typedef struct _EPROCESS* PEPROCESS;
typedef struct _ETHREAD* PETHREAD;

struct _EX_PUSH_LOCK {
	union {
		__int64 Value;
		VOID* Ptr;
		struct {
			__int64 Locked : 1;
			__int64 Waiting : 1;
			__int64 Waking : 1;
			__int64 MultipleShared : 1;
			__int64 Shared : 60;
		};
	};
};

struct _ALPC_HANDLE_ENTRY {
	VOID* Object;
};

struct _ALPC_HANDLE_TABLE {
	_ALPC_HANDLE_ENTRY* Handles;
	_EX_PUSH_LOCK Lock;
	__int64 TotalHandles;
	DWORD Flags;
};

#define _ALPC_PORT ULONG_PTR 

typedef struct _PORT_MESSAGE {
	union
	{
		struct
		{
			SHORT DataLength;
			SHORT TotalLength;
		}s1;
		DWORD Length;
	}u1;

	union
	{

		struct
		{
			SHORT Type;
			SHORT DataInfoOffset;
		}s2;
		DWORD ZeroInit;
	}u2;

	union {
		_CLIENT_ID ClientId;
		float DoNotUseThisField;
	};
	DWORD MessageId;
	union {
		__int64 ClientViewSize;
		DWORD CallbackId;
	};
}PORT_MESSAGE, *PPORT_MESSAGE;

#define _KALPC_SECURITY_DATA ULONG_PTR
#define _KALPC_VIEW ULONG_PTR
#define _KALPC_HANDLE_DATA ULONG_PTR

struct _KALPC_DIRECT_EVENT {
	union {
		__int64 Value;
		struct {
			__int64 DirectType : 1;
			__int64 EventReferenced : 1;
			__int64 EventObjectBits : 62;
		};
	};
};

struct _ALPC_WORK_ON_BEHALF_TICKET {
	DWORD ThreadId;
	DWORD ThreadCreationTimeLow;
};

struct _KALPC_WORK_ON_BEHALF_DATA {
	_ALPC_WORK_ON_BEHALF_TICKET Ticket;
};

struct _KALPC_MESSAGE_ATTRIBUTES {
	VOID* ClientContext;
	VOID* ServerContext;
	VOID* PortContext;
	VOID* CancelPortContext;
	_KALPC_SECURITY_DATA* SecurityData;
	_KALPC_VIEW* View;
	_KALPC_HANDLE_DATA* HandleData;
	_KALPC_DIRECT_EVENT DirectEvent;
	_KALPC_WORK_ON_BEHALF_DATA WorkOnBehalfData;
};

#define _ALPC_COMMUNICATION_INFO ULONG_PTR

struct _KALPC_RESERVE {
	_ALPC_PORT* OwnerPort;
	_ALPC_HANDLE_TABLE* HandleTable;
	VOID* Handle;
	VOID* Message;
	__int64 Size;
	DWORD Active;
};
//#define _KALPC_RESERVE ULONG_PTR

struct _KALPC_MESSAGE {
	_LIST_ENTRY Entry;
	_ALPC_PORT* PortQueue;
	_ALPC_PORT* OwnerPort;
	_ETHREAD* WaitingThread;
	union
	{
		struct
		{
			unsigned int QueueType : 3;
			unsigned int QueuePortType : 4;
			unsigned int Canceled : 1;
			unsigned int Ready : 1;
			unsigned int ReleaseMessage : 1;
			unsigned int SharedQuota : 1;
			unsigned int ReplyWaitReply : 1;
			unsigned int OwnerPortReference : 1;
			unsigned int ReceiverReference : 1;
			unsigned int ViewAttributeRetrieved : 1;
			unsigned int ViewAttributeDeleteOnRelease : 1;
			unsigned int InDispatch : 1;
			unsigned int InCanceledQueue : 1;
		}s1;
		DWORD State;
	}u1;

	DWORD SequenceNo;
	union {
		_EPROCESS* QuotaProcess;
		VOID* QuotaBlock;
	};
	_ALPC_PORT* CancelSequencePort;
	_ALPC_PORT* CancelQueuePort;
	DWORD CancelSequenceNo;
	_LIST_ENTRY CancelListEntry;
	_KALPC_RESERVE* Reserve;
	_KALPC_MESSAGE_ATTRIBUTES MessageAttributes;
	VOID* DataUserVa;
	_ALPC_COMMUNICATION_INFO* CommunicationInfo;
	_ALPC_PORT* ConnectionPort;
	_ETHREAD* ServerThread;
	VOID* WakeReference;
	VOID* WakeReference2;
	VOID* ExtensionBuffer;
	__int64 ExtensionBufferSize;
	_PORT_MESSAGE PortMessage;
};

#define _PALPC_PORT_REFERENCE_WAIT_BLOCK ULONG_PTR
#define _KQUEUE ULONG_PTR
#define _ALPC_COMPLETION_PACKET_LOOKASIDE ULONG_PTR
#define _KALPC_SECURITY_DATA ULONG_PTR
#define _KALPC_VIEW ULONG_PTR
#define _KALPC_HANDLE_DATA ULONG_PTR
#define _KALPC_WORK_ON_BEHALF_DATA 
