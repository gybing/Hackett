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

#ifdef HOPENGL_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HGRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1

#include "opengl.h"

//==============================================================================
#if HIOS
 #import <QuartzCore/QuartzCore.h>

//==============================================================================
#elif HWINDOWS
 #include <windowsx.h>

 #if HMSVC && ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
  #pragma comment(lib, "OpenGL32.Lib")
 #endif

//==============================================================================
#elif HLINUX
 /* Got an include error here?

    If you want to install OpenGL support, the packages to get are "mesa-common-dev"
    and "freeglut3-dev".
 */
 #include <GL/glx.h>

//==============================================================================
#elif HMAC
 #include <OpenGL/CGLCurrent.h> // These are both just needed with the 10.5 SDK
 #include <OpenGL/OpenGL.h>

//==============================================================================
#elif HANDROID
 #ifndef GL_GLEXT_PROTOTYPES
  #define GL_GLEXT_PROTOTYPES 1
 #endif

 #if HANDROID_GL_ES_VERSION_3_0
  #include <GLES3/gl3.h>

  // workaround for a bug in SDK 18 and 19
  // see: https://stackoverflow.com/questions/31003863/gles-3-0-including-gl2ext-h
  #define __gl2_h_
  #include <GLES2/gl2ext.h>
 #else
  #include <GLES2/gl2.h>
 #endif
#endif

//==============================================================================
void OpenGLExtensionFunctions::initialise()
{
   #if HWINDOWS || HLINUX
    #define HINIT_GL_FUNCTION(name, returnType, params, callparams) \
        name = (type_ ## name) OpenGLHelpers::getExtensionFunction (#name);

    HGL_BASE_FUNCTIONS (HINIT_GL_FUNCTION)
    #undef HINIT_GL_FUNCTION

    #define HINIT_GL_FUNCTION(name, returnType, params, callparams) \
        name = (type_ ## name) OpenGLHelpers::getExtensionFunction (#name); \
        if (name == nullptr) \
            name = (type_ ## name) OpenGLHelpers::getExtensionFunction (HSTRINGIFY (name ## EXT));

    HGL_EXTENSION_FUNCTIONS (HINIT_GL_FUNCTION)
    #if HOPENGL3
     HGL_VERTEXBUFFER_FUNCTIONS (HINIT_GL_FUNCTION)
    #endif

    #undef HINIT_GL_FUNCTION
   #endif
}

#if HOPENGL_ES
 #define HDECLARE_GL_FUNCTION(name, returnType, params, callparams) \
    returnType OpenGLExtensionFunctions::name params noexcept { return ::name callparams; }

 HGL_BASE_FUNCTIONS (HDECLARE_GL_FUNCTION)
 HGL_EXTENSION_FUNCTIONS (HDECLARE_GL_FUNCTION)
#if HOPENGL3
 HGL_VERTEXBUFFER_FUNCTIONS (HDECLARE_GL_FUNCTION)
#endif

 #undef HDECLARE_GL_FUNCTION
#endif

#undef HGL_EXTENSION_FUNCTIONS

#if HDEBUG && ! defined (HCHECK_OPENGL_ERROR)
static const char* getGLErrorMessage (const GLenum e) noexcept
{
    switch (e)
    {
        case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
       #ifdef GL_STACK_OVERFLOW
        case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
       #endif
       #ifdef GL_STACK_UNDERFLOW
        case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
       #endif
       #ifdef GL_INVALID_FRAMEBUFFER_OPERATION
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
       #endif
        default: break;
    }

    return "Unknown error";
}

#if HMAC || HIOS

 #ifndef HIOS_MAC_VIEW
  #if HIOS
   #define HIOS_MAC_VIEW    UIView
   #define HIOS_MAC_WINDOW  UIWindow
  #else
   #define HIOS_MAC_VIEW    NSView
   #define HIOS_MAC_WINDOW  NSWindow
  #endif
 #endif

#endif

static bool checkPeerIsValid (OpenGLContext* context)
{
    HAssert (context != nullptr);

    if (context != nullptr)
    {
        if (auto* comp = context->getTargetComponent())
        {
            if (auto* peer = comp->getPeer())
            {
               #if HMAC || HIOS
                if (auto* nsView = (HIOS_MAC_VIEW*) peer->getNativeHandle())
                {
                    if (auto nsWindow = [nsView window])
                    {
                       #if HMAC
                        return ([nsWindow isVisible]
                                  && (! [nsWindow hidesOnDeactivate] || [NSApp isActive]));
                       #else
                        ignoreUnused (nsWindow);
                        return true;
                       #endif
                    }
                }
               #else
                ignoreUnused (peer);
                return true;
               #endif
            }
        }
    }

    return false;
}

static void checkGLError (const char* file, const int line)
{
    for (;;)
    {
        const GLenum e = glGetError();

        if (e == GL_NO_ERROR)
            break;

        // if the peer is not valid then ignore errors
        if (! checkPeerIsValid (OpenGLContext::getCurrentContext()))
            continue;

        DBG ("***** " << getGLErrorMessage (e) << "  at " << file << " : " << line);
        HAssertfalse;
    }
}

 #define HCHECK_OPENGL_ERROR checkGLError (__FILE__, __LINE__);
#else
 #define HCHECK_OPENGL_ERROR ;
#endif

static void clearGLError() noexcept
{
    while (glGetError() != GL_NO_ERROR) {}
}

struct OpenGLTargetSaver
{
    OpenGLTargetSaver (const OpenGLContext& c) noexcept
        : context (c), oldFramebuffer (OpenGLFrameBuffer::getCurrentFrameBufferTarget())
    {
        glGetIntegerv (GL_VIEWPORT, oldViewport);
    }

    ~OpenGLTargetSaver() noexcept
    {
        context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, oldFramebuffer);
        glViewport (oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    }

private:
    const OpenGLContext& context;
    GLuint oldFramebuffer;
    GLint oldViewport[4];

    OpenGLTargetSaver& operator= (const OpenGLTargetSaver&);
};



//==============================================================================
#include "opengl/OpenGLFrameBuffer.cpp"
#include "opengl/OpenGLGraphicsContext.cpp"
#include "opengl/OpenGLHelpers.cpp"
#include "opengl/OpenGLImage.cpp"
#include "opengl/OpenGLPixelFormat.cpp"
#include "opengl/OpenGLShaderProgram.cpp"
#include "opengl/OpenGLTexture.cpp"

//==============================================================================
#if HMAC || HIOS

 #if HCLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundeclared-selector"
 #endif

 #if HMAC
  #include "native/OpenGL_osx.h"
 #else
  #include "native/OpenGL_ios.h"
 #endif

 #if HCLANG
  #pragma clang diagnostic pop
 #endif

#elif HWINDOWS
 #include "native/OpenGL_win32.h"

#elif HLINUX
 #include "native/OpenGL_linux_X11.h"

#elif HANDROID
 #include "native/OpenGL_android.h"

#endif

#include "opengl/OpenGLContext.cpp"
#include "utils/OpenGLAppComponent.cpp"
