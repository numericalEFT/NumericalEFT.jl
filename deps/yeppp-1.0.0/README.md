Yeppp! F.A.Q
==============

What is Yeppp!?
--------------

Yeppp! is a high-performance SIMD-optimized mathematical library for x86, ARM, and MIPS processors on Windows, Android, Mac OS X, and GNU/Linux systems. Yeppp! contains versions of its functions for multiple architectures and instruction sets and chooses the optimal implementation during initialization to guarantee the best performance on the host machine.

Which vector operations are supported?
-------------------------------------

Yeppp! aims to provide an orthogonal set of vector operations. By *orthogonal* we mean that Yeppp! supports all possible variations of operands. For example, for subtraction Yeppp! has 7 variants:

*   Subtraction of two arrays (z[i] := x[i] - y[i])
*   Subtraction of constant from array (z[i] := x[i] - c)
*   Subtraction of array from constant (z[i] := c - x[i])
*   In-place subtraction of two arrays, overwrite first array (x[i] := x[i] - y[i])
*   In-place subtraction of two arrays, overwrite second array (y[i] := x[i] - y[i])
*   In-place subtraction of constant from array (x[i] := x[i] - c)
*   In-place subtraction of array from constant (x[i] := c - x[i])

Moreover, there is a complete set of the above functions for different data types:

*   8-bit, 16-bit, 32-bit, and 64-bit integers (signed and unsigned)
*   Single precision (32-bit) and double precision (64-bit) floating-point numbers

Besides basic arithmetic operations (addition, subtraction, multiplication) Yeppp! provides high-performance mathematical functions, such as *log*, *exp*, *sin*, which operate on vectors.

What other functionality is supported?
------------------------------------

Yeppp! provides a number of functions to retrieve information about the processor (supported instruction extensions, architecture and microarchitecture, canonical processor name), measure processor cycles, access high-performance system timer, and processor energy counters.

Do I have to write in C++ to use Yeppp?
---------------------------------------

No. Yeppp! functions can be called from C, C++, C#, Java, FORTRAN.

Which architectures and operating systems are supported?
-----------------------------

As of Yeppp! 1.0, the library supports 13 platforms:

*   Windows on x86 (32-bit) architecture
*   Windows on x86-64 (64-bit) architecture
*   Mac OS X on x86 (Intel 32 bit) architecture
*   Mac OS X on x86-64 (Intel 64 bit) architecture
*   Linux on x86 architecture
*   Linux on x86-64 architecture
*   Linux on armel ABI (ARMv5T architecture, Soft-Float EABI)
*   Linux on armhf ABI (ARMv7-A architecture, Hard-Float EABI)
*   Linux on k1om architecture (Xeon Phi)
*   Android "armeabi" ABI (ARMv5TE architecture)
*   Android "armeabi-v7a" ABI (ARMv7-A architecture)
*   Android "x86" ABI
*   Android "mips" ABI

What is the license?
-------------------

Yeppp! is licensed under the New BSD license (AKA 3-clause BSD), an OSI-approved permissive open source license. In particular, the license allows you to

*   Use Yeppp! in closed-source projects
*   Distribute Yeppp! as a part of your commercial product
*   Modify Yeppp! source code
