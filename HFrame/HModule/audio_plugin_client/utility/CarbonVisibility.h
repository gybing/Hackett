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

//==============================================================================
#if HSUPPORT_CARBON && HMAC_WINDOW_VISIBITY_BODGE

/* When you wrap a WindowRef as an NSWindow, it seems to bugger up the HideWindow
   function, so when the host tries (and fails) to hide the window, this stuff catches
   the event and forces it to update.
*/
static pascal OSStatus windowVisibilityBodge (EventHandlerCallRef, EventRef e, void* user)
{
    NSWindow* hostWindow = (NSWindow*) user;

    switch (GetEventKind (e))
    {
        case kEventWindowInit:    [hostWindow display]; break;
        case kEventWindowShown:   [hostWindow orderFront: nil]; break;
        case kEventWindowHidden:  [hostWindow orderOut: nil]; break;
    }

    return eventNotHandledErr;
}

inline void attachWindowHidingHooks (Component* comp, void* hostWindowRef, NSWindow* nsWindow)
{
    const EventTypeSpec eventsToCatch[] =
    {
        { kEventClassWindow, kEventWindowInit },
        { kEventClassWindow, kEventWindowShown },
        { kEventClassWindow, kEventWindowHidden }
    };

    EventHandlerRef ref;
    InstallWindowEventHandler ((WindowRef) hostWindowRef,
                               NewEventHandlerUPP (windowVisibilityBodge),
                               GetEventTypeCount (eventsToCatch), eventsToCatch,
                               (void*) nsWindow, &ref);

    comp->getProperties().set ("carbonEventRef", String::toHexString ((pointer_sized_int) (void*) ref));
}

inline void removeWindowHidingHooks (Component* comp)
{
    if (comp != nullptr)
        RemoveEventHandler ((EventHandlerRef) (void*) (pointer_sized_int)
                              comp->getProperties() ["carbonEventRef"].toString().getHexValue64());
}

#elif HMAC
 inline void attachWindowHidingHooks (void*, void*, void*) {}
 inline void removeWindowHidingHooks (void*) {}
#endif


