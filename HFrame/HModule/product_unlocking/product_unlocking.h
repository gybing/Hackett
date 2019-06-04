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

  ID:               product_unlocking
  vendor:           H
  version:          5.4.3
  name:             H Online marketplace support
  description:      Classes for online product authentication
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     cryptography core

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HPRODUCT_UNLOCKING_H_INCLUDED

/**
    The product_unlocking module provides simple user-registration classes
    for allowing you to build apps/plugins with features that are unlocked by a
    user having a suitable account on a webserver.

    Although originally designed for use with products that are sold on the
    Tracktion Marketplace web-store, the module itself is fully open, and can
    be used to connect to your own web-store instead, if you implement your
    own compatible web-server back-end.

    In additional, the module supports in-app purchases both on iOS and Android
    platforms.
*/

//==============================================================================
#include <core/core.h>
#include <cryptography/cryptography.h>

#include <data_structures/data_structures.h>
#include <gui_extra/gui_extra.h>

#if HIN_APP_PURCHASES
 #include "in_app_purchases/InAppPurchases.h"
#endif

#include "marketplace/OnlineUnlockStatus.h"
#include "marketplace/TracktionMarketplaceStatus.h"

#include "marketplace/KeyFileGeneration.h"
#include "marketplace/OnlineUnlockForm.h"