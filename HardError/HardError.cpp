// HardError.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "HardError.h"
#include <strsafe.h>


typedef enum HardErrorResponseType {
    ResponseTypeAbortRetryIgnore,
    ResponseTypeOK,
    ResponseTypeOKCancel,
    ResponseTypeRetryCancel,
    ResponseTypeYesNo,
    ResponseTypeYesNoCancel,
    ResponseTypeShutdownSystem,
    ResponseTypeTrayNotify,
    ResponseTypeCancelTryAgainContinue
} HardErrorResponseType;

typedef enum HardErrorResponse {
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort, ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes
} HardErrorResponse;

typedef enum HardErrorResponseButton {
    ResponseButtonOK,
    ResponseButtonOKCancel,
    ResponseButtonAbortRetryIgnore,
    ResponseButtonYesNoCancel,
    ResponseButtonYesNo,
    ResponseButtonRetryCancel,
    ResponseButtonCancelTryAgainContinue
} HardErrorResponseButton;

typedef enum HardErrorResponseDefaultButton {
    DefaultButton1 = 0,
    DefaultButton2 = 0x100,
    DefaultButton3 = 0x200
} HardErrorResponseDefaultButton;

typedef enum HardErrorResponseIcon {
    IconAsterisk = 0x40,
    IconError = 0x10,
    IconExclamation = 0x30,
    IconHand = 0x10,
    IconInformation = 0x40,
    IconNone = 0,
    IconQuestion = 0x20,
    IconStop = 0x10,
    IconWarning = 0x30,
    IconUserIcon = 0x80
} HardErrorResponseIcon;

typedef enum HardErrorResponseOptions {
    ResponseOptionNone = 0,
    ResponseOptionDefaultDesktopOnly = 0x20000,
    ResponseOptionHelp = 0x4000,
    ResponseOptionRightAlign = 0x80000,
    ResponseOptionRightToLeftReading = 0x100000,
    ResponseOptionTopMost = 0x40000,
    ResponseOptionServiceNotification = 0x00200000,
    ResponseOptionServiceNotificationNT3X = 0x00040000,
    ResponseOptionSetForeground = 0x10000,
    ResponseOptionSystemModal = 0x1000,
    ResponseOptionTaskModal = 0x2000,
    ResponseOptionNoFocus = 0x00008000
} HardErrorResponseOptions;


