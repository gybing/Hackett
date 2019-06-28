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

  ID:               opengl
  vendor:           H
  version:          5.4.3
  name:             H OpenGL classes
  description:      Classes for rendering OpenGL in a H window.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_extra
  OSXFrameworks:    OpenGL
  iOSFrameworks:    OpenGLES
  linuxLibs:        GL
  mingwLibs:        opengl32

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HOPENGL_H_INCLUDED

#include <gui_extra/gui_extra.h>

#undef HOPENGL
#define HOPENGL 1

#if HIOS || HANDROID
 #define HOPENGL_ES 1
#endif

#if HWINDOWS
 #ifndef APIENTRY
  #define APIENTRY __stdcall
  #define CLEAR_TEMP_APIENTRY 1
 #endif
 #ifndef WINGDIAPI
  #define WINGDIAPI __declspec(dllimport)
  #define CLEAR_TEMP_WINGDIAPI 1
 #endif

 #if HMINGW
  #include <GL/gl.h>
 #else
  #include <gl/GL.h>
 #endif

 #ifdef CLEAR_TEMP_WINGDIAPI
  #undef WINGDIAPI
  #undef CLEAR_TEMP_WINGDIAPI
 #endif
 #ifdef CLEAR_TEMP_APIENTRY
  #undef APIENTRY
  #undef CLEAR_TEMP_APIENTRY
 #endif
#elif HLINUX
 #include <GL/gl.h>
 #undef KeyPress
#elif HIOS
 #if defined (__IPHONE_7_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_7_0
  #include <OpenGLES/ES3/gl.h>
 #else
  #include <OpenGLES/ES2/gl.h>
 #endif
#elif HMAC
  #define HOPENGL3 1
  #include <OpenGL/gl3.h>
  #include <OpenGL/gl3ext.h>
#elif HANDROID
 #include <android/native_window.h>
 #include <android/native_window_jni.h>
 #if HANDROID_GL_ES_VERSION_3_0
  #define HOPENGL3 1
  #include <GLES3/gl3.h>
 #else
  #include <GLES2/gl2.h>
 #endif
 #include <EGL/egl.h>
#endif

#if GL_ES_VERSION_3_0
 #define HOPENGL3 1
#endif

//==============================================================================
/** This macro is a helper for use in GLSL shader code which needs to compile on both OpenGL 2.1 and OpenGL 3.0.
    It's mandatory in OpenGL 3.0 to specify the GLSL version.
*/
#if HOPENGL3
 #if HOPENGL_ES
  #define HGLSL_VERSION "#version 300 es"
 #else
  #define HGLSL_VERSION "#version 150"
 #endif
#else
 #define HGLSL_VERSION ""
#endif

//==============================================================================
#if HOPENGL_ES || defined (DOXYGEN)
 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
 #define HMEDIUMP  "mediump"

 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
 #define HHIGHP    "highp"

 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
 #define HLOWP     "lowp"
#else
 #define HMEDIUMP
 #define HHIGHP
 #define HLOWP
#endif

//==============================================================================
namespace H
{
    class OpenGLTexture;
    class OpenGLFrameBuffer;
    class OpenGLShaderProgram;
}

#include "geometry/Vector3D.h"
#include "geometry/Matrix3D.h"
#include "geometry/Quaternion.h"
#include "geometry/Draggable3DOrientation.h"
#include "native/MissingGLDefinitions.h"
#include "opengl/OpenGLHelpers.h"
#include "opengl/OpenGLPixelFormat.h"
#include "native/OpenGLExtensions.h"
#include "opengl/OpenGLRenderer.h"
#include "opengl/OpenGLContext.h"
#include "opengl/OpenGLFrameBuffer.h"
#include "opengl/OpenGLGraphicsContext.h"
#include "opengl/OpenGLImage.h"
#include "opengl/OpenGLShaderProgram.h"
#include "opengl/OpenGLTexture.h"
#include "utils/OpenGLAppComponent.h"
