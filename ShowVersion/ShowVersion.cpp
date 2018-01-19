// ShowVersion.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <cassert>


WCHAR TitleText[128] = { 0 };
WCHAR CompatText[512] = { 0 };

WORD Versions[][4] = {
    { 3, 1 , IDC_31A , IDC_31W },
    { 3, 5 , IDC_35A , IDC_35W },
    { 3, 51, IDC_351A, IDC_351W},
    { 4, 0 , IDC_40A , IDC_40W },
    { 5, 0 , IDC_50A , IDC_50W },
    { 5, 1 , IDC_51A , IDC_51W },
    { 5, 2 , IDC_52A , IDC_52W },
    { 6, 0 , IDC_60A , IDC_60W },
    { 6, 1 , IDC_61A , IDC_61W },
    { 6, 2 , IDC_62A , IDC_62W },
    { 6, 3 , IDC_63A , IDC_63W },
    {10, 0 , IDC_100A, IDC_100W},
};

static void WinVerOrGTA(HWND hDlg, DWORD nID, WORD wMajorVersion, WORD wMinorVersion)
{
    BOOL IsWinOrGT;

    OSVERSIONINFOEXA osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
    DWORD dwTypeMask;
    DWORDLONG dwlConditionMask = 0;

    osvi.dwOSVersionInfoSize = sizeof(osvi);

    dwTypeMask = VER_MAJORVERSION;
    osvi.dwMajorVersion = wMajorVersion;
    dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    osvi.dwMinorVersion = wMinorVersion;
    dwTypeMask |= VER_MINORVERSION;
    dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    IsWinOrGT = VerifyVersionInfoA(&osvi, dwTypeMask, dwlConditionMask);

    CheckDlgButton(hDlg, nID, IsWinOrGT ? BST_INDETERMINATE : BST_UNCHECKED);
}

static void WinVerOrGTW(HWND hDlg, DWORD nID, WORD wMajorVersion, WORD wMinorVersion)
{
    BOOL IsWinOrGT;
    OSVERSIONINFOEXW osvi = { sizeof(osvi), wMajorVersion, wMinorVersion, 0, 0,{ 0 }, 0, 0 };
    DWORD dwTypeMask = VER_MAJORVERSION | VER_MINORVERSION;

    DWORDLONG dwlConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    IsWinOrGT = VerifyVersionInfoW(&osvi, dwTypeMask, dwlConditionMask);

    CheckDlgButton(hDlg, nID, IsWinOrGT ? BST_INDETERMINATE : BST_UNCHECKED);
}


void PrintA(HWND hDlg, DWORD nID, OSVERSIONINFOA* osver)
{
    char Buffer[512];
    bool bEx = osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA);
    StringCchPrintfA(Buffer, _countof(Buffer), "GetVersion%sA:\r\n   %d.%d.%d Plat:%d\r\n",
        bEx ? "Ex" : "", osver->dwMajorVersion, osver->dwMinorVersion, osver->dwBuildNumber, osver->dwPlatformId);

    if (osver->szCSDVersion[0])
    {
        StringCchCatA(Buffer, _countof(Buffer), "   ");
        StringCchCatA(Buffer, _countof(Buffer), osver->szCSDVersion);
        StringCchCatA(Buffer, _countof(Buffer), "\r\n");
    }

    HWND hEdit = GetDlgItem(hDlg, nID);
    int index = GetWindowTextLengthA(hEdit);
    SetFocus(hEdit);

    SendMessageA(hEdit, EM_SETSEL, (WPARAM)index, (LPARAM)index);
    SendMessageA(hEdit, EM_REPLACESEL, 0, (LPARAM)Buffer);
}