typedef struct _UNICODE_STRING_HE {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING_HE, *PUNICODE_STRING_HE;

typedef LONG NTSTATUS;

typedef void (NTAPI *tRtlInitUnicodeString)(PUNICODE_STRING_HE DestinationString, PCWSTR SourceString);
typedef NTSTATUS(NTAPI *tNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, HardErrorResponseType ValidResponseOptions, HardErrorResponse *Response);

#define STATUS_SERVICE_NOTIFICATION ((NTSTATUS)0x40000018L)
#define HARDERROR_OVERRIDE_ERRORMODE           0x10000000

inline
static HardErrorResponse DisplayNativeMessageBox(
    PCWSTR Message,
    PCWSTR Title,
    HardErrorResponseIcon Icon,
    HardErrorResponseButton Button,
    HardErrorResponseDefaultButton DefButton,
    HardErrorResponseOptions Options,
    HardErrorResponseType ResponseType)
{
    static tRtlInitUnicodeString pRtlInitUnicodeString = NULL;
    static tNtRaiseHardError pNtRaiseHardError = NULL;
    UNICODE_STRING_HE UnicodeTitle, UnicodeText;
    HardErrorResponse Response = ResponseNotHandled;
    ULONG_PTR params[4] = {
        (ULONG_PTR)&UnicodeText,
        (ULONG_PTR)&UnicodeTitle,
        (
            (ULONG)Button |
            (ULONG)Icon |
            (ULONG)Options |
            (ULONG)DefButton
            ),
        INFINITE
    };

    if (!pRtlInitUnicodeString)
    {
        pRtlInitUnicodeString = (tRtlInitUnicodeString)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlInitUnicodeString");
    }
    if (!pNtRaiseHardError)
    {
        pNtRaiseHardError = (tNtRaiseHardError)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtRaiseHardError");
    }

    pRtlInitUnicodeString(&UnicodeTitle, Title);
    pRtlInitUnicodeString(&UnicodeText, Message);

    pNtRaiseHardError(STATUS_SERVICE_NOTIFICATION | HARDERROR_OVERRIDE_ERRORMODE, _countof(params), 1 | 2, params, ResponseType, &Response);
    return Response;
}


struct OptionInfo
{
    UINT Value;
    int nID;
    const char* Name;
};

OptionInfo ResponseTypeInfo[] = {
    { ResponseTypeAbortRetryIgnore,         IDC_RADIO1, "ResponseTypeAbortRetryIgnore" },
    { ResponseTypeOK,                       IDC_RADIO2, "ResponseTypeOK" },
    { ResponseTypeOKCancel,                 IDC_RADIO3, "ResponseTypeOKCancel" },
    { ResponseTypeRetryCancel,              IDC_RADIO4, "ResponseTypeRetryCancel" },
    { ResponseTypeYesNo,                    IDC_RADIO5, "ResponseTypeYesNo" },
    { ResponseTypeYesNoCancel,              IDC_RADIO6, "ResponseTypeYesNoCancel" },
    { ResponseTypeShutdownSystem,           IDC_RADIO7, "ResponseTypeShutdownSystem" },
    { ResponseTypeTrayNotify,               IDC_RADIO8, "ResponseTypeTrayNotify" },
    { ResponseTypeCancelTryAgainContinue ,  IDC_RADIO9, "ResponseTypeCancelTryAgainContinue" },
    { 0, 0, 0 }
};


OptionInfo ResponseButtonInfo[] = {
    { ResponseButtonOK,                     IDC_RADIO10, "ResponseButtonOK" },
    { ResponseButtonOKCancel,               IDC_RADIO11, "ResponseButtonOKCancel" },
    { ResponseButtonAbortRetryIgnore,       IDC_RADIO12, "ResponseButtonAbortRetryIgnore" },
    { ResponseButtonYesNoCancel,            IDC_RADIO13, "ResponseButtonYesNoCancel" },
    { ResponseButtonYesNo,                  IDC_RADIO14, "ResponseButtonYesNo" },
    { ResponseButtonRetryCancel,            IDC_RADIO15, "ResponseButtonRetryCancel" },
    { ResponseButtonCancelTryAgainContinue, IDC_RADIO16, "ResponseButtonCancelTryAgainContinue" },
    { 0, 0, 0 }
};

OptionInfo ResponseDefaultButtonInfo[] = {
    { DefaultButton1,                       IDC_RADIO17, "DefaultButton1" },
    { DefaultButton2,                       IDC_RADIO18, "DefaultButton2" },
    { DefaultButton3,                       IDC_RADIO19, "DefaultButton3" },
    { 0, 0, 0 }
};

OptionInfo ResponseIconInfo[] = {
    { IconAsterisk,     IDC_RADIO20, "IconAsterisk" },
    { IconError,        IDC_RADIO21, "IconError" },
    { IconExclamation,  IDC_RADIO22, "IconExclamation" },
    { IconHand,         IDC_RADIO23, "IconHand" },
    { IconInformation,  IDC_RADIO24, "IconInformation" },
    { IconNone,         IDC_RADIO25, "IconNone" },
    { IconQuestion,     IDC_RADIO26, "IconQuestion" },
    { IconStop,         IDC_RADIO27, "IconStop" },
    { IconWarning,      IDC_RADIO28, "IconWarning" },
    { IconUserIcon,     IDC_RADIO29, "IconUserIcon" },
    { 0, 0, 0 }
};


OptionInfo ResponseOptionInfo[] = {
    { ResponseOptionNone,                       IDC_RADIO30, "ResponseOptionNone" },
    { ResponseOptionDefaultDesktopOnly,         IDC_RADIO31, "ResponseOptionDefaultDesktopOnly" },
    { ResponseOptionHelp,                       IDC_RADIO32, "ResponseOptionHelp" },
    { ResponseOptionRightAlign,                 IDC_RADIO33, "ResponseOptionRightAlign" },
    { ResponseOptionRightToLeftReading,         IDC_RADIO34, "ResponseOptionRightToLeftReading" },
    { ResponseOptionTopMost,                    IDC_RADIO35, "ResponseOptionTopMost" },
    { ResponseOptionServiceNotification,        IDC_RADIO36, "ResponseOptionServiceNotification" },
    { ResponseOptionServiceNotificationNT3X,    IDC_RADIO37, "ResponseOptionServiceNotificationNT3X" },
    { ResponseOptionSetForeground,              IDC_RADIO38, "ResponseOptionSetForeground" },
    { ResponseOptionSystemModal,                IDC_RADIO39, "ResponseOptionSystemModal" },
    { ResponseOptionTaskModal,                  IDC_RADIO40, "ResponseOptionTaskModal" },
    { ResponseOptionNoFocus,                    IDC_RADIO41, "ResponseOptionNoFocus" },
    { 0, 0, 0 }
};



const char* Response2Str(HardErrorResponse response)
{
    switch (response)
    {
    case ResponseReturnToCaller: return "ResponseReturnToCaller";
    case ResponseNotHandled: return "ResponseNotHandled";
    case ResponseAbort: return "ResponseAbort";
    case ResponseCancel: return "ResponseCancel";
    case ResponseIgnore: return "ResponseIgnore";
    case ResponseNo: return "ResponseNo";
    case ResponseOk: return "ResponseOk";
    case ResponseRetry: return "ResponseRetry";
    case ResponseYes: return "ResponseYes";
    default:
        __debugbreak();
        return "";
    }
}


static UINT ParseOption(HWND hDlg, OptionInfo Options[], char* Buffer, size_t Cch)
{
    UINT Value = 0;
    bool first = true;
    for (size_t n = 0; Options[n].Name; ++n)
    {
        if (IsDlgButtonChecked(hDlg, Options[n].nID) == BST_CHECKED)
        {
            Value |= Options[n].Value;
            if (!first)
                StringCchCatA(Buffer, Cch, " | ");
            first = false;
            StringCchCatA(Buffer, Cch, Options[n].Name);
        }
    }
    return Value;
}


static void ParseOptions(HWND hDlg, bool execute)
{
    char TypeText[MAX_PATH] = "";
    char ButtonText[MAX_PATH] = "";
    char DefBtnText[MAX_PATH] = "";
    char IconText[MAX_PATH] = "";
    char OptionText[MAX_PATH] = "";


    HardErrorResponseType ResponseType = (HardErrorResponseType)ParseOption(hDlg, ResponseTypeInfo, TypeText, _countof(TypeText));
    HardErrorResponseButton ResponseButton = (HardErrorResponseButton)ParseOption(hDlg, ResponseButtonInfo, ButtonText, _countof(ButtonText));
    HardErrorResponseDefaultButton ResponseDefBtn = (HardErrorResponseDefaultButton)ParseOption(hDlg, ResponseDefaultButtonInfo, DefBtnText, _countof(DefBtnText));
    HardErrorResponseIcon ResponseIcon = (HardErrorResponseIcon)ParseOption(hDlg, ResponseIconInfo, IconText, _countof(IconText));
    HardErrorResponseOptions ResponseOptions = (HardErrorResponseOptions)ParseOption(hDlg, ResponseOptionInfo, OptionText, _countof(OptionText));

    HardErrorResponse Response = ResponseNotHandled;
    const char* ResponseText = "???";

    if (execute)
    {
        WCHAR Title[MAX_PATH];
        WCHAR Text[MAX_PATH];

        GetDlgItemTextW(hDlg, IDC_TITLE, Title, _countof(Title));
        GetDlgItemTextW(hDlg, IDC_TEXT, Text, _countof(Text));

        Response = DisplayNativeMessageBox(Text, Title, ResponseIcon, ResponseButton, ResponseDefBtn, ResponseOptions, ResponseType);
        ResponseText = Response2Str(Response);
    }


    char AllText[MAX_PATH * 5];
    StringCchPrintfA(AllText, _countof(AllText),
                     "ULONG Flags = %s | %s | %s | %s;\r\n"
                     "ULONG_PTR params[4] = { UnicodeText, UnicodeTitle, Flags, INFINITE };\r\n"
                     "NTSTATUS Status = STATUS_SERVICE_NOTIFICATION | HARDERROR_OVERRIDE_ERRORMODE;\r\n"
                     "NtRaiseHardError(Status, 4, 3, params, %s, &Response);\r\n"
                     "// Response = %s",
                     ButtonText,
                     IconText,
                     DefBtnText,
                     OptionText,
                     TypeText,
                     ResponseText);

    SetDlgItemTextA(hDlg, IDC_OUTPUT, AllText);

}



INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO10, BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO17, BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO20, BST_CHECKED);
        SetDlgItemTextA(hDlg, IDC_TITLE, "Title");
        SetDlgItemTextA(hDlg, IDC_TEXT, "Text");
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_GO)
        {
            ParseOptions(hDlg, true);
            break;
        }
        else if (LOWORD(wParam) >= IDC_RADIO1 && LOWORD(wParam) <= IDC_RADIO42)
        {
            ParseOptions(hDlg, false);
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
    return 0;
}
