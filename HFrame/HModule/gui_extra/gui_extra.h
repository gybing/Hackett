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

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               gui_extra
  vendor:           H
  version:          5.4.3
  name:             H extended GUI classes
  description:      Miscellaneous GUI classes for specialised tasks.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_basics
  OSXFrameworks:    WebKit

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HGUI_EXTRA_H_INCLUDED

#include <gui_basics/gui_basics.h>

//==============================================================================
/** Config: HWEB_BROWSER
    This lets you disable the WebBrowserComponent class (Mac and Windows).
    If you're not using any embedded web-pages, turning this off may reduce your code size.
*/
#ifndef HWEB_BROWSER
 #define HWEB_BROWSER 1
#endif

/** Config: HENABLE_LIVE_CONSTANT_EDITOR
    This lets you turn on the HENABLE_LIVE_CONSTANT_EDITOR support (desktop only). By default
    this will be enabled for debug builds and disabled for release builds. See the documentation
    for that macro for more details.
*/
#ifndef HENABLE_LIVE_CONSTANT_EDITOR
 #if HDEBUG && ! (HIOS || HANDROID)
  #define HENABLE_LIVE_CONSTANT_EDITOR 1
 #endif
#endif

//==============================================================================
#include "documents/FileBasedDocument.h"
#include "code_editor/CodeDocument.h"
#include "code_editor/CodeEditorComponent.h"
#include "code_editor/CodeTokeniser.h"
#include "code_editor/CPlusPlusCodeTokeniser.h"
#include "code_editor/CPlusPlusCodeTokeniserFunctions.h"
#include "code_editor/XMLCodeTokeniser.h"
#include "code_editor/LuaCodeTokeniser.h"
#include "embedding/ActiveXControlComponent.h"
#include "embedding/AndroidViewComponent.h"
#include "embedding/NSViewComponent.h"
#include "embedding/UIViewComponent.h"
#include "embedding/XEmbedComponent.h"
#include "embedding/ScopedDPIAwarenessDisabler.h"
#include "misc/AppleRemote.h"
#include "misc/BubbleMessageComponent.h"
#include "misc/ColourSelector.h"
#include "misc/KeyMappingEditorComponent.h"
#include "misc/PreferencesPanel.h"
#include "misc/PushNotifications.h"
#include "misc/RecentlyOpenedFilesList.h"
#include "misc/SplashScreen.h"
#include "misc/SystemTrayIconComponent.h"
#include "misc/WebBrowserComponent.h"
#include "misc/LiveConstantEditor.h"
#include "misc/AnimatedAppComponent.h"
