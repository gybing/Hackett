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

#include "../../core/system/TargetPlatform.h"
#include "../utility/CheckSettingMacros.h"

#if JucePlugin_Build_RTAS

// (these functions are in their own file because of problems including windows.h
// at the same time as the Digi headers)

#define _DO_NOT_DECLARE_INTERLOCKED_INTRINSICS_IN_MEMORY // (workaround for a VC build problem)

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#undef STRICT
#define STRICT
#include <intrin.h>
#include <windows.h>

#pragma pack (push, 8)
#include "../utility/IncludeModuleHeaders.h"
#pragma pack (pop)

//==============================================================================
void CALLTYPE attachSubWindow (void* hostWindow,
                                    int& titleW, int& titleH,
                                    Component* comp)
{
    using namespace H;

    RECT clientRect;
    GetClientRect ((HWND) hostWindow, &clientRect);

    titleW = clientRect.right - clientRect.left;
    titleH = jmax (0, (int) (clientRect.bottom - clientRect.top) - comp->getHeight());
    comp->setTopLeftPosition (0, titleH);

    comp->addToDesktop (0);

    HWND plugWnd = (HWND) comp->getWindowHandle();
    SetParent (plugWnd, (HWND) hostWindow);

    DWORD val = GetWindowLong (plugWnd, GWL_STYLE);
    val = (val & ~WS_POPUP) | WS_CHILD;
    SetWindowLong (plugWnd, GWL_STYLE, val);

    val = GetWindowLong ((HWND) hostWindow, GWL_STYLE);
    SetWindowLong ((HWND) hostWindow, GWL_STYLE, val | WS_CLIPCHILDREN);
}

void CALLTYPE resizeHostWindow (void* hostWindow,
                                     int& titleW, int& titleH,
                                     Component* comp)
{
    using namespace H;

    RECT clientRect, windowRect;
    GetClientRect ((HWND) hostWindow, &clientRect);
    GetWindowRect ((HWND) hostWindow, &windowRect);
    const int borderW = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
    const int borderH = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

    SetWindowPos ((HWND) hostWindow, 0, 0, 0,
                  borderW + jmax (titleW, comp->getWidth()),
                  borderH + comp->getHeight() + titleH,
                  SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

extern "C" BOOL WINAPI DllMainRTAS (HINSTANCE, DWORD, LPVOID);

extern "C" BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
        Process::setCurrentModuleInstanceHandle (instance);

    if (GetModuleHandleA ("DAE.DLL") != 0)
        return DllMainRTAS (instance, reason, reserved);

    ignoreUnused (reserved);
    return TRUE;
}

#if ! JucePlugin_EditorRequiresKeyboardFocus

namespace
{
    HWND findMDIParentOf (HWND w)
    {
        const int frameThickness = GetSystemMetrics (SM_CYFIXEDFRAME);

        while (w != 0)
        {
            HWND parent = GetParent (w);

            if (parent == 0)
                break;

            TCHAR windowType [32] = { 0 };
            GetClassName (parent, windowType, 31);

            if (String (windowType).equalsIgnoreCase ("MDIClient"))
            {
                w = parent;
                break;
            }

            RECT windowPos, parentPos;
            GetWindowRect (w, &windowPos);
            GetWindowRect (parent, &parentPos);

            int dw = (parentPos.right - parentPos.left) - (windowPos.right - windowPos.left);
            int dh = (parentPos.bottom - parentPos.top) - (windowPos.bottom - windowPos.top);

            if (dw > 100 || dh > 100)
                break;

            w = parent;

            if (dw == 2 * frameThickness)
                break;
        }

        return w;
    }
}

void CALLTYPE passFocusToHostWindow (void* hostWindow)
{
    SetFocus (findMDIParentOf ((HWND) hostWindow));
}

#endif

#endif
