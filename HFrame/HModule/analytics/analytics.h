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

  ID:               analytics
  vendor:           H
  version:          5.4.3
  name:             H analytics classes
  description:      Classes to collect analytics and send to destinations
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_basics

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HANALYTICS_H_INCLUDED

#include <queue>
#include <gui_basics/gui_basics.h>

#include "destinations/AnalyticsDestination.h"
#include "destinations/ThreadedAnalyticsDestination.h"
#include "analytics/Analytics.h"
#include "analytics/ButtonTracker.h"
