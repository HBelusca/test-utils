/*
 * PROJECT:     TestCallbacks
 * LICENSE:     GPL-2.0+ (https://spdx.org/licenses/GPL-2.0+)
 * PURPOSE:     Show debug info at various (system) callbacks.
 * COPYRIGHT:   Copyright 2018 Mark Jansen (mark.jansen@reactos.org)
 */

#include "stdafx.h"
#include <TlHelp32.h>

// Computer\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TestCallbacks.exe

static HWND g_Dialog = NULL;
static HANDLE g_Heap;
static ULONG g_Message;


static void log_fn(const char* fmt, ...)
{
    char* ptr;
    int len;
    va_list args;

    va_start(args, fmt);

    len = _vscprintf(fmt, args);
    if (len < 0)
    {
        va_end(args);
        return;
    }
    ptr = (char*)HeapAlloc(g_Heap, HEAP_ZERO_MEMORY, len + 1);
    StringCbVPrintfA(ptr, len+1, fmt, args);
    va_end(args);

    OutputDebugStringA(ptr);
    if (g_Dialog)
        PostMessage(g_Dialog, g_Message, 0L, (LPARAM)ptr);
    else
        HeapFree(g_Heap, 0, ptr);
}

#define log(fmt, ...) log_fn("[%5x]: " fmt "\n", GetCurrentThreadId(), __VA_ARGS__)


static void EnumModules(bool (*PFN)(MODULEENTRY32&, PVOID), PVOID Ptr)
{
    MODULEENTRY32 me = { sizeof(me) };
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        DWORD dwErr = GetLastError();
        log("CreateToolhelp32Snapshot failed with 0x%x", dwErr);
        return;
    }
    if (Module32First(hSnap, &me))
    {
        do
        {
            if (!PFN(me, Ptr))
                break;
        } while (Module32Next(hSnap, &me));
    }
    CloseHandle(hSnap);
}

struct addr_2_mod
{
    PVOID Addr;
    std::wstring Mod;
};

bool FindModule(MODULEENTRY32& me, PVOID Ptr)
{
    addr_2_mod* a2m = (addr_2_mod*)Ptr;
    if (a2m->Addr >= me.modBaseAddr &&
        a2m->Addr <= (me.modBaseAddr + me.modBaseSize))
    {
        a2m->Mod = me.szModule;
        return false;
    }
    return true;
}

std::wstring Addr2Mod(PVOID Addr)
{
    addr_2_mod tmp;
    tmp.Addr = Addr;
    EnumModules(FindModule, &tmp);
    return tmp.Mod;
}


static DWORD WINAPI ReturnThreadProc0(IN LPVOID lpParameter)
{
    PVOID Addr = _ReturnAddress();
    log("Entering ReturnThreadProc0(%s) (ret=%p,%ws)", lpParameter, Addr, Addr2Mod(Addr).c_str());
    Sleep(5000);
    log("Returning 0");
    return 0;
}

static DWORD WINAPI ReturnThreadProc1234(IN LPVOID lpParameter)
{
    PVOID Addr = _ReturnAddress();
    log("Entering ReturnThreadProc1234(%s) (ret=%p,%ws)", lpParameter, Addr, Addr2Mod(Addr).c_str());
    Sleep(5000);
    log("Returning 1234");
    return 1234;
}

static DWORD WINAPI ExitThreadProc0(IN LPVOID lpParameter)
{
    PVOID Addr = _ReturnAddress();
    log("Entering ExitThreadProc(%s) (ret=%p,%ws)", lpParameter, Addr, Addr2Mod(Addr).c_str());
    Sleep(5000);
    log("ExitThread(0)");
    ExitThread(0);
}

static DWORD WINAPI ExitThreadProc1234(IN LPVOID lpParameter)
{
    PVOID Addr = _ReturnAddress();
    log("Entering ExitThreadProc1234(%s) (ret=%p,%ws)", lpParameter, Addr, Addr2Mod(Addr).c_str());
    Sleep(5000);
    log("ExitThread(1234)");
    ExitThread(1234);
}

bool ShowModule(MODULEENTRY32& me, PVOID)
{
    log("%p-%p %ws", me.modBaseAddr, me.modBaseAddr+me.modBaseSize, me.szModule);
    return true;
}

static void ListModules()
{
    log("== Begin module list ==");
    EnumModules(ShowModule, NULL);
    log("== End module list ==");
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == g_Message)
    {
        HWND hList = GetDlgItem(hDlg, IDC_LSTLOG);
        int index = SendMessageA(hList, LB_ADDSTRING, 0L, lParam);
        HeapFree(g_Heap, 0, (LPVOID)lParam);
        SendMessage(hList, LB_SETTOPINDEX, index, 0L);
        return 0;
    }
    switch (message)
    {
    case WM_INITDIALOG:
        CheckDlgButton(hDlg, IDC_RETURN0, BST_CHECKED);
        g_Dialog = hDlg;
        break;
    case WM_COMMAND:
    {
        LPTHREAD_START_ROUTINE lpRoutine;
        if (IsDlgButtonChecked(hDlg, IDC_RETURN0))
            lpRoutine = ReturnThreadProc0;
        else if (IsDlgButtonChecked(hDlg, IDC_RETURN1234))
            lpRoutine = ReturnThreadProc1234;
        else if (IsDlgButtonChecked(hDlg, IDC_EXITTHREAD0))
            lpRoutine = ExitThreadProc0;
        else
            lpRoutine = ExitThreadProc1234;

        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDC_BTNCREATETHREAD:
            CloseHandle(CreateThread(NULL, 0, lpRoutine, "CreateThread", 0, NULL));
            break;
        case IDC_BTNCREATEREMOTETHREAD:
            CloseHandle(CreateRemoteThread(GetCurrentProcess(), NULL, 0, lpRoutine, "CreateRemoteThread", 0, NULL));
            break;
        case IDC_BTNCREATEREMOTETHREADEX:
            CloseHandle(CreateRemoteThreadEx(GetCurrentProcess(), NULL, 0, lpRoutine, "CreateRemoteThreadEx", 0, NULL, NULL));
            break;
        case IDC_BTNLISTMOD:
            ListModules();
            break;
        case IDC_BTNCLS:
            SendDlgItemMessage(hDlg, IDC_LSTLOG, LB_RESETCONTENT, 0L, 0L);
            break;
        }
        break;
    }
    }
    return 0;
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    PVOID Addr = _ReturnAddress();
    g_Message = RegisterWindowMessageA("TEST_LOG_MESSAGE");
    g_Heap = GetProcessHeap();
    log("wWinMain(hInstance=%p, hPrevInstance=%p, lpCmdLine='%ws', nCmdShow=%i) (ret=%p,%ws)",
        hInstance, hPrevInstance, lpCmdLine, nCmdShow, Addr, Addr2Mod(Addr).c_str());

    DialogBox(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DlgProc);
    ExitProcess(0);
}
