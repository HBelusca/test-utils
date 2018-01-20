# test-utils


## HardError
Call the native api NtRaiseHardError with configurable arguments


## ShowVersion
Shows the result of various Version api's with and without a compatibility section in the manifest:
* GetVersion
* GetVersionExA
* GetVersionExW
* VerifyVersionInfoA
* VerifyVersionInfoW


## TooltipsTest
Shows proper usage of the TOOLTIPS window class, and its messages and notificatons.
For now:
* Tests balloon notification show/hide on a hWnd target
TODO:
* Test other styles of balloon tracking.
* Test non-balloon tooltips.
* Test "truncated text" helper tooltips.