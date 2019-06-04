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

  ID:               graphics
  vendor:           H
  version:          5.4.3
  name:             H graphics classes
  description:      Classes for 2D vector graphics, image loading/saving, font handling, etc.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     events
  OSXFrameworks:    Cocoa QuartzCore
  iOSFrameworks:    CoreGraphics CoreImage CoreText QuartzCore
  linuxPackages:    x11 xinerama xext freetype2

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HGRAPHICS_H_INCLUDED

#include <core/core.h>
#include <events/events.h>

//==============================================================================
/** Config: HUSE_COREIMAGE_LOADER

    On OSX, enabling this flag means that the CoreImage codecs will be used to load
    PNG/JPEG/GIF files. It is enabled by default, but you may want to disable it if
    you'd rather use libpng, libjpeg, etc.
*/
#ifndef HUSE_COREIMAGE_LOADER
 #define HUSE_COREIMAGE_LOADER 1
#endif

/** Config: HUSE_DIRECTWRITE

    Enabling this flag means that DirectWrite will be used when available for font
    management and layout.
*/
#ifndef HUSE_DIRECTWRITE
 #define HUSE_DIRECTWRITE 1
#endif

/** Config: HDISABLE_COREGRAPHICS_FONT_SMOOTHING

    Setting this flag will turn off CoreGraphics font smoothing, which some people
    find makes the text too 'fat' for their taste.
*/
#ifndef HDISABLE_COREGRAPHICS_FONT_SMOOTHING
 #define HDISABLE_COREGRAPHICS_FONT_SMOOTHING 0
#endif

#ifndef HINCLUDE_PNGLIB_CODE
 #define HINCLUDE_PNGLIB_CODE 1
#endif

#ifndef HINCLUDE_JPEGLIB_CODE
 #define HINCLUDE_JPEGLIB_CODE 1
#endif

#ifndef USE_COREGRAPHICS_RENDERING
 #define USE_COREGRAPHICS_RENDERING 1
#endif

//==============================================================================
namespace H
{
    class Image;
    class AffineTransform;
    class Path;
    class Font;
    class Graphics;
    class FillType;
    class LowLevelGraphicsContext;
}

#include "geometry/AffineTransform.h"
#include "geometry/Point.h"
#include "geometry/Line.h"
#include "geometry/Rectangle.h"
#include "geometry/Parallelogram.h"
#include "placement/Justification.h"
#include "geometry/Path.h"
#include "geometry/RectangleList.h"
#include "colour/PixelFormats.h"
#include "colour/Colour.h"
#include "colour/ColourGradient.h"
#include "colour/Colours.h"
#include "geometry/BorderSize.h"
#include "geometry/EdgeTable.h"
#include "geometry/PathIterator.h"
#include "geometry/PathStrokeType.h"
#include "placement/RectanglePlacement.h"
#include "images/ImageCache.h"
#include "images/ImageConvolutionKernel.h"
#include "images/ImageFileFormat.h"
#include "fonts/Typeface.h"
#include "fonts/Font.h"
#include "fonts/AttributedString.h"
#include "fonts/GlyphArrangement.h"
#include "fonts/TextLayout.h"
#include "fonts/CustomTypeface.h"
#include "contexts/GraphicsContext.h"
#include "contexts/LowLevelGraphicsContext.h"
#include "images/Image.h"
#include "colour/FillType.h"
#include "native/RenderingHelpers.h"
#include "contexts/LowLevelGraphicsSoftwareRenderer.h"
#include "contexts/LowLevelGraphicsPostScriptRenderer.h"
#include "effects/ImageEffectFilter.h"
#include "effects/DropShadowEffect.h"
#include "effects/GlowEffect.h"

#if HGRAPHICS_INCLUDE_COREGRAPHICS_HELPERS && (HMAC || HIOS)
 #include "native/mac_CoreGraphicsHelpers.h"
 #include "native/mac_CoreGraphicsContext.h"
#endif

#if HDIRECT2D && HWINDOWS
#include "native/win32_Direct2DGraphicsContext.h"
#endif
