/* ===-- int_lib.h - configuration header for compiler-rt  -----------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses.
 *
 *==============================================================================
 *compiler_rt License
 *==============================================================================
 *
 *The compiler_rt library is dual licensed under both the University of Illinois
 *"BSD-Like" license and the MIT license.  As a user of this code you may choose
 *to use it under either license.  As a contributor, you agree to allow your code
 *to be used under both.
 *
 *Full text of the relevant licenses is included below.
 *
 *==============================================================================
 *
 *University of Illinois/NCSA
 *Open Source License
 *
 *Copyright (c) 2009-2013 by the contributors listed below
 *
 *All rights reserved.
 *
 *Developed by:
 *
 *    LLVM Team
 *
 *    University of Illinois at Urbana-Champaign
 *
 *    http://llvm.org
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of
 *this software and associated documentation files (the "Software"), to deal with
 *the Software without restriction, including without limitation the rights to
 *use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *of the Software, and to permit persons to whom the Software is furnished to do
 *so, subject to the following conditions:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimers in the
 *      documentation and/or other materials provided with the distribution.
 *
 *    * Neither the names of the LLVM Team, University of Illinois at
 *      Urbana-Champaign, nor the names of its contributors may be used to
 *      endorse or promote products derived from this Software without specific
 *      prior written permission.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
 *SOFTWARE.
 *
 *=============================================================================
 *
 *Copyright (c) 2009-2013 by the contributors listed below
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *THE SOFTWARE.
 *
 *=============================================================================
 *CONTRIBUTORS
 *
 *Below is a partial list of people who have contributed to the LLVM/CompilerRT
 *project.
 *
 *The list is sorted by surname and formatted to allow easy grepping and
 *beautification by scripts.  The fields are: name (N), email (E), web-address
 *(W), PGP key ID and fingerprint (P), description (D), and snail-mail address
 *(S).
 *
 *N: Craig van Vliet
 *E: cvanvliet@auroraux.org
 *W: http://www.auroraux.org
 *D: Code style and Readability fixes.
 *
 *N: Edward O'Callaghan
 *E: eocallaghan@auroraux.org
 *W: http://www.auroraux.org
 *D: CMake'ify Compiler-RT build system
 *D: Maintain Solaris & AuroraUX ports of Compiler-RT
 *
 *N: Howard Hinnant
 *E: hhinnant@apple.com
 *D: Architect and primary author of compiler-rt
 * ===----------------------------------------------------------------------===
 *
 * This file is a configuration header for compiler-rt.
 * This file is not part of the interface of this library.
 *
 * ===----------------------------------------------------------------------===
 */

#ifndef INT_LIB_H
#define INT_LIB_H

/* Assumption: Signed integral is 2's complement. */
/* Assumption: Right shift of signed negative is arithmetic shift. */
/* Assumption: Endianness is little or big (not mixed). */

/* ABI macro definitions */

#if __ARM_EABI__
# define ARM_EABI_FNALIAS(aeabi_name, name)         \
  void __aeabi_##aeabi_name() __attribute__((alias("__" #name)));
# define COMPILER_RT_ABI __attribute__((pcs("aapcs")))
#else
# define ARM_EABI_FNALIAS(aeabi_name, name)
# define COMPILER_RT_ABI
#endif

/* Include the standard compiler builtin headers we use functionality from. */
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

/* Include the commonly used internal type definitions. */
#include "int_types.h"

/* Include internal utility function declarations. */
#include "int_util.h"

#endif /* INT_LIB_H */
