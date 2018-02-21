# test-utils


## BalloonsTestApp
Uses the .NET NotifyIcon class to show notification icons and show balloon notifications on it.


## FullScreenWindow
Shows a "borderless maximized" window, showing that the taskbar stops being always-on-top while the fullscreen window is active, and returns to always-on-top when any other window is active.


## HardError
Call the native api NtRaiseHardError with configurable arguments.


## LoadDllAtAddress
Load system dlls, or load a test dll at a specific address.


## ShowVersion
Shows the result of various Version api's with and without a compatibility section in the manifest:
* GetVersion
* GetVersionExA
* GetVersionExW
* VerifyVersionInfoA
* VerifyVersionInfoW


## TooltipsTester
Shows proper usage of the TOOLTIPS window class, and its messages and notificatons.
For now:
* Tests balloon notification show/hide on a hWnd target

TODO:
* Test other styles of balloon tracking.
* Test non-balloon tooltips.
* Test "truncated text" helper tooltips.
