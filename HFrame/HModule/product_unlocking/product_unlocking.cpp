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

#ifdef HPRODUCT_UNLOCKING_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_JNI_HELPERS    1
#define HCORE_INCLUDE_OBJC_HELPERS   1
#define HCORE_INCLUDE_NATIVE_HEADERS 1

// Set this flag to 1 to use test servers on iOS
#ifndef HIN_APP_PURCHASES_USE_SANDBOX_ENVIRONMENT
    #define HIN_APP_PURCHASES_USE_SANDBOX_ENVIRONMENT 0
#endif

#include "product_unlocking.h"

#if HIOS || HMAC
 #import <StoreKit/StoreKit.h>
#endif

#if HIN_APP_PURCHASES
 #if HANDROID
  #include "native/android_InAppPurchases.cpp"
 #elif HIOS || HMAC
  #include "native/ios_InAppPurchases.cpp"
 #endif

 #include "in_app_purchases/InAppPurchases.cpp"
#endif

#include "marketplace/OnlineUnlockStatus.cpp"

#include "marketplace/TracktionMarketplaceStatus.cpp"

#include "marketplace/OnlineUnlockForm.cpp"