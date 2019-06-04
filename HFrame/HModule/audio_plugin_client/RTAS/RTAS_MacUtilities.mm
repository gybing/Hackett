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

// Horrible carbon-based fix for a cocoa bug, where an NSWindow that wraps a carbon
// window fails to keep its position updated when the user drags the window around..
#define WINDOWPOSITION_BODGE 1
#define HMAC_WINDOW_VISIBITY_BODGE 1

#include "../utility/IncludeSystemHeaders.h"
#include "../utility/IncludeModuleHeaders.h"
#include "../utility/CarbonVisibility.h"

using namespace H;

//==============================================================================
void initialiseMacRTAS();
void initialiseMacRTAS()
{
   #if ! H64BIT
    NSApplicationLoad();
   #endif
}

void* attachSubWindow (void*, Component*);
void* attachSubWindow (void* hostWindowRef, Component* comp)
{
    HAUTORELEASEPOOL
    {
       #if 0
        // This was suggested as a way to improve passing keypresses to the host, but
        // a side-effect seems to be occasional rendering artifacts.
        HIWindowChangeClass ((WindowRef) hostWindowRef, kFloatingWindowClass);
       #endif

        NSWindow* hostWindow = [[NSWindow alloc] initWithWindowRef: hostWindowRef];
        [hostWindow retain];
        [hostWindow setCanHide: YES];
        [hostWindow setReleasedWhenClosed: YES];
        NSRect oldWindowFrame = [hostWindow frame];

        NSView* content = [hostWindow contentView];
        NSRect f = [content frame];
        f.size.width = comp->getWidth();
        f.size.height = comp->getHeight();
        [content setFrame: f];

        const CGFloat mainScreenHeight = [[[NSScreen screens] objectAtIndex: 0] frame].size.height;

       #if WINDOWPOSITION_BODGE
        {
            Rect winBounds;
            GetWindowBounds ((WindowRef) hostWindowRef, kWindowContentRgn, &winBounds);
            NSRect w = [hostWindow frame];
            w.origin.x = winBounds.left;
            w.origin.y = mainScreenHeight - winBounds.bottom;
            [hostWindow setFrame: w display: NO animate: NO];
        }
       #endif

        NSPoint windowPos = [hostWindow convertBaseToScreen: f.origin];
        windowPos.x = windowPos.x + jmax (0.0f, (oldWindowFrame.size.width - f.size.width) / 2.0f);
        windowPos.y = mainScreenHeight - (windowPos.y + f.size.height);

        comp->setTopLeftPosition ((int) windowPos.x, (int) windowPos.y);

       #if ! JucePlugin_EditorRequiresKeyboardFocus
        comp->addToDesktop (ComponentPeer::windowIsTemporary | ComponentPeer::windowIgnoresKeyPresses);
       #else
        comp->addToDesktop (ComponentPeer::windowIsTemporary);
       #endif

        comp->setVisible (true);

        NSView* pluginView = (NSView*) comp->getWindowHandle();
        NSWindow* pluginWindow = [pluginView window];

        [hostWindow addChildWindow: pluginWindow
                           ordered: NSWindowAbove];
        [hostWindow orderFront: nil];
        [pluginWindow orderFront: nil];

        attachWindowHidingHooks (comp, (WindowRef) hostWindowRef, hostWindow);

        return hostWindow;
    }
}

void removeSubWindow (void*, Component*);
void removeSubWindow (void* nsWindow, Component* comp)
{
    HAUTORELEASEPOOL
    {
        NSView* pluginView = (NSView*) comp->getWindowHandle();
        NSWindow* hostWindow = (NSWindow*) nsWindow;
        NSWindow* pluginWindow = [pluginView window];

        removeWindowHidingHooks (comp);
        [hostWindow removeChildWindow: pluginWindow];
        comp->removeFromDesktop();
        [hostWindow release];
    }
}

namespace
{
    bool isJuceWindow (WindowRef w)
    {
        for (int i = ComponentPeer::getNumPeers(); --i >= 0;)
        {
            ComponentPeer* peer = ComponentPeer::getPeer(i);
            NSView* view = (NSView*) peer->getNativeHandle();

            if ([[view window] windowRef] == w)
                return true;
        }

        return false;
    }
}

void forwardCurrentKeyEventToHostWindow();
void forwardCurrentKeyEventToHostWindow()
{
    WindowRef w = FrontNonFloatingWindow();
    WindowRef original = w;

    while (IsValidWindowPtr (w) && isJuceWindow (w))
    {
        w = GetNextWindowOfClass (w, kDocumentWindowClass, true);

        if (w == original)
            break;
    }

    if (! isJuceWindow (w))
    {
        ActivateWindow (w, true);
        repostCurrentNSEvent();
    }
}

#endif