void PrintW(HWND hDlg, DWORD nID, OSVERSIONINFOW* osver)
{
    wchar_t Buffer[512];
    bool bEx = osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXW);
    StringCchPrintfW(Buffer, _countof(Buffer), L"GetVersion%sW:\r\n   %d.%d.%d Plat:%d\r\n",
        bEx ? L"Ex" : L"", osver->dwMajorVersion, osver->dwMinorVersion, osver->dwBuildNumber, osver->dwPlatformId);

    if (osver->szCSDVersion[0])
    {
        StringCchCatW(Buffer, _countof(Buffer), L"   ");
        StringCchCatW(Buffer, _countof(Buffer), osver->szCSDVersion);
        StringCchCatW(Buffer, _countof(Buffer), L"\r\n");
    }

    HWND hEdit = GetDlgItem(hDlg, nID);
    int index = GetWindowTextLengthW(hEdit);
    SetFocus(hEdit);

    SendMessageW(hEdit, EM_SETSEL, (WPARAM)index, (LPARAM)index);
    SendMessageW(hEdit, EM_REPLACESEL, 0, (LPARAM)Buffer);
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetWindowTextW(hDlg, TitleText);
        SetDlgItemTextW(hDlg, IDC_COMPAT_TEXT, CompatText);
        for (SIZE_T n = 0; n < _countof(Versions); ++n)
        {
            assert((IDC_31A + n) == Versions[n][2]);
            assert((IDC_31W + n) == Versions[n][3]);
            WinVerOrGTA(hDlg, IDC_31A + n, Versions[n][0], Versions[n][1]);
            WinVerOrGTW(hDlg, IDC_31W + n, Versions[n][0], Versions[n][1]);
        }

        {
            OSVERSIONINFOA osa = { sizeof(osa) };
            OSVERSIONINFOW osw = { sizeof(osw) };
            OSVERSIONINFOEXA osexa = { sizeof(osexa) };
            OSVERSIONINFOEXW osexw = { sizeof(osexw) };
            DWORD dwVersion = GetVersion();
            GetVersionExA(&osa);
            GetVersionExW(&osw);
            GetVersionExA((LPOSVERSIONINFOA)&osexa);
            GetVersionExW((LPOSVERSIONINFOW)&osexw);

            WCHAR Buf[100];
            StringCchPrintfW(Buf, _countof(Buf), L"GetVersion:\r\n   0x%x\r\n", dwVersion);
            SetDlgItemTextW(hDlg, IDC_GETVERSION, Buf);

            PrintA(hDlg, IDC_GETVERSION, &osa);
            PrintW(hDlg, IDC_GETVERSION, &osw);

            PrintA(hDlg, IDC_GETVERSION, (LPOSVERSIONINFOA)&osexa);
            PrintW(hDlg, IDC_GETVERSION, (LPOSVERSIONINFOW)&osexw);
        }

        {
            DWORD(WINAPI *pGetVersion)();
            BOOL(WINAPI *pGetVersionExA)(LPOSVERSIONINFOA lpVersionInformation);
            BOOL(WINAPI *pGetVersionExW)(LPOSVERSIONINFOW lpVersionInformation);
            HMODULE mod = GetModuleHandleW(L"kernel32.dll");

            pGetVersion = (DWORD (WINAPI*)())GetProcAddress(mod, "GetVersion");
            pGetVersionExA = (BOOL(WINAPI*)(LPOSVERSIONINFOA))GetProcAddress(mod, "GetVersionExA");
            pGetVersionExW = (BOOL(WINAPI*)(LPOSVERSIONINFOW))GetProcAddress(mod, "GetVersionExW");

            {
                OSVERSIONINFOA osa = { sizeof(osa) };
                OSVERSIONINFOW osw = { sizeof(osw) };
                OSVERSIONINFOEXA osexa = { sizeof(osexa) };
                OSVERSIONINFOEXW osexw = { sizeof(osexw) };

                DWORD dwVersion = pGetVersion();
                pGetVersionExA(&osa);
                pGetVersionExW(&osw);
                pGetVersionExA((LPOSVERSIONINFOA)&osexa);
                pGetVersionExW((LPOSVERSIONINFOW)&osexw);

                WCHAR Buf[100];
                StringCchPrintfW(Buf, _countof(Buf), L"GetVersion:\r\n   0x%x\r\n", dwVersion);
                SetDlgItemTextW(hDlg, IDC_GETVERSION_GPA, Buf);

                PrintA(hDlg, IDC_GETVERSION_GPA, &osa);
                PrintW(hDlg, IDC_GETVERSION_GPA, &osw);

                PrintA(hDlg, IDC_GETVERSION_GPA, (LPOSVERSIONINFOA)&osexa);
                PrintW(hDlg, IDC_GETVERSION_GPA, (LPOSVERSIONINFOW)&osexw);
            }
        }


        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RELAUNCH)
        {
            WCHAR Buffer[MAX_PATH], TmpFile[MAX_PATH];
            GetTempPathW(_countof(Buffer), Buffer);
            GetTempFileNameW(Buffer, NULL, 0, TmpFile);

            GetModuleFileNameW(NULL, Buffer, _countof(Buffer));

            CopyFileW(Buffer, TmpFile, FALSE);

            HINSTANCE hInst = GetModuleHandleW(NULL);
            HRSRC hrSrc = FindResourceW(hInst, MAKEINTRESOURCEW(IDR_BINARY1), L"BINARY");
            if (hrSrc)
            {
                HGLOBAL hRes = LoadResource(hInst, hrSrc);
                if (hRes)
                {
                    DWORD dwSize = SizeofResource(hInst, hrSrc);

                    StringCchCopyW(Buffer, _countof(Buffer), TmpFile);
                    StringCchCatW(Buffer, _countof(Buffer), L".manifest");


                    HANDLE hFile = CreateFileW(Buffer, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile != INVALID_HANDLE_VALUE)
                    {
                        DWORD dwResult;
                        WriteFile(hFile, LockResource(hRes), dwSize, &dwResult, NULL);
                        CloseHandle(hFile);


                        {
                            PROCESS_INFORMATION ProcessInformation = { 0 };
                            STARTUPINFO StartupInfo = { 0 };
                            StartupInfo.cb = sizeof(StartupInfo);

                            if (CreateProcessW(TmpFile, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInformation))
                            {
                                CloseHandle(ProcessInformation.hThread);
                                WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
                                CloseHandle(ProcessInformation.hProcess);
                            }
                        }
                        DeleteFileW(Buffer);
                    }
                }
            }

            DeleteFileW(TmpFile);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


#ifndef _DEBUG
int(*p_vsnprintf)(char *str, size_t count, const char *fmt, va_list arg);
int _vsnprintf(char *str, size_t count, const char *fmt, va_list arg)
{
    if (!p_vsnprintf)
    {
        *(void**)&p_vsnprintf = GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "_vsnprintf");
    }
    return p_vsnprintf(str, count, fmt, arg);
}

