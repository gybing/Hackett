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

  ID:               osc
  vendor:           H
  version:          5.4.3
  name:             H OSC classes
  description:      Open Sound Control implementation.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     core, events

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HOSC_H_INCLUDED

#include <core/core.h>
#include <events/events.h>

//==============================================================================
#include "osc/OSCTypes.h"
#include "osc/OSCTimeTag.h"
#include "osc/OSCArgument.h"
#include "osc/OSCAddress.h"
#include "osc/OSCMessage.h"
#include "osc/OSCBundle.h"
#include "osc/OSCReceiver.h"
#include "osc/OSCSender.h"
