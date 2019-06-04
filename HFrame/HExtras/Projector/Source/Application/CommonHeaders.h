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

#pragma once


//==============================================================================
struct TargetOS
{
    enum OS
    {
        windows = 0,
        osx,
        linux,
        unknown
    };

    static OS getThisOS() noexcept
    {
       #if HWINDOWS
        return windows;
       #elif HMAC
        return osx;
       #elif HLINUX
        return linux;
       #else
        return unknown;
       #endif
    }
};

typedef TargetOS::OS DependencyPathOS;

//==============================================================================
#include "../Settings/StoredSettings.h"
#include "../Utility/UI/Icons.h"
#include "../Utility/Helpers/MiscUtilities.h"
#include "../Utility/Helpers/CodeHelpers.h"
#include "../Utility/Helpers/FileHelpers.h"
#include "../Utility/Helpers/RelativePath.h"
#include "../Utility/Helpers/ValueSourceHelpers.h"
#include "../Utility/Helpers/PresetIDs.h"
#include "CommandIDs.h"

//==============================================================================
const char* const projectItemDragType   = "Project Items";
const char* const drawableItemDragType  = "Drawable Items";
const char* const componentItemDragType = "Components";

enum ColourIds
{
    backgroundColourId                = 0x2340000,
    secondaryBackgroundColourId       = 0x2340001,
    defaultTextColourId               = 0x2340002,
    widgetTextColourId                = 0x2340003,
    defaultButtonBackgroundColourId   = 0x2340004,
    secondaryButtonBackgroundColourId = 0x2340005,
    userButtonBackgroundColourId      = 0x2340006,
    defaultIconColourId               = 0x2340007,
    treeIconColourId                  = 0x2340008,
    defaultHighlightColourId          = 0x2340009,
    defaultHighlightedTextColourId    = 0x234000a,
    codeEditorLineNumberColourId      = 0x234000b,
    activeTabIconColourId             = 0x234000c,
    inactiveTabBackgroundColourId     = 0x234000d,
    inactiveTabIconColourId           = 0x234000e,
    contentHeaderBackgroundColourId   = 0x234000f,
    widgetBackgroundColourId          = 0x2340010,
    secondaryWidgetBackgroundColourId = 0x2340011,
};
