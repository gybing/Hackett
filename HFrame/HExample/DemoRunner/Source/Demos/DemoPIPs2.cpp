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

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../Assets/DemoUtilities.h"
#include "HDemos.h"

#include "../../../Assets/AudioLiveScrollingDisplay.h"

//==============================================================================
#include "../../../GUI/AnimationAppDemo.h"
#include "../../../GUI/AnimationDemo.h"
#include "../../../GUI/BouncingBallWavetableDemo.h"
#if HUSE_CAMERA && ! HLINUX
 #include "../../../GUI/CameraDemo.h"
#endif
#if ! HANDROID
 #include "../../../GUI/CodeEditorDemo.h"
#endif
#include "../../../GUI/ComponentDemo.h"
#include "../../../GUI/ComponentTransformsDemo.h"
#include "../../../GUI/DialogsDemo.h"
#include "../../../GUI/FlexBoxDemo.h"
#include "../../../GUI/FontsDemo.h"
#include "../../../GUI/GraphicsDemo.h"
#if HHAS_CONSTEXPR
 #include "../../../GUI/GridDemo.h"
#endif
#include "../../../GUI/ImagesDemo.h"
#include "../../../GUI/KeyMappingsDemo.h"
#include "../../../GUI/LookAndFeelDemo.h"
#include "../../../GUI/MDIDemo.h"
#include "../../../GUI/MenusDemo.h"
#include "../../../GUI/MultiTouchDemo.h"
#if HOPENGL
 #include "../../../GUI/OpenGLAppDemo.h"
 #include "../../../GUI/OpenGLDemo.h"
 #include "../../../GUI/OpenGLDemo2D.h"
#endif
#include "../../../GUI/PropertiesDemo.h"
#if ! HLINUX
 #include "../../../GUI/VideoDemo.h"
#endif
#include "../../../GUI/WebBrowserDemo.h"
#include "../../../GUI/WidgetsDemo.h"
#include "../../../GUI/WindowsDemo.h"

void registerDemos_Two() noexcept
{
    REGISTER_DEMO (AnimationAppDemo,          GUI, false)
    REGISTER_DEMO (AnimationDemo,             GUI, false)
    REGISTER_DEMO (BouncingBallWavetableDemo, GUI, false)
   #if HUSE_CAMERA && ! HLINUX
    REGISTER_DEMO (CameraDemo,                GUI, true)
   #endif
   #if ! HANDROID
    REGISTER_DEMO (CodeEditorDemo,            GUI, false)
   #endif
    REGISTER_DEMO (ComponentDemo,             GUI, false)
    REGISTER_DEMO (ComponentTransformsDemo,   GUI, false)
    REGISTER_DEMO (DialogsDemo,               GUI, false)
    REGISTER_DEMO (FlexBoxDemo,               GUI, false)
    REGISTER_DEMO (FontsDemo,                 GUI, false)
    REGISTER_DEMO (GraphicsDemo,              GUI, false)
   #if HHAS_CONSTEXPR
    REGISTER_DEMO (GridDemo,                  GUI, false)
   #endif
    REGISTER_DEMO (ImagesDemo,                GUI, false)
    REGISTER_DEMO (KeyMappingsDemo,           GUI, false)
    REGISTER_DEMO (LookAndFeelDemo,           GUI, false)
    REGISTER_DEMO (MDIDemo,                   GUI, false)
    REGISTER_DEMO (MenusDemo,                 GUI, false)
    REGISTER_DEMO (MultiTouchDemo,            GUI, false)
   #if HOPENGL
    REGISTER_DEMO (OpenGLAppDemo,             GUI, true)
    REGISTER_DEMO (OpenGLDemo2D,              GUI, true)
    REGISTER_DEMO (OpenGLDemo,                GUI, true)
   #endif
    REGISTER_DEMO (PropertiesDemo,            GUI, false)
   #if ! HLINUX
    REGISTER_DEMO (VideoDemo,                 GUI, true)
   #endif
    REGISTER_DEMO (WebBrowserDemo,            GUI, true)
    REGISTER_DEMO (WidgetsDemo,               GUI, false)
    REGISTER_DEMO (WindowsDemo,               GUI, false)
}

CodeEditorComponent::ColourScheme getDarkColourScheme()
{
    return getDarkCodeEditorColourScheme();
}

CodeEditorComponent::ColourScheme getLightColourScheme()
{
    return getLightCodeEditorColourScheme();
}
