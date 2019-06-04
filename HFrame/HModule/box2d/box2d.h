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

  ID:               box2d
  vendor:           H
  version:          5.4.3
  name:             H wrapper for the Box2D physics engine
  description:      The Box2D physics engine and some utility classes.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     graphics

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HBOX2D_H_INCLUDED

//==============================================================================
#include <graphics/graphics.h>

#ifdef __GNUC__
 #pragma GCC diagnostic push
 #pragma GCC diagnostic ignored "-Wconversion"
 #if defined (__clang__)
  #if __has_warning("-Wzero-as-null-pointer-constant")
   #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
  #endif
 #endif
#endif

#include <climits>
#include <cfloat>

#include "box2d/Box2D.h"

#ifdef __GNUC__
 #pragma GCC diagnostic pop
#endif

#ifndef DOXYGEN // for some reason, Doxygen sees this as a re-definition of Box2DRenderer
 #include "utils/Box2DRenderer.h"
#endif // DOXYGEN
