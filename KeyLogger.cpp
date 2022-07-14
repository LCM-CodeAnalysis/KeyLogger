#include <iostream>
#include <windows.h>
#include <conio.h>
#include <winternl.h>

#define	DEF_DLL_PATH ".\\KeyBoardHook.dll"

typedef void (*PFN_HOOKSTART)();    // 함수 포인터 정의
typedef void (*PFN_HOOKSTOP)();     // 함수 포인터 정의
typedef NTSTATUS(WINAPI* NTQUERYINFORMATIONPROCESS)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );
void NTAPI TLS_CALLBACK(PVOID DllHandel, DWORD Reason, PVOID Reserved);
void Proc_Exit_By_Debug();
bool Check_Debugging_By_PEB_BeingDebugged();
bool Check_Debugging_By_NtQueryInformationProcess();
PPEB Get_PEB();

#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:tls_callback_func")
#else
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback_func")
#endif

#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = TLS_CALLBACK;
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif

using namespace std;

void NTAPI TLS_CALLBACK(PVOID DllHandel, DWORD Reason, PVOID Reserved) {
    
    switch (Reason) {
    case DLL_PROCESS_ATTACH:
        cout << "DLL_PROCESS_ATTACH." << endl;
        if (Check_Debugging_By_PEB_BeingDebugged()) Proc_Exit_By_Debug();
        if (Check_Debugging_By_NtQueryInformationProcess()) Proc_Exit_By_Debug();
        break;
    case DLL_THREAD_ATTACH:
        cout << "DLL_THREAD_ATTACH." << endl;
        break;
    case DLL_THREAD_DETACH:
        cout << "DLL_THREAD_DETACH." << endl;
        break;
    case DLL_PROCESS_DETACH:
        cout << "DLL_PROCESS_DETACH." << endl;
        break;
    }
}

void Proc_Exit_By_Debug() {
    cout << "디버깅이 감지되어 프로그램을 종료합니다." << endl;
    exit(0);
}

bool Check_Debugging_By_PEB_BeingDebugged() {
    PPEB PEB = NULL;
    PEB = Get_PEB();
    return PEB->BeingDebugged;
}

PPEB Get_PEB() {
    PTEB tebPtr = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
    PPEB pebPtr = tebPtr->ProcessEnvironmentBlock;
    return pebPtr;
}


bool Check_Debugging_By_NtQueryInformationProcess() {
    NTQUERYINFORMATIONPROCESS pNtQueryInformationProcess = NULL;
    pNtQueryInformationProcess = (NTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
    
    // ProcessDebugPort (0x7)
    DWORD dwDebugPort = 0;
    pNtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, &dwDebugPort, sizeof(dwDebugPort), NULL);
    if (dwDebugPort != 0x0) return true;

    // ProcessDebugObjectHandle (0x1E)
    HANDLE hDebugObject = NULL;
    pNtQueryInformationProcess(GetCurrentProcess(), (PROCESSINFOCLASS)0x1e, &hDebugObject, sizeof(hDebugObject), NULL);
    if(hDebugObject != 0x0) return true;

    // ProcessDebugFlags (0x1F)
    BOOL bDebugFlag = TRUE;
    pNtQueryInformationProcess(GetCurrentProcess(), (PROCESSINFOCLASS)0x1f, &bDebugFlag, sizeof(bDebugFlag), NULL);
    if (bDebugFlag == 0x0) return true;

    return false;
}


int main()
{
    HMODULE			hDll = NULL;
    PFN_HOOKSTART	HookStart = NULL;
    PFN_HOOKSTOP	HookStop = NULL;

    // KeyBoardHook.dll 로딩
    hDll = LoadLibraryA(DEF_DLL_PATH);
    if (hDll == NULL)
    {
        cout << "LoadLibrary(" << DEF_DLL_PATH << ") failed!!! [" << dec << GetLastError() << "]";
        return -1;
    }

    // export 함수 주소 얻기
    HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, "HookStart");
    HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, "HookStop");

    // 후킹 시작
    HookStart();

    // 사용자가 'q' 를 입력할 때까지 대기
    cout << "press 'q' to quit!" << endl;
    while (_getch() != 'q');

    // 후킹 종료
    HookStop();

    // KeyBoardHook.dll 언로딩
    FreeLibrary(hDll);

    return 0;
}