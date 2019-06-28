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

  ID:               video
  vendor:           H
  version:          5.4.3
  name:             H video playback and capture classes
  description:      Classes for playing video and capturing camera input.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_extra
  OSXFrameworks:    AVKit AVFoundation CoreMedia
  iOSFrameworks:    AVKit AVFoundation CoreMedia

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HVIDEO_H_INCLUDED

//==============================================================================
#include <gui_extra/gui_extra.h>

//=============================================================================
/** Config: HUSE_CAMERA
    Enables camera support using the CameraDevice class (Mac, Windows, iOS, Android).
*/
#ifndef HUSE_CAMERA
 #define HUSE_CAMERA 0
#endif

#ifndef HCAMERA_LOG_ENABLED
 #define HCAMERA_LOG_ENABLED 0
#endif

#if HCAMERA_LOG_ENABLED
 #define HCAMERA_LOG(x) DBG(x)
#else
 #define HCAMERA_LOG(x) {}
#endif

#if ! (HMAC || HWINDOWS || HIOS || HANDROID)
 #undef HUSE_CAMERA
#endif

//=============================================================================
/** Config: HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME
    Enables synchronisation between video playback volume and OS media volume.
    Currently supported on Android only.
 */
#ifndef HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME
 #define HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME 1
#endif

#ifndef HVIDEO_LOG_ENABLED
 #define HVIDEO_LOG_ENABLED 1
#endif

#if HVIDEO_LOG_ENABLED
 #define HVIDEO_LOG(x) DBG(x)
#else
 #define HVIDEO_LOG(x) {}
#endif

//=============================================================================
#include "playback/VideoComponent.h"
#include "capture/CameraDevice.h"
