/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if HWINDOWS

// This function is in win32_Windowing.cpp
extern bool offerKeyMessageToHWindow (MSG&);

namespace
{
    static HHOOK mouseWheelHook = 0, keyboardHook = 0;
    static int numHookUsers = 0;

    struct WindowsHooks
    {
        WindowsHooks()
        {
            if (numHookUsers++ == 0)
            {
                mouseWheelHook = SetWindowsHookEx (WH_MOUSE, mouseWheelHookCallback,
                                                   (HINSTANCE) Process::getCurrentModuleInstanceHandle(),
                                                   GetCurrentThreadId());

                keyboardHook = SetWindowsHookEx (WH_GETMESSAGE, keyboardHookCallback,
                                                 (HINSTANCE) Process::getCurrentModuleInstanceHandle(),
                                                 GetCurrentThreadId());
            }
        }

        ~WindowsHooks()
        {
            if (--numHookUsers == 0)
            {
                if (mouseWheelHook != 0)
                {
                    UnhookWindowsHookEx (mouseWheelHook);
                    mouseWheelHook = 0;
                }

                if (keyboardHook != 0)
                {
                    UnhookWindowsHookEx (keyboardHook);
                    keyboardHook = 0;
                }
            }
        }

        static LRESULT CALLBACK mouseWheelHookCallback (int nCode, WPARAM wParam, LPARAM lParam)
        {
            if (nCode >= 0 && wParam == WM_MOUSEWHEEL)
            {
                // using a local copy of this struct to support old mingw libraries
                struct MOUSEHOOKSTRUCTEX_  : public MOUSEHOOKSTRUCT  { DWORD mouseData; };

                auto& hs = *(MOUSEHOOKSTRUCTEX_*) lParam;

                if (auto* comp = Desktop::getInstance().findComponentAt ({ hs.pt.x, hs.pt.y }))
                    if (comp->getWindowHandle() != 0)
                        return PostMessage ((HWND) comp->getWindowHandle(), WM_MOUSEWHEEL,
                                            hs.mouseData & 0xffff0000, (hs.pt.x & 0xffff) | (hs.pt.y << 16));
            }

            return CallNextHookEx (mouseWheelHook, nCode, wParam, lParam);
        }

        static LRESULT CALLBACK keyboardHookCallback (int nCode, WPARAM wParam, LPARAM lParam)
        {
            MSG& msg = *(MSG*) lParam;

            if (nCode == HC_ACTION && wParam == PM_REMOVE
                 && offerKeyMessageToHWindow (msg))
            {
                zerostruct (msg);
                msg.message = WM_USER;
                return 1;
            }

            return CallNextHookEx (keyboardHook, nCode, wParam, lParam);
        }
    };
}

} // H namespace

#endif
