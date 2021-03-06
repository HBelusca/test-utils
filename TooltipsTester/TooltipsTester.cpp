// TooltipsTester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TooltipsTester.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int dbg_swprintf(const wchar_t * fmt, ...)
{
    wchar_t tmp[4096];
    va_list vl;

    va_start(vl, fmt);
    auto ret = vswprintf_s(tmp, _countof(tmp), fmt, vl);
    va_end(vl);

    OutputDebugStringW(tmp);

    return ret;
}

class CTooltips :
    public CWindow
{
public: // Configuration methods

    HWND Create(HWND hWndParent, DWORD dwStyles = 0, DWORD dwExStyles = 0)
    {
        if (!dwStyles)
        {
            dwStyles = WS_POPUP | TTS_NOPREFIX;
        }

        if (!dwExStyles)
        {
            dwExStyles = WS_EX_TOPMOST;
        }

        m_hWnd = CreateWindowExW(dwExStyles,
            TOOLTIPS_CLASS,
            TEXT(""),
            dwStyles,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            hWndParent,
            NULL,
            _AtlBaseModule.GetModuleInstance(),
            NULL);

        if (!m_hWnd)
            return NULL;

        SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        return m_hWnd;
    }

public: // Relay event

    // Win7+: Can use GetMessageExtraInfo to provide the WPARAM value.
    VOID RelayEvent(MSG * pMsg, WPARAM extraInfo = 0)
    {
        SendMessageW(TTM_RELAYEVENT, extraInfo, reinterpret_cast<LPARAM>(pMsg));
    }

public: // Helpers

    INT GetToolCount()
    {
        return SendMessageW(TTM_GETTOOLCOUNT, 0, 0);
    }

    BOOL AddTool(IN CONST TTTOOLINFOW * pInfo)
    {
        return SendMessageW(TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    VOID DelTool(IN HWND hwndToolOwner, IN UINT uId)
    {
        TTTOOLINFOW info = { sizeof(TTTOOLINFOW), 0 };
        info.hwnd = hwndToolOwner;
        info.uId = uId;
        SendMessageW(TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&info));
    }

    VOID NewToolRect(IN HWND hwndToolOwner, IN UINT uId, IN RECT rect)
    {
        TTTOOLINFOW info = { sizeof(TTTOOLINFOW), 0 };
        info.hwnd = hwndToolOwner;
        info.uId = uId;
        info.rect = rect;
        SendMessageW(TTM_NEWTOOLRECT, 0, reinterpret_cast<LPARAM>(&info));
    }

    BOOL GetToolInfo(IN HWND hwndToolOwner, IN UINT uId, OUT TTTOOLINFOW * pInfo)
    {
        pInfo->cbSize = sizeof(TTTOOLINFOW);
        pInfo->hwnd = hwndToolOwner;
        pInfo->uId = uId;
        return SendMessageW(TTM_GETTOOLINFO, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    VOID SetToolInfo(IN CONST TTTOOLINFOW * pInfo)
    {
        SendMessageW(TTM_SETTOOLINFO, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    BOOL HitTest(IN CONST TTHITTESTINFOW * pInfo)
    {
        return SendMessageW(TTM_HITTEST, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    VOID GetText(IN HWND hwndToolOwner, IN UINT uId, OUT PWSTR pBuffer, IN DWORD cchBuffer)
    {
        TTTOOLINFOW info = { sizeof(TTTOOLINFOW), 0 };
        info.hwnd = hwndToolOwner;
        info.uId = uId;
        info.lpszText = pBuffer;
        SendMessageW(TTM_GETTEXT, cchBuffer, reinterpret_cast<LPARAM>(&info));
    }

    VOID UpdateTipText(IN HWND hwndToolOwner, IN UINT uId, IN PCWSTR szText, IN HINSTANCE hinstResourceOwner = NULL)
    {
        TTTOOLINFOW info = { sizeof(TTTOOLINFOW), 0 };
        info.hwnd = hwndToolOwner;
        info.uId = uId;
        info.lpszText = const_cast<PWSTR>(szText);
        info.hinst = hinstResourceOwner;
        SendMessageW(TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&info));
    }

    BOOL EnumTools(IN CONST TTTOOLINFOW * pInfo)
    {
        return SendMessageW(TTM_ENUMTOOLS, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    BOOL GetCurrentTool(OUT OPTIONAL TTTOOLINFOW * pInfo = NULL)
    {
        return SendMessageW(TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(pInfo));
    }

    VOID GetTitle(TTGETTITLE * pTitleInfo)
    {
        SendMessageW(TTM_GETTITLE, 0, reinterpret_cast<LPARAM>(pTitleInfo));
    }

    BOOL SetTitle(PCWSTR szTitleText, WPARAM icon = 0)
    {
        return SendMessageW(TTM_SETTITLE, icon, reinterpret_cast<LPARAM>(szTitleText));
    }

    VOID TrackActivate(IN HWND hwndToolOwner, IN UINT uId)
    {
        TTTOOLINFOW info = { sizeof(TTTOOLINFOW), 0 };
        info.hwnd = hwndToolOwner;
        info.uId = uId;
        SendMessageW(TTM_TRACKACTIVATE, TRUE, reinterpret_cast<LPARAM>(&info));
    }

    VOID TrackDeactivate()
    {
        SendMessageW(TTM_TRACKACTIVATE, FALSE, NULL);
    }

    VOID TrackPosition(IN WORD x, IN WORD y)
    {
        SendMessageW(TTM_TRACKPOSITION, 0, MAKELPARAM(x, y));
    }

    // Opens the tooltip
    VOID Popup()
    {
        SendMessageW(TTM_POPUP);
    }

    // Closes the tooltip - Pressing the [X] for a TTF_CLOSE balloon is equivalent to calling this
    VOID Pop()
    {
        SendMessageW(TTM_POP);
    }

    // Delay times for AUTOMATIC tooltips (they don't affect balloons)
    INT GetDelayTime(UINT which)
    {
        return SendMessageW(TTM_GETDELAYTIME, which);
    }

    VOID SetDelayTime(UINT which, WORD time)
    {
        SendMessageW(TTM_SETDELAYTIME, which, MAKELPARAM(time, 0));
    }

    // Activates or deactivates the automatic tooltip display when hovering a control
    VOID Activate(IN BOOL bActivate = TRUE)
    {
        SendMessageW(TTM_ACTIVATE, bActivate);
    }

    // Adjusts the position of a tooltip when used to display trimmed text
    VOID AdjustRect(IN BOOL bActivate = TRUE)
    {
        SendMessageW(TTM_ADJUSTRECT, bActivate);
    }

    // Useful for TTF_ABSOLUTE|TTF_TRACK tooltip positioning
    SIZE GetBubbleSize(OUT TTTOOLINFOW * pInfo)
    {
        DWORD ret = SendMessageW(TTM_GETBUBBLESIZE, 0, reinterpret_cast<LPARAM>(pInfo));
        const SIZE sz = { LOWORD(ret), HIWORD(ret) };
        return sz;
    }

    // Fills the RECT with the margin size previously set. Default is 0 margins.
    VOID GetMargin(RECT * pRect)
    {
        SendMessageW(TTM_GETMARGIN, 0, reinterpret_cast<LPARAM>(pRect));
    }

    VOID SetMargin(RECT * pRect)
    {
        SendMessageW(TTM_SETMARGIN, 0, reinterpret_cast<LPARAM>(pRect));
    }

    // Gets a previously established max width. Returns -1 if no limit is set
    INT GetMaxTipWidth()
    {
        return SendMessageW(TTM_GETMAXTIPWIDTH);
    }

    INT SetMaxTipWidth(IN OPTIONAL INT width = -1)
    {
        return SendMessageW(TTM_SETMAXTIPWIDTH, 0, width);
    }

    // Get the color of the tooltip text
    COLORREF GetTipTextColor()
    {
        return SendMessageW(TTM_GETTIPTEXTCOLOR);
    }

    VOID SetTipTextColor(COLORREF textColor)
    {
        SendMessageW(TTM_SETTIPTEXTCOLOR, textColor);
    }

    COLORREF GetTipBkColor()
    {
        return SendMessageW(TTM_GETTIPBKCOLOR);
    }

    VOID SetTipBkColor(COLORREF textColor)
    {
        SendMessageW(TTM_SETTIPBKCOLOR, textColor);
    }

    VOID SetWindowTheme(IN PWSTR szThemeName)
    {
        SendMessageW(TTM_SETWINDOWTHEME, 0, reinterpret_cast<LPARAM>(szThemeName));
    }

    // Forces redraw
    VOID Update()
    {
        SendMessageW(TTM_UPDATE);
    }

    HWND TooltipWindowFromPoint(IN POINT * pPoint)
    {
        return reinterpret_cast<HWND>(SendMessageW(TTM_WINDOWFROMPOINT, 0, reinterpret_cast<LPARAM>(pPoint)));
    }
};

class CButton :
    public CWindow
{
public:
    HWND Create(HWND hWndParent, UINT cmdId, POINT position, SIZE size, PCWSTR text, DWORD dwStyles = 0, DWORD dwExStyles = 0)
    {
        if (!dwStyles)
        {
            dwStyles = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON;
        }

        m_hWnd = CreateWindowExW(dwExStyles,
            L"BUTTON",
            text,
            dwStyles,
            position.x, position.y,
            size.cx, size.cy,
            hWndParent,
            (HMENU)cmdId,
            _AtlBaseModule.GetModuleInstance(),
            NULL);

        if (!m_hWnd)
            return NULL;

        return m_hWnd;
    }

};

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_mainWindow;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CTooltips g_Tooltips;
CTooltips g_Balloons;

CButton g_Button1;
CButton g_Button2;
CButton g_Button3;

#define IDBTN_START 1000
#define IDBTN_SHOWBALLOON (IDBTN_START+1)
#define IDBTN_HIDEBALLOON (IDBTN_START+2)
#define IDBTN_DUMMYTARGET (IDBTN_START+3)

BOOL g_BallonShown = FALSE;

BOOL ShowBalloonTip()
{
    const WPARAM icon = 2;
    BOOL ret = g_Balloons.SetTitle(L"This is a test balloon", icon);
    if (!ret)
        dbg_swprintf(L"SetTitle failed, GetLastError=%d", GetLastError());

    RECT rc;
    ::GetWindowRect(g_Button3.m_hWnd, &rc);
    WORD x = (rc.left + rc.right) / 2;
    WORD y = (rc.top + rc.bottom) / 2;
    
    g_Balloons.TrackPosition(x, y);
    g_Balloons.UpdateTipText(g_mainWindow, reinterpret_cast<UINT_PTR>(g_Button3.m_hWnd), L"This is a test message in the balloon.");
    g_Balloons.TrackActivate(g_mainWindow, reinterpret_cast<UINT_PTR>(g_Button3.m_hWnd));

    g_BallonShown = TRUE;

    return TRUE;
}

VOID HideBalloonTip()
{
    if (g_BallonShown)
    {
        g_BallonShown = FALSE;
        g_Balloons.TrackDeactivate();
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    INITCOMMONCONTROLSEX iccx = {
        sizeof(INITCOMMONCONTROLSEX),
        ICC_STANDARD_CLASSES | ICC_BAR_CLASSES
    };
    InitCommonControlsEx(&iccx);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOOLTIPSTESTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOOLTIPSTESTER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOOLTIPSTESTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TOOLTIPSTESTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_Button1.Create(hWnd, IDBTN_SHOWBALLOON, { 50,50 }, { 180,40 }, L"Show Balloon");
   g_Button2.Create(hWnd, IDBTN_HIDEBALLOON, { 50,100 }, { 180,40 }, L"Hide Balloon");
   g_Button3.Create(hWnd, IDBTN_DUMMYTARGET, { 50,150 }, { 180,40 }, L"Balloon Target");
   g_Balloons.Create(hWnd, TTS_BALLOON | TTS_CLOSE);
   
   TOOLINFOW ti = { 0 };
   ti.cbSize = TTTOOLINFOW_V1_SIZE;
   ti.uFlags = TTF_TRACK | TTF_IDISHWND;
   ti.uId = reinterpret_cast<UINT_PTR>(g_Button3.m_hWnd);
   ti.hwnd = hWnd;
   ti.lpszText = NULL;
   ti.lParam = NULL;

   BOOL ret = g_Balloons.AddTool(&ti);
   if (!ret)
   {
       dbg_swprintf(L"AddTool failed");
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_mainWindow = hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDBTN_SHOWBALLOON:
                ShowBalloonTip();
                break;
            case IDBTN_HIDEBALLOON:
                HideBalloonTip();
                break;
            case IDBTN_DUMMYTARGET:
                // DO NOTHING YET
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_NOTIFY:
        {
            int id = wParam;
            LPNMHDR hdr = (LPNMHDR)lParam;
            if (hdr->code == TTN_POP)
            {
                HideBalloonTip();
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
