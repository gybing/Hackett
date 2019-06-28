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

#ifdef HDSP_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#include "dsp.h"

#if ! HHAS_CONSTEXPR
 #ifndef HDEMO_RUNNER
  #error "The dsp module requires a compiler that supports constexpr"
 #endif
#else

#ifndef HUSE_VDSP_FRAMEWORK
 #define HUSE_VDSP_FRAMEWORK 1
#endif

#if (HMAC || HIOS) && HUSE_VDSP_FRAMEWORK
 #include <Accelerate/Accelerate.h>
#else
 #undef HUSE_VDSP_FRAMEWORK
#endif

#if HDSP_USE_INTEL_MKL
 #include <mkl_dfti.h>
#endif

#include "processors/FIRFilter.cpp"
#include "processors/IIRFilter.cpp"
#include "processors/LadderFilter.cpp"
#include "processors/Oversampling.cpp"
#include "maths/SpecialFunctions.cpp"
#include "maths/Matrix.cpp"
#include "maths/LookupTable.cpp"
#include "frequency/FFT.cpp"
#include "frequency/Convolution.cpp"
#include "frequency/Windowing.cpp"
#include "filter_design/FilterDesign.cpp"

#if HUSE_SIMD
 #if defined(__i386__) || defined(__amd64__) || defined(_M_X64) || defined(_X86_) || defined(_M_IX86)
  #ifdef __AVX2__
   #include "native/avx_SIMDNativeOps.cpp"
  #else
   #include "native/sse_SIMDNativeOps.cpp"
  #endif
 #elif defined(__arm__) || defined(_M_ARM) || defined (__arm64__) || defined (__aarch64__)
  #include "native/neon_SIMDNativeOps.cpp"
 #else
  #error "SIMD register support not implemented for this platform"
 #endif
#endif

#if HUNIT_TESTS
 #include "maths/Matrix_test.cpp"
 #include "maths/LogRampedValue_test.cpp"

 #if HUSE_SIMD
  #include "containers/SIMDRegister_test.cpp"
 #endif

 #include "frequency/FFT_test.cpp"
 #include "processors/FIRFilter_test.cpp"
#endif

#endif