int(*p_vsnwprintf)(wchar_t  *str, size_t count, const wchar_t *fmt, va_list arg);
int _vsnwprintf(wchar_t  *str, size_t count, const wchar_t *fmt, va_list arg)
{
    if (!p_vsnwprintf)
    {
        *(void**)&p_vsnwprintf = GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "_vsnwprintf");
    }
    return p_vsnwprintf(str, count, fmt, arg);
}


#pragma function(memset)
void* __cdecl memset(void* src, int val, size_t count)
{
    char *char_src = (char *)src;

    while(count>0) {
        *char_src = (char)val;
        char_src++;
        count--;
    }
    return src;
}
#endif


// Prevent clashes with MS headers by dropping this in a namespace
namespace impl
{
    typedef enum {
        ACTCTX_RUN_LEVEL_UNSPECIFIED = 0,
        ACTCTX_RUN_LEVEL_AS_INVOKER,
        ACTCTX_RUN_LEVEL_HIGHEST_AVAILABLE,
        ACTCTX_RUN_LEVEL_REQUIRE_ADMIN,
        ACTCTX_RUN_LEVEL_NUMBERS
    } ACTCTX_REQUESTED_RUN_LEVEL;

    typedef struct _ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION {
        ULONG                      ulFlags;
        ACTCTX_REQUESTED_RUN_LEVEL RunLevel;
        ULONG                      UiAccess;
    } ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION, *PACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION;

#define RunlevelInformationInActivationContext 5

    static WCHAR* RunLevel2Str(ACTCTX_REQUESTED_RUN_LEVEL lvl)
    {
        switch (lvl)
        {
        case ACTCTX_RUN_LEVEL_UNSPECIFIED: return L"Unspecified";
        case ACTCTX_RUN_LEVEL_AS_INVOKER: return L"AsInvoker";
        case ACTCTX_RUN_LEVEL_HIGHEST_AVAILABLE: return L"HighestAvailable";
        case ACTCTX_RUN_LEVEL_REQUIRE_ADMIN: return L"RequireAdmin";
        case ACTCTX_RUN_LEVEL_NUMBERS: return L"Numbers";
        default: return L"ERR";
        }
    }


