#include <iostream>
#include <windows.h>
#include <conio.h>

#define	DEF_DLL_PATH ".\\KeyBoardHook.dll"

using namespace std;

typedef void (*PFN_HOOKSTART)();    // 함수 포인터 정의
typedef void (*PFN_HOOKSTOP)();     // 함수 포인터 정의

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