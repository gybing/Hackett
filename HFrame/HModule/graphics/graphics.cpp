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

#ifdef HGRAPHICS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HGRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1

#include "graphics.h"

//==============================================================================
#if HMAC
 #import <QuartzCore/QuartzCore.h>

#elif HWINDOWS
  // get rid of some warnings in Window's own headers
 #ifdef HMSVC
  #pragma warning (push)
  #pragma warning (disable : 4458)
 #endif

 #if HMINGW && HUSE_DIRECTWRITE
  #warning "DirectWrite not currently implemented with mingw..."
  #undef HUSE_DIRECTWRITE
 #endif

 #if HUSE_DIRECTWRITE || HDIRECT2D
  /* If you hit a compile error trying to include these files, you may need to update
     your version of the Windows SDK to the latest one. The DirectWrite and Direct2D
     headers are in the version 7 SDKs.
  */
  #include <d2d1.h>
  #include <dwrite.h>
 #endif

 #if HMINGW
  #include <malloc.h>
  #include <cstdio>
 #endif

 #include <unordered_map>

 #ifdef HMSVC
  #pragma warning (pop)
 #endif

#elif HIOS
 #import <QuartzCore/QuartzCore.h>
 #import <CoreText/CoreText.h>

 #if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
  #error "H no longer supports targets earlier than iOS 3.2"
 #endif

#elif HLINUX
 #ifndef HUSE_FREETYPE
  #define HUSE_FREETYPE 1
 #endif
#endif

#if HUSE_FREETYPE
 #if HUSE_FREETYPE_AMALGAMATED
  #include "native/freetype/FreeTypeAmalgam.h"
 #else
  #include <ft2build.h>
  #include FT_FREETYPE_H
 #endif
#endif

#undef SIZEOF

#if (HMAC || HIOS) && USE_COREGRAPHICS_RENDERING && HUSE_COREIMAGE_LOADER
 #define HUSING_COREIMAGE_LOADER 1
#else
 #define HUSING_COREIMAGE_LOADER 0
#endif

//==============================================================================
#include "colour/Colour.cpp"
#include "colour/ColourGradient.cpp"
#include "colour/Colours.cpp"
#include "colour/FillType.cpp"
#include "geometry/AffineTransform.cpp"
#include "geometry/EdgeTable.cpp"
#include "geometry/Path.cpp"
#include "geometry/PathIterator.cpp"
#include "geometry/PathStrokeType.cpp"
#include "placement/RectanglePlacement.cpp"
#include "contexts/GraphicsContext.cpp"
#include "contexts/LowLevelGraphicsPostScriptRenderer.cpp"
#include "contexts/LowLevelGraphicsSoftwareRenderer.cpp"
#include "images/Image.cpp"
#include "images/ImageCache.cpp"
#include "images/ImageConvolutionKernel.cpp"
#include "images/ImageFileFormat.cpp"
#include "image_formats/GIFLoader.cpp"
#include "image_formats/JPEGLoader.cpp"
#include "image_formats/PNGLoader.cpp"
#include "fonts/AttributedString.cpp"
#include "fonts/Typeface.cpp"
#include "fonts/CustomTypeface.cpp"
#include "fonts/Font.cpp"
#include "fonts/GlyphArrangement.cpp"
#include "fonts/TextLayout.cpp"
#include "effects/DropShadowEffect.cpp"
#include "effects/GlowEffect.cpp"

#if HUSE_FREETYPE
 #include "native/freetype_Fonts.cpp"
#endif

//==============================================================================
#if HMAC || HIOS
 #include "native/mac_Fonts.mm"
 #include "native/mac_CoreGraphicsContext.mm"
 #include "native/mac_IconHelpers.cpp"

#elif HWINDOWS
 #include "native/win32_DirectWriteTypeface.cpp"
 #include "native/win32_DirectWriteTypeLayout.cpp"
 #include "native/win32_Fonts.cpp"
 #include "native/win32_IconHelpers.cpp"
 #if HDIRECT2D
  #include "native/win32_Direct2DGraphicsContext.cpp"
 #endif

#elif HLINUX
 #include "native/linux_Fonts.cpp"
 #include "native/linux_IconHelpers.cpp"

#elif HANDROID
 #include "native/android_GraphicsContext.cpp"
 #include "native/android_Fonts.cpp"
 #include "native/android_IconHelpers.cpp"

#endif

//==============================================================================
#if HUSE_FREETYPE && HUSE_FREETYPE_AMALGAMATED
 #undef PIXEL_MASK
 #undef ZLIB_VERSION
 #undef Z_ASCII
 #undef ZEXTERN
 #undef ZEXPORT

 extern "C"
 {
   #include "native/freetype/FreeTypeAmalgam.c"
 }
#endif
