#include <iostream>
#include <windows.h>
#include <conio.h>
#include <winternl.h>

#define	DEF_DLL_PATH ".\\KeyBoardHook.dll"

typedef void (*PFN_HOOKSTART)();    // �Լ� ������ ����
typedef void (*PFN_HOOKSTOP)();     // �Լ� ������ ����
void NTAPI TLS_CALLBACK(PVOID DllHandel, DWORD Reason, PVOID Reserved);
void Proc_Exit_By_Debug();
bool Check_Debugging_By_PEB_BeingDebugged();
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
    cout << "������� �����Ǿ� ���α׷��� �����մϴ�." << endl;
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


int main()
{
    HMODULE			hDll = NULL;
    PFN_HOOKSTART	HookStart = NULL;
    PFN_HOOKSTOP	HookStop = NULL;

    // KeyBoardHook.dll �ε�
    hDll = LoadLibraryA(DEF_DLL_PATH);
    if (hDll == NULL)
    {
        cout << "LoadLibrary(" << DEF_DLL_PATH << ") failed!!! [" << dec << GetLastError() << "]";
        return -1;
    }

    // export �Լ� �ּ� ���
    HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, "HookStart");
    HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, "HookStop");

    // ��ŷ ����
    HookStart();

    // ����ڰ� 'q' �� �Է��� ������ ���
    cout << "press 'q' to quit!" << endl;
    while (_getch() != 'q');

    // ��ŷ ����
    HookStop();

    // KeyBoardHook.dll ��ε�
    FreeLibrary(hDll);

    return 0;
}