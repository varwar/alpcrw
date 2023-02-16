#include "common.h"

extern HANDLE Connect(IN LPCWSTR ServerName);
extern void InitFakeStructs();
constexpr auto ServerName = L"\\RPC Control\\DNSResolver";

int main()
{
	InitFakeStructs();
	auto hServer = Connect(ServerName);
	SendMsg(hServer);
	CloseHandle(hServer);
    return 0;
}
