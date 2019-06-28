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

  ID:               data_structures
  vendor:           H
  version:          5.4.3
  name:             H data model helper classes
  description:      Classes for undo/redo management, and smart data structures.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     events

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HDATA_STRUCTURES_H_INCLUDED

//==============================================================================
#include <events/events.h>

#include "undomanager/UndoableAction.h"
#include "undomanager/UndoManager.h"
#include "values/Value.h"
#include "values/ValueTree.h"
#include "values/ValueTreeSynchroniser.h"
#include "values/CachedValue.h"
#include "values/ValueWithDefault.h"
#include "app_properties/PropertiesFile.h"
#include "app_properties/ApplicationProperties.h"
