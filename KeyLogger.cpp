#include <iostream>
#include <windows.h>
#include <conio.h>

#define	DEF_DLL_PATH ".\\KeyBoardHook.dll"

using namespace std;

typedef void (*PFN_HOOKSTART)();    // �Լ� ������ ����
typedef void (*PFN_HOOKSTOP)();     // �Լ� ������ ����

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