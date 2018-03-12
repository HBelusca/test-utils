// HardError.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include <strsafe.h>


/* Typedefs from Native Development Kit (NDK) */

//
// Responses for NtRaiseHardError
//
typedef enum _HARDERROR_RESPONSE_OPTION
{
    OptionAbortRetryIgnore,
    OptionOk,
    OptionOkCancel,
    OptionRetryCancel,
    OptionYesNo,
    OptionYesNoCancel,
    OptionShutdownSystem,
    OptionOkNoWait,
    OptionCancelTryContinue
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE
{
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort,
    ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes,
    ResponseTryAgain,
    ResponseContinue
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef LONG NTSTATUS;

typedef void (NTAPI *tRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
typedef NTSTATUS(NTAPI *tNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, HARDERROR_RESPONSE_OPTION ValidStyles, HARDERROR_RESPONSE *Response);

#define STATUS_SERVICE_NOTIFICATION  ((NTSTATUS)0x40000018L)
#define HARDERROR_OVERRIDE_ERRORMODE            0x10000000
#define MAXIMUM_HARDERROR_PARAMETERS            5


typedef enum HardErrorResponseButton {
    ResponseButtonOK = MB_OK,
    ResponseButtonOKCancel = MB_OKCANCEL,
    ResponseButtonAbortRetryIgnore = MB_ABORTRETRYIGNORE,
    ResponseButtonYesNoCancel = MB_YESNOCANCEL,
    ResponseButtonYesNo = MB_YESNO,
    ResponseButtonRetryCancel = MB_RETRYCANCEL,
    ResponseButtonCancelTryAgainContinue = MB_CANCELTRYCONTINUE
} HardErrorResponseButton;

typedef enum HardErrorDefaultButton {
    DefaultButton1 = MB_DEFBUTTON1,
    DefaultButton2 = MB_DEFBUTTON2,
    DefaultButton3 = MB_DEFBUTTON3,
    DefaultButton4 = MB_DEFBUTTON4,
} HardErrorDefaultButton;

typedef enum HardErrorIcon {
    IconNone = 0,
    IconError = MB_ICONERROR,
    IconQuestion = MB_ICONQUESTION,
    IconWarning = MB_ICONWARNING,
    IconInformation = MB_ICONINFORMATION,
    IconUserIcon = MB_USERICON
} HardErrorIcon;

typedef enum HardErrorStyle {
    StyleNone = 0,
    StyleDefaultDesktopOnly = MB_DEFAULT_DESKTOP_ONLY,
    StyleHelp = MB_HELP,
    StyleRightAlign = MB_RIGHT,
    StyleRightToLeftReading = MB_RTLREADING,
    StyleTopMost = MB_TOPMOST,
    StyleServiceNotification = MB_SERVICE_NOTIFICATION, // 0x00200000,
    StyleServiceNotificationNT3X = 0x00040000,
    StyleSetForeground = MB_SETFOREGROUND,
    StyleSystemModal = MB_SYSTEMMODAL,
    StyleTaskModal = MB_TASKMODAL,
    StyleNoFocus = MB_NOFOCUS
} HardErrorStyle;

inline static HARDERROR_RESPONSE
DisplayNativeMessageBox(
    NTSTATUS Status,
    PCWSTR Message,
    PCWSTR Title,
    PULONG_PTR pParams,
    ULONG NumberOfParams,
    HardErrorIcon Icon,
    HardErrorResponseButton Button,
    HardErrorDefaultButton DefButton,
    HardErrorStyle Style,
    HARDERROR_RESPONSE_OPTION ResponseType)
{
    static tRtlInitUnicodeString pRtlInitUnicodeString = NULL;
    static tNtRaiseHardError pNtRaiseHardError = NULL;
    UNICODE_STRING UnicodeTitle, UnicodeText;
    HARDERROR_RESPONSE Response = ResponseNotHandled;
    ULONG_PTR MsgBoxParams[4] = {
        (ULONG_PTR)&UnicodeText,
        (ULONG_PTR)&UnicodeTitle,
        (
            (ULONG)Button |
            (ULONG)Icon |
            (ULONG)Style |
            (ULONG)DefButton
        ),
        INFINITE
    };
    ULONG UnicodeStringParameterMask = 1 | 2;

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

    if ((Status & ~HARDERROR_OVERRIDE_ERRORMODE) == STATUS_SERVICE_NOTIFICATION)
    {
        /* Use parameter list for STATUS_SERVICE_NOTIFICATION message box */
        pParams = MsgBoxParams;
        NumberOfParams = _countof(MsgBoxParams);
        // UnicodeStringParameterMask = 1 | 2;
    }
    else
    {
        /* Use the user-provided parameter list */
        UnicodeStringParameterMask = 0;
    }

    pNtRaiseHardError(Status, NumberOfParams, UnicodeStringParameterMask, pParams, ResponseType, &Response);
    return Response;
}


struct OptionInfo
{
    UINT Value;
    int nID;
    const char* Name;
};

OptionInfo ResponseTypeInfo[] = {
    { OptionAbortRetryIgnore,       IDC_RADIO1, "OptionAbortRetryIgnore" },
    { OptionOk,                     IDC_RADIO2, "OptionOk" },
    { OptionOkCancel,               IDC_RADIO3, "OptionOkCancel" },
    { OptionRetryCancel,            IDC_RADIO4, "OptionRetryCancel" },
    { OptionYesNo,                  IDC_RADIO5, "OptionYesNo" },
    { OptionYesNoCancel,            IDC_RADIO6, "OptionYesNoCancel" },
    { OptionShutdownSystem,         IDC_RADIO7, "OptionShutdownSystem" },
    { OptionOkNoWait,               IDC_RADIO8, "OptionOkNoWait" },
    { OptionCancelTryContinue,      IDC_RADIO9, "OptionCancelTryContinue" },
    { 0, 0, 0 }
};

OptionInfo ResponseButtonInfo[] = {
    { ResponseButtonOK,                     IDC_RADIO10, "MB_OK" },
    { ResponseButtonOKCancel,               IDC_RADIO11, "MB_OKCANCEL" },
    { ResponseButtonAbortRetryIgnore,       IDC_RADIO12, "MB_ABORTRETRYIGNORE" },
    { ResponseButtonYesNoCancel,            IDC_RADIO13, "MB_YESNOCANCEL" },
    { ResponseButtonYesNo,                  IDC_RADIO14, "MB_YESNO" },
    { ResponseButtonRetryCancel,            IDC_RADIO15, "MB_RETRYCANCEL" },
    { ResponseButtonCancelTryAgainContinue, IDC_RADIO16, "MB_CANCELTRYCONTINUE" },
    { 0, 0, 0 }
};

OptionInfo ResponseDefaultButtonInfo[] = {
    { DefaultButton1,                       IDC_RADIO17, "MB_DEFBUTTON1" },
    { DefaultButton2,                       IDC_RADIO18, "MB_DEFBUTTON2" },
    { DefaultButton3,                       IDC_RADIO19, "MB_DEFBUTTON3" },
    { DefaultButton4,                       IDC_RADIO20, "MB_DEFBUTTON4" },
    { 0, 0, 0 }
};

OptionInfo ResponseIconInfo[] = {
    { IconNone,         IDC_RADIO21, "0" },
    { IconError,        IDC_RADIO22, "MB_ICONERROR" },
    { IconQuestion,     IDC_RADIO23, "MB_ICONQUESTION" },
    { IconWarning,      IDC_RADIO24, "MB_ICONWARNING" },
    { IconInformation,  IDC_RADIO25, "MB_ICONINFORMATION" },
    { IconUserIcon,     IDC_RADIO26, "MB_USERICON" },
    { 0, 0, 0 }
};

OptionInfo StyleInfo[] = {
    { StyleDefaultDesktopOnly,         IDC_RADIO31, "MB_DEFAULT_DESKTOP_ONLY" },
    { StyleHelp,                       IDC_RADIO32, "MB_HELP" },
    { StyleRightAlign,                 IDC_RADIO33, "MB_RIGHT" },
    { StyleRightToLeftReading,         IDC_RADIO34, "MB_RTLREADING" },
    { StyleTopMost,                    IDC_RADIO35, "MB_TOPMOST" },
    { StyleServiceNotification,        IDC_RADIO36, "MB_SERVICE_NOTIFICATION" },
    { StyleServiceNotificationNT3X,    IDC_RADIO37, "0x00040000" },
    { StyleSetForeground,              IDC_RADIO38, "MB_SETFOREGROUND" },
    { StyleSystemModal,                IDC_RADIO39, "MB_SYSTEMMODAL" },
    { StyleTaskModal,                  IDC_RADIO40, "MB_TASKMODAL" },
    { StyleNoFocus,                    IDC_RADIO41, "MB_NOFOCUS" },
    { 0, 0, 0 }
};


const char* Response2Str(HARDERROR_RESPONSE response)
{
    switch (response)
    {
    case ResponseReturnToCaller:
        return "ResponseReturnToCaller";
    case ResponseNotHandled:
        return "ResponseNotHandled";
    case ResponseAbort:
        return "ResponseAbort";
    case ResponseCancel:
        return "ResponseCancel";
    case ResponseIgnore:
        return "ResponseIgnore";
    case ResponseNo:
        return "ResponseNo";
    case ResponseOk:
        return "ResponseOk";
    case ResponseRetry:
        return "ResponseRetry";
    case ResponseYes:
        return "ResponseYes";
    case ResponseTryAgain:
        return "ResponseTryAgain";
    case ResponseContinue:
        return "ResponseContinue";
    default:
        return "???"; // itoa(response, ..., 16);
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
    if (first && Value == 0)
        StringCchCopyA(Buffer, Cch, "0");
    return Value;
}

static void ParseParams(HWND hDlg, PULONG_PTR pParams, ULONG NumberOfParams, char* Buffer, size_t Cch)
{
    // UINT nParams = GetDlgItemInt(hDlg, IDC_PARAMNUM, NULL, FALSE);
    INT nID[MAXIMUM_HARDERROR_PARAMETERS] = {IDC_PARAM0, IDC_PARAM1, IDC_PARAM2, IDC_PARAM3, IDC_PARAM4};
    bool first = true;
    for (size_t n = 0; n < NumberOfParams; ++n)
    {
        if (!first)
            StringCchCatExA(Buffer, Cch, ", ", &Buffer, &Cch, STRSAFE_IGNORE_NULLS);
        first = false;
        pParams[n] = (ULONG_PTR)GetDlgItemInt(hDlg, nID[n], NULL, FALSE);
        StringCchPrintfExA(Buffer, Cch, &Buffer, &Cch, STRSAFE_IGNORE_NULLS, "0x%lx", pParams[n]);
    }
}

static void ParseOptions(HWND hDlg, bool execute)
{
    char ParamsText[MAX_PATH] = "";
    char TypeText[MAX_PATH] = "";
    char ButtonText[MAX_PATH] = "";
    char DefBtnText[MAX_PATH] = "";
    char IconText[MAX_PATH] = "";
    char OptionText[MAX_PATH] = "";
    char StatusText[MAX_PATH] = "";

    HARDERROR_RESPONSE_OPTION ResponseType = (HARDERROR_RESPONSE_OPTION)ParseOption(hDlg, ResponseTypeInfo, TypeText, _countof(TypeText));
    HardErrorResponseButton ResponseButton = (HardErrorResponseButton)ParseOption(hDlg, ResponseButtonInfo, ButtonText, _countof(ButtonText));
    HardErrorDefaultButton ResponseDefBtn = (HardErrorDefaultButton)ParseOption(hDlg, ResponseDefaultButtonInfo, DefBtnText, _countof(DefBtnText));
    HardErrorIcon ResponseIcon = (HardErrorIcon)ParseOption(hDlg, ResponseIconInfo, IconText, _countof(IconText));
    HardErrorStyle Styles = (HardErrorStyle)ParseOption(hDlg, StyleInfo, OptionText, _countof(OptionText));
    NTSTATUS Status = STATUS_SERVICE_NOTIFICATION | HARDERROR_OVERRIDE_ERRORMODE;
    UINT nParams = 0;
    ULONG_PTR params[MAXIMUM_HARDERROR_PARAMETERS] = {0, 0, 0, 0, 0};

    HARDERROR_RESPONSE Response = ResponseNotHandled;
    const char* ResponseText = "???";

    GetDlgItemTextA(hDlg, IDC_STATUS, StatusText, _countof(StatusText));
    if (StatusText[0])
    {
        char* ptr = NULL;
        Status = strtoul(StatusText, &ptr, 16);
        if (*ptr)
        {
            /* Invalid Status */
            Status = 0xFFFFFFFF;
        }
    }

    if ((Status & ~HARDERROR_OVERRIDE_ERRORMODE) == STATUS_SERVICE_NOTIFICATION)
    {
        nParams = 4;
        StringCchCopyA(ParamsText, _countof(ParamsText), "UnicodeText, UnicodeTitle, Flags, INFINITE");
    }
    else
    {
        nParams = GetDlgItemInt(hDlg, IDC_PARAMNUM, NULL, FALSE);
        ParseParams(hDlg, params, nParams, ParamsText, _countof(ParamsText));
    }

    if (execute)
    {
        PULONG_PTR pParams;
        ULONG NumberOfParams;
        WCHAR Title[MAX_PATH];
        WCHAR Text[MAX_PATH];

        GetDlgItemTextW(hDlg, IDC_TITLE, Title, _countof(Title));
        GetDlgItemTextW(hDlg, IDC_TEXT, Text, _countof(Text));

        if ((Status & ~HARDERROR_OVERRIDE_ERRORMODE) == STATUS_SERVICE_NOTIFICATION)
        {
            /* Everything is specified using Text, Title, and the different flags */
            pParams = NULL;
            NumberOfParams = 0;
        }
        else
        {
            /* Use the parameter list instead */
            pParams = params;
            NumberOfParams = nParams; // _countof(params);
        }

        Response = DisplayNativeMessageBox(Status, Text, Title,
                                           pParams, NumberOfParams,
                                           ResponseIcon, ResponseButton, ResponseDefBtn,
                                           Styles, ResponseType);
        ResponseText = Response2Str(Response);
    }

    if (Status == (STATUS_SERVICE_NOTIFICATION | HARDERROR_OVERRIDE_ERRORMODE))
    {
        StringCchCopyA(StatusText, _countof(StatusText), "(STATUS_SERVICE_NOTIFICATION | HARDERROR_OVERRIDE_ERRORMODE)");
    }
    else
    {
        StringCchPrintfA(StatusText, _countof(StatusText), "0x%08lx", Status);
    }

    char AllText[MAX_PATH * 5];
    if ((Status & ~HARDERROR_OVERRIDE_ERRORMODE) == STATUS_SERVICE_NOTIFICATION)
    {
        StringCchPrintfA(AllText, _countof(AllText),
                         "NTSTATUS Status = %s;\r\n"
                         "ULONG Flags = %s | %s | %s | %s;\r\n"
                         "ULONG_PTR params[%u] = { %s };\r\n"
                         "HARDERROR_RESPONSE Response;\r\n"
                         "NtRaiseHardError(Status, %u, 3, params, %s, &Response);\r\n"
                         "// Response = %s",
                         StatusText,
                         ButtonText,
                         IconText,
                         DefBtnText,
                         OptionText,
                         nParams,
                         ParamsText,
                         nParams,
                         TypeText,
                         ResponseText);
    }
    else
    {
        StringCchPrintfA(AllText, _countof(AllText),
                         "NTSTATUS Status = %s;\r\n"
                         "ULONG_PTR params[%u] = { %s };\r\n"
                         "HARDERROR_RESPONSE Response;\r\n"
                         "NtRaiseHardError(Status, %u, 0, params, %s, &Response);\r\n"
                         "// Response = %s",
                         StatusText,
                         nParams,
                         ParamsText,
                         nParams,
                         TypeText,
                         ResponseText);
    }

    SetDlgItemTextA(hDlg, IDC_OUTPUT, AllText);
}

static void
EnableParamEdits(HWND hDlg, INT nParams)
{
    SetDlgItemInt(hDlg, IDC_PARAMNUM, nParams, FALSE);

    EnableWindow(GetDlgItem(hDlg, IDC_PARAM0), (nParams >= 1));
    EnableWindow(GetDlgItem(hDlg, IDC_PARAM1), (nParams >= 2));
    EnableWindow(GetDlgItem(hDlg, IDC_PARAM2), (nParams >= 3));
    EnableWindow(GetDlgItem(hDlg, IDC_PARAM3), (nParams >= 4));
    EnableWindow(GetDlgItem(hDlg, IDC_PARAM4), (nParams >= 5));
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        CheckDlgButton(hDlg, IDC_RADIO1 , BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO10, BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO17, BST_CHECKED);
        CheckDlgButton(hDlg, IDC_RADIO25, BST_CHECKED);
        SetDlgItemTextA(hDlg, IDC_TITLE, "Title");
        SetDlgItemTextA(hDlg, IDC_TEXT , "Text");

        SendDlgItemMessage(hDlg, IDC_PARAMNUM_SPIN, UDM_SETRANGE, 0, MAKELPARAM(MAXIMUM_HARDERROR_PARAMETERS, 1));
        SetDlgItemInt(hDlg, IDC_PARAMNUM, 4, FALSE);
        EnableParamEdits(hDlg, 4);
        ParseOptions(hDlg, false);

        return (INT_PTR)TRUE;
    }

    case WM_NOTIFY:
    {
        if (((LPNMHDR)lParam)->code == UDN_DELTAPOS)
        {
            LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;
            UINT nParams = (UINT)(lpnmud->iPos + lpnmud->iDelta);

            if (1 <= nParams && nParams <= MAXIMUM_HARDERROR_PARAMETERS)
            {
                EnableParamEdits(hDlg, nParams);
                ParseOptions(hDlg, false);
                return (INT_PTR)0;
            }
            else
            {
                return (INT_PTR)1;
            }
        }
        break;
    }

    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_KILLFOCUS)
        {
            if (LOWORD(wParam) == IDC_PARAMNUM)
            {
                UINT nParams = GetDlgItemInt(hDlg, IDC_PARAMNUM, NULL, FALSE);
                nParams = min(max(nParams , 1), MAXIMUM_HARDERROR_PARAMETERS);
                EnableParamEdits(hDlg, nParams);
            }
            else if (LOWORD(wParam) == IDC_STATUS ||
                     LOWORD(wParam) == IDC_PARAM0 ||
                     LOWORD(wParam) == IDC_PARAM1 ||
                     LOWORD(wParam) == IDC_PARAM2 ||
                     LOWORD(wParam) == IDC_PARAM3 ||
                     LOWORD(wParam) == IDC_PARAM4)
            {
                ParseOptions(hDlg, false);
            }
        }
        else if (LOWORD(wParam) == IDC_GO)
        {
            ParseOptions(hDlg, true);
        }
        else if (LOWORD(wParam) >= IDC_RADIO1 && LOWORD(wParam) <= IDC_RADIO41)
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
