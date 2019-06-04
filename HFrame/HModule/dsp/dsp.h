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

  ID:                 dsp
  vendor:             H
  version:            5.4.3
  name:               H DSP classes
  description:        Classes for audio buffer manipulation, digital audio processing, filtering, oversampling, fast math functions etc.
  website:            http://www.H.com/H
  license:            GPL/Commercial
  minimumCppStandard: 14

  dependencies:       audio_basics, audio_formats
  OSXFrameworks:      Accelerate
  iOSFrameworks:      Accelerate

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once

#define HDSP_H_INCLUDED

#include <audio_basics/audio_basics.h>
#include <audio_formats/audio_formats.h>

#if ! HHAS_CONSTEXPR
 #ifndef HDEMO_RUNNER
  #error "The dsp module requires a compiler that supports constexpr"
 #endif
#else

#if defined(_M_X64) || defined(__amd64__) || defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)

 #if defined(_M_X64) || defined(__amd64__)
  #ifndef __SSE2__
   #define __SSE2__
  #endif
 #endif

 #ifndef HUSE_SIMD
  #define HUSE_SIMD 1
 #endif

 #if HUSE_SIMD
  #include <immintrin.h>
 #endif

#elif defined (__ARM_NEON__) || defined (__ARM_NEON) || defined (__arm64__) || defined (__aarch64__)

 #ifndef HUSE_SIMD
  #define HUSE_SIMD 1
 #endif

 #include <arm_neon.h>

#else

 // No SIMD Support
 #ifndef HUSE_SIMD
  #define HUSE_SIMD 0
 #endif

#endif

#ifndef HVECTOR_CALLTYPE
 // __vectorcall does not work on 64-bit due to internal compiler error in
 // release mode in both VS2015 and VS2017. Re-enable when Microsoft fixes this
 #if _MSC_VER && HUSE_SIMD && ! (defined(_M_X64) || defined(__amd64__))
  #define HVECTOR_CALLTYPE __vectorcall
 #else
  #define HVECTOR_CALLTYPE
 #endif
#endif

#include <atomic>
#include <complex>
#include <cmath>
#include <array>


//==============================================================================
/** Config: HASSERTION_FIRFILTER

    When this flag is enabled, an assertion will be generated during the
    execution of DEBUG configurations if you use a FIRFilter class to process
    FIRCoefficients with a size higher than 128, to tell you that's it would be
    more efficient to use the Convolution class instead. It is enabled by
    default, but you may want to disable it if you really want to process such
    a filter in the time domain.
*/
#ifndef HASSERTION_FIRFILTER
 #define HASSERTION_FIRFILTER 1
#endif

/** Config: HDSP_USE_INTEL_MKL

    If this flag is set, then H will use Intel's MKL for H's FFT and
    convolution classes.

    The folder containing the mkl_dfti.h header must be in your header
    search paths when using this flag. You also need to add all the necessary
    intel mkl libraries to the "External Libraries to Link" field in the
    Projector.
*/
#ifndef HDSP_USE_INTEL_MKL
 #define HDSP_USE_INTEL_MKL 0
#endif

/** Config: HDSP_USE_SHARED_FFTW

    If this flag is set, then H will search for the fftw shared libraries
    at runtime and use the library for H's FFT and convolution classes.

    If the library is not found, then H's fallback FFT routines will be used.

    This is especially useful on linux as fftw often comes pre-installed on
    popular linux distros.

    You must respect the FFTW license when enabling this option.
*/
 #ifndef HDSP_USE_SHARED_FFTW
 #define HDSP_USE_SHARED_FFTW 0
#endif

/** Config: HDSP_USE_STATIC_FFTW

    If this flag is set, then H will use the statically linked fftw libraries
    for H's FFT and convolution classes.

    You must add the fftw header/library folder to the extra header/library search
    paths of your H project. You also need to add the fftw library itself
    to the extra libraries supplied to your H project during linking.

    You must respect the FFTW license when enabling this option.
*/
#ifndef HDSP_USE_STATIC_FFTW
 #define HDSP_USE_STATIC_FFTW 0
#endif

/** Config: HDSP_ENABLE_SNAP_TO_ZERO

    Enables code in the dsp module to avoid floating point denormals during the
    processing of some of the dsp module's filters.

    Enabling this will add a slight performance overhead to the DSP module's
    filters and algorithms. If your audio app already disables denormals altogether
    (for example, by using the ScopedNoDenormals class or the
    FloatVectorOperations::disableDenormalisedNumberSupport method), then you
    can safely disable this flag to shave off a few cpu cycles from the DSP module's
    filters and algorithms.
*/
#ifndef HDSP_ENABLE_SNAP_TO_ZERO
 #define HDSP_ENABLE_SNAP_TO_ZERO 1
#endif


//==============================================================================
#undef Complex  // apparently some C libraries actually define these symbols (!)
#undef Factor
#undef check

namespace H
{
    namespace dsp
    {
        template <typename Type>
        using Complex = std::complex<Type>;

        //==============================================================================
        namespace util
        {
            /** Use this function to prevent denormals on intel CPUs.
                This function will work with both primitives and simple containers.
            */
          #if HDSP_ENABLE_SNAP_TO_ZERO
            inline void snapToZero (float&       x) noexcept            { HSNAP_TO_ZERO (x); }
           #ifndef DOXYGEN
            inline void snapToZero (double&      x) noexcept            { HSNAP_TO_ZERO (x); }
            inline void snapToZero (long double& x) noexcept            { HSNAP_TO_ZERO (x); }
           #endif
          #else
            inline void snapToZero (float&       x) noexcept            { ignoreUnused (x); }
           #ifndef DOXYGEN
            inline void snapToZero (double&      x) noexcept            { ignoreUnused (x); }
            inline void snapToZero (long double& x) noexcept            { ignoreUnused (x); }
           #endif
          #endif
        }
    }
}

//==============================================================================
#if HUSE_SIMD
 #include "native/fallback_SIMDNativeOps.h"

 // include the correct native file for this build target CPU
 #if defined(__i386__) || defined(__amd64__) || defined(_M_X64) || defined(_X86_) || defined(_M_IX86)
  #ifdef __AVX2__
   #include "native/avx_SIMDNativeOps.h"
  #else
   #include "native/sse_SIMDNativeOps.h"
  #endif
 #elif defined(__arm__) || defined(_M_ARM) || defined (__arm64__) || defined (__aarch64__)
  #include "native/neon_SIMDNativeOps.h"
 #else
  #error "SIMD register support not implemented for this platform"
 #endif

 #include "containers/SIMDRegister.h"
#endif

#include "maths/SpecialFunctions.h"
#include "maths/Matrix.h"
#include "maths/Phase.h"
#include "maths/Polynomial.h"
#include "maths/FastMathApproximations.h"
#include "maths/LookupTable.h"
#include "maths/LogRampedValue.h"
#include "containers/AudioBlock.h"
#include "processors/ProcessContext.h"
#include "processors/ProcessorWrapper.h"
#include "processors/ProcessorChain.h"
#include "processors/ProcessorDuplicator.h"
#include "processors/Bias.h"
#include "processors/Gain.h"
#include "processors/WaveShaper.h"
#include "processors/IIRFilter.h"
#include "processors/FIRFilter.h"
#include "processors/Oscillator.h"
#include "processors/LadderFilter.h"
#include "processors/StateVariableFilter.h"
#include "processors/Oversampling.h"
#include "processors/Reverb.h"
#include "frequency/FFT.h"
#include "frequency/Convolution.h"
#include "frequency/Windowing.h"
#include "filter_design/FilterDesign.h"

#endif
