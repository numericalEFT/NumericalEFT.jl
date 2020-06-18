//===-- lib/fp_lib.h - Floating-point utilities -------------------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses.
//
//==============================================================================
//compiler_rt License
//==============================================================================
//
//The compiler_rt library is dual licensed under both the University of Illinois
//"BSD-Like" license and the MIT license.  As a user of this code you may choose
//to use it under either license.  As a contributor, you agree to allow your code
//to be used under both.
//
//Full text of the relevant licenses is included below.
//
//==============================================================================
//
//University of Illinois/NCSA
//Open Source License
//
//Copyright (c) 2009-2013 by the contributors listed below
//
//All rights reserved.
//
//Developed by:
//
//    LLVM Team
//
//    University of Illinois at Urbana-Champaign
//
//    http://llvm.org
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal with
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimers.
//
//    * Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimers in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the names of the LLVM Team, University of Illinois at
//      Urbana-Champaign, nor the names of its contributors may be used to
//      endorse or promote products derived from this Software without specific
//      prior written permission.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
//CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
//SOFTWARE.
//
//=============================================================================
//
//Copyright (c) 2009-2013 by the contributors listed below
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//
//=============================================================================
//CONTRIBUTORS
//
//Below is a partial list of people who have contributed to the LLVM/CompilerRT
//project.
//
//The list is sorted by surname and formatted to allow easy grepping and
//beautification by scripts.  The fields are: name (N), email (E), web-address
//(W), PGP key ID and fingerprint (P), description (D), and snail-mail address
//(S).
//
//N: Craig van Vliet
//E: cvanvliet@auroraux.org
//W: http://www.auroraux.org
//D: Code style and Readability fixes.
//
//N: Edward O'Callaghan
//E: eocallaghan@auroraux.org
//W: http://www.auroraux.org
//D: CMake'ify Compiler-RT build system
//D: Maintain Solaris & AuroraUX ports of Compiler-RT
//
//N: Howard Hinnant
//E: hhinnant@apple.com
//D: Architect and primary author of compiler-rt
//===----------------------------------------------------------------------===//
//
// This file is a configuration header for soft-float routines in compiler-rt.
// This file does not provide any part of the compiler-rt interface, but defines
// many useful constants and utility routines that are used in the
// implementation of the soft-float routines in compiler-rt.
//
// Assumes that float and double correspond to the IEEE-754 binary32 and
// binary64 types, respectively, and that integer endianness matches floating
// point endianness on the target platform.
//
//===----------------------------------------------------------------------===//

#ifndef FP_LIB_HEADER
#define FP_LIB_HEADER

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "int_lib.h"

#if defined SINGLE_PRECISION

typedef uint32_t rep_t;
typedef int32_t srep_t;
typedef float fp_t;
#define REP_C UINT32_C
#define significandBits 23

static inline int rep_clz(rep_t a) {
    return __builtin_clz(a);
}

// 32x32 --> 64 bit multiply
static inline void wideMultiply(rep_t a, rep_t b, rep_t *hi, rep_t *lo) {
    const uint64_t product = (uint64_t)a*b;
    *hi = product >> 32;
    *lo = product;
}

#elif defined DOUBLE_PRECISION

typedef uint64_t rep_t;
typedef int64_t srep_t;
typedef double fp_t;
#define REP_C UINT64_C
#define significandBits 52

static inline int rep_clz(rep_t a) {
#if defined __LP64__
    return __builtin_clzl(a);
#else
    if (a & REP_C(0xffffffff00000000))
        return __builtin_clz(a >> 32);
    else 
        return 32 + __builtin_clz(a & REP_C(0xffffffff));
#endif
}

#define loWord(a) (a & 0xffffffffU)
#define hiWord(a) (a >> 32)

// 64x64 -> 128 wide multiply for platforms that don't have such an operation;
// many 64-bit platforms have this operation, but they tend to have hardware
// floating-point, so we don't bother with a special case for them here.
static inline void wideMultiply(rep_t a, rep_t b, rep_t *hi, rep_t *lo) {
    // Each of the component 32x32 -> 64 products
    const uint64_t plolo = loWord(a) * loWord(b);
    const uint64_t plohi = loWord(a) * hiWord(b);
    const uint64_t philo = hiWord(a) * loWord(b);
    const uint64_t phihi = hiWord(a) * hiWord(b);
    // Sum terms that contribute to lo in a way that allows us to get the carry
    const uint64_t r0 = loWord(plolo);
    const uint64_t r1 = hiWord(plolo) + loWord(plohi) + loWord(philo);
    *lo = r0 + (r1 << 32);
    // Sum terms contributing to hi with the carry from lo
    *hi = hiWord(plohi) + hiWord(philo) + hiWord(r1) + phihi;
}

#else
#error Either SINGLE_PRECISION or DOUBLE_PRECISION must be defined.
#endif

#define typeWidth       (sizeof(rep_t)*CHAR_BIT)
#define exponentBits    (typeWidth - significandBits - 1)
#define maxExponent     ((1 << exponentBits) - 1)
#define exponentBias    (maxExponent >> 1)

#define implicitBit     (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (REP_C(1) << (significandBits + exponentBits))
#define absMask         (signBit - 1U)
#define exponentMask    (absMask ^ significandMask)
#define oneRep          ((rep_t)exponentBias << significandBits)
#define infRep          exponentMask
#define quietBit        (implicitBit >> 1)
#define qnanRep         (exponentMask | quietBit)

static inline rep_t toRep(fp_t x) {
    const union { fp_t f; rep_t i; } rep = {.f = x};
    return rep.i;
}

static inline fp_t fromRep(rep_t x) {
    const union { fp_t f; rep_t i; } rep = {.i = x};
    return rep.f;
}

static inline int normalize(rep_t *significand) {
    const int shift = rep_clz(*significand) - rep_clz(implicitBit);
    *significand <<= shift;
    return 1 - shift;
}

static inline void wideLeftShift(rep_t *hi, rep_t *lo, int count) {
    *hi = *hi << count | *lo >> (typeWidth - count);
    *lo = *lo << count;
}

static inline void wideRightShiftWithSticky(rep_t *hi, rep_t *lo, unsigned int count) {
    if (count < typeWidth) {
        const bool sticky = *lo << (typeWidth - count);
        *lo = *hi << (typeWidth - count) | *lo >> count | sticky;
        *hi = *hi >> count;
    }
    else if (count < 2*typeWidth) {
        const bool sticky = *hi << (2*typeWidth - count) | *lo;
        *lo = *hi >> (count - typeWidth) | sticky;
        *hi = 0;
    } else {
        const bool sticky = *hi | *lo;
        *lo = sticky;
        *hi = 0;
    }
}

#endif // FP_LIB_HEADER
