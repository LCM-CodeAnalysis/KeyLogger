#include <iostream>
#include <windows.h>
#include <conio.h>

#define	DEF_DLL_PATH ".\\KeyBoardHook.dll"

typedef void (*PFN_HOOKSTART)();    // 함수 포인터 정의
typedef void (*PFN_HOOKSTOP)();     // 함수 포인터 정의
void NTAPI TLS_CALLBACK(PVOID DllHandel, DWORD Reason, PVOID Reserved);

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