    typedef enum {
        ACTCX_COMPATIBILITY_ELEMENT_TYPE_UNKNOWN = 0,
        ACTCX_COMPATIBILITY_ELEMENT_TYPE_OS
    } ACTCTX_COMPATIBILITY_ELEMENT_TYPE;

    typedef struct _COMPATIBILITY_CONTEXT_ELEMENT {
        GUID                              Id;
        ACTCTX_COMPATIBILITY_ELEMENT_TYPE Type;
    } COMPATIBILITY_CONTEXT_ELEMENT, *PCOMPATIBILITY_CONTEXT_ELEMENT;

    typedef struct _ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION {
        DWORD                         ElementCount;
        COMPATIBILITY_CONTEXT_ELEMENT Elements[20]; /* Officially this is undefined size, but for simplicity use a default */
    } ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION, *PACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION;

#define CompatibilityInformationInActivationContext 6


    GUID XP_CONTEXT_GUID = { 0xbeb1b341, 0x6837, 0x4c83,{ 0x83, 0x66, 0x2b, 0x45, 0x1e, 0x7c, 0xe6, 0x9b } };
    GUID VISTA_CONTEXT_GUID = { 0xe2011457, 0x1546, 0x43c5,{ 0xa5, 0xfe, 0x00, 0x8d, 0xee, 0xe3, 0xd3, 0xf0 } };
    GUID WIN7_CONTEXT_GUID = { 0x35138b9a, 0x5d96, 0x4fbd,{ 0x8e, 0x2d, 0xa2, 0x44, 0x02, 0x25, 0xf9, 0x3a } };
    GUID WIN8_CONTEXT_GUID = { 0x4a2f28e3, 0x53b9, 0x4441,{ 0xba, 0x9c, 0xd6, 0x9d, 0x4a, 0x4a, 0x6e, 0x38 } };
    GUID WINBLUE_CONTEXT_GUID = { 0x1f676c76, 0x80e1, 0x4239,{ 0x95, 0xbb, 0x83, 0xd0, 0xf6, 0xd0, 0xda, 0x78 } };
    GUID WINTHRESHOLD_CONTEXT_GUID = { 0x8e0f7a12, 0xbfb3, 0x4fe8,{ 0xb9, 0xa5, 0x48, 0xfd, 0x50, 0xa1, 0x5a, 0x9a } };


    static void QueryInfo()
    {
        SIZE_T Returned;

        StringCchCopyW(TitleText, _countof(TitleText), L"ShowVersion");

        ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION runLevel = { 0 };
        if (::QueryActCtxW(QUERY_ACTCTX_FLAG_USE_ACTIVE_ACTCTX, NULL, 0, RunlevelInformationInActivationContext, &runLevel, sizeof(runLevel), &Returned))
        {
            WCHAR Tmp[100];
            StringCchPrintfW(Tmp, _countof(Tmp), L", RunLevel:%s, UiAccess:%u", RunLevel2Str(runLevel.RunLevel), runLevel.UiAccess);
            StringCchCatW(TitleText, _countof(TitleText), Tmp);
        }
        else
        {

        }

        ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION compatInfo = { 0 };
        if (::QueryActCtxW(QUERY_ACTCTX_FLAG_USE_ACTIVE_ACTCTX, NULL, 0, CompatibilityInformationInActivationContext, &compatInfo, sizeof(compatInfo), &Returned))
        {
            StringCchCopyW(CompatText, _countof(CompatText), L"Compat: ");

            for (DWORD n = 0; n < compatInfo.ElementCount; ++n)
            {
                if (n)
                    StringCchCatW(CompatText, _countof(CompatText), L", ");
                if (compatInfo.Elements[n].Id == XP_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"WinXP");
                }
                else if (compatInfo.Elements[n].Id == VISTA_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Vista");
                }
                else if (compatInfo.Elements[n].Id == WIN7_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Win7");
                }
                else if (compatInfo.Elements[n].Id == WIN8_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Win8");
                }
                else if (compatInfo.Elements[n].Id == WINBLUE_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Win8.1");
                }
                else if (compatInfo.Elements[n].Id == WINTHRESHOLD_CONTEXT_GUID)
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Win10");
                }
                else
                {
                    StringCchCatW(CompatText, _countof(CompatText), L"Unknown GUID");
                }
            }
        }
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    impl::QueryInfo();


    DialogBox(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
    ExitProcess(0);
}
