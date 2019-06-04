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

#ifdef HAUDIO_PROCESSORS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HCORE_INCLUDE_OBJC_HELPERS 1

#include "audio_processors.h"
#include <gui_extra/gui_extra.h>

//==============================================================================
#if HMAC
 #if HSUPPORT_CARBON && (HPLUGINHOST_VST || HPLUGINHOST_AU)
  #include <Carbon/Carbon.h>
  #include "../gui_extra/native/mac_CarbonViewWrapperComponent.h"
 #endif
#endif

#if HPLUGINHOST_VST && HLINUX
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 #undef KeyPress
#endif

#if ! HWINDOWS && ! HMAC
 #undef HPLUGINHOST_VST3
 #define HPLUGINHOST_VST3 0
#endif

#if HPLUGINHOST_AU && (HMAC || HIOS)
 #include <AudioUnit/AudioUnit.h>
#endif

//==============================================================================
static inline bool arrayContainsPlugin (const OwnedArray<PluginDescription>& list,
                                        const PluginDescription& desc)
{
    for (auto* p : list)
        if (p->isDuplicateOf (desc))
            return true;

    return false;
}

#if HMAC || HIOS

#if HIOS
 #define HIOS_MAC_VIEW  UIView
 using ViewComponentBaseClass = UIViewComponent;
#else
 #define HIOS_MAC_VIEW  NSView
 using ViewComponentBaseClass = NSViewComponent;
#endif

//==============================================================================
struct AutoResizingNSViewComponent  : public ViewComponentBaseClass,
                                      private AsyncUpdater
{
    void childBoundsChanged (Component*) override  { triggerAsyncUpdate(); }
    void handleAsyncUpdate() override              { resizeToFitView(); }
};

//==============================================================================
struct AutoResizingNSViewComponentWithParent  : public AutoResizingNSViewComponent,
                                                private Timer
{
    AutoResizingNSViewComponentWithParent()
    {
        HIOS_MAC_VIEW* v = [[HIOS_MAC_VIEW alloc] init];
        setView (v);
        [v release];

        startTimer (30);
    }

    HIOS_MAC_VIEW* getChildView() const
    {
        if (HIOS_MAC_VIEW* parent = (HIOS_MAC_VIEW*) getView())
            if ([[parent subviews] count] > 0)
                return [[parent subviews] objectAtIndex: 0];

        return nil;
    }

    void timerCallback() override
    {
        if (HIOS_MAC_VIEW* child = getChildView())
        {
            stopTimer();
            setView (child);
        }
    }
};
#endif



#if HCLANG
 #pragma clang diagnostic ignored "-Wdeprecated-declarations"
 #if __has_warning("-Wcast-align")
  #pragma clang diagnostic ignored "-Wcast-align"
 #endif
#endif

#include "format/AudioPluginFormat.cpp"
#include "format/AudioPluginFormatManager.cpp"
#include "format_types/LegacyAudioParameter.cpp"
#include "processors/AudioProcessor.cpp"
#include "processors/AudioPluginInstance.cpp"
#include "processors/AudioProcessorEditor.cpp"
#include "processors/AudioProcessorGraph.cpp"
#include "processors/GenericAudioProcessorEditor.cpp"
#include "processors/PluginDescription.cpp"
#include "format_types/LADSPAPluginFormat.cpp"
#include "format_types/VSTPluginFormat.cpp"
#include "format_types/VST3PluginFormat.cpp"
#include "format_types/AudioUnitPluginFormat.mm"
#include "scanning/KnownPluginList.cpp"
#include "scanning/PluginDirectoryScanner.cpp"
#include "scanning/PluginListComponent.cpp"
#include "processors/AudioProcessorParameterGroup.cpp"
#include "utilities/AudioProcessorParameterWithID.cpp"
#include "utilities/RangedAudioParameter.cpp"
#include "utilities/AudioParameterFloat.cpp"
#include "utilities/AudioParameterInt.cpp"
#include "utilities/AudioParameterBool.cpp"
#include "utilities/AudioParameterChoice.cpp"
#include "utilities/AudioProcessorValueTreeState.cpp"
