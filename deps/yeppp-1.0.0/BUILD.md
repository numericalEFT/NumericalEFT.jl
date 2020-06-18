How to build Yeppp!
=====================

Do you need to build Yeppp?
------------------------------

**For 99% of cases, no!** Yeppp! releases include pre-built binaries for all supported platforms, and for most users we recommend using these pre-built binaries. Unlike most other Unix libraries, Yeppp! maintains binary compatibility with nearly all Linux distributions, so there is no need to rebuild it yourself. Moreover, **BUILDING YEPPP! WITH OUTDATED TOOLCHAIN CAN PRODUCE BINARIES WHICH CRASH ON SOME SYSTEMS, BUT NOT OTHERS** (and by Murphy's law it will crash on all systems but the ones you tested on). This guide is provided for those who want to participate in development of Yeppp! or otherwise modify the library, all others will be better off using pre-built binaries.

Prerequisites
----------

You will need the following tools to build Yeppp!

*   Python 2.x to regenerate auto-generated parts of Yeppp! If you use sources from Yeppp! release, you can skip this requirement as Yeppp! releases already include all auto-generated files. 
*   Java Runtime Environment version 1.6 or later. Yeppp! build system is written in Java, so there is no way around it.
*   Java Development Kit version 1.6 or later. You can use either Open JDK or Oracle JDK. JDK is required even if you don't plan to develop with Yeppp! in Java.
*   Intellij IDEA to build the build system. If you use source from Yeppp! release, you can skip this requirement as Yeppp! releases already include pre-built build system.
*   Microsoft Visual Studio 2008, 2010, or 2012 to build Windows binaries. Note that some parts (runtime) of the library can not be built with Visual Studio 2012. We use Visual Studio 2012 to compile most part of Yeppp!, and Visual Studio 2010 where required.
*   Intel C/C++ Compilers to build Linux/Xeon Phi binaries. We use 13.1.3 version.
*   GNU C/C++ Compilers to build Linux binaries (except Xeon Phi). We use 4.8 version.
*   Clang C/C++ Compilers to build Mac OS X binaries. We use trunk 3.4 version.
*   Android NDK to build Android binaries. Use the latest Android NDK. Also specify the root of Android NDK directory in *ANDROID_NDK_ROOT* environment variable.
*   NASM to build any x86/x86-64 binaries (except Xeon Phi). **Use the trunk version of NASM.** Yeppp! widely uses new CPU instructions, and old (pre-trunk) versions of NASM may have bugs in handling these instructions. **NASM from your Linux repo almost certainly has such bugs.**
*   GNU Binutils to build Linux binaries.
*   Apple utilities (strip and dsymutil) to build Mac OS X binaries.
*   Apache Ant to build Java bindings.
*   MSBuild (part of Visual Studio) and [InjectModuleInitializer.exe](http://einaregilsson.com/module-initializers-in-csharp/) to build CLR bindings.
*   Doxygen to build documentation.
*   WiX Toolkit to build MSI installer for Windows.
   
Generating auto-generated parts
-------------------------------

**This step is needed only if you use sources from a Yeppp! repository.** Yeppp! release distributions already include auto-generated files.

Check out [Peach-Py](https://bitbucket.org/MDukhan/peachpy) and add its directory to environment variable PYTHONPATH.

Change directory to Yeppp! root and run `python codegen/core.py` to generate yepCore module. This will generate the files:

*    library/headers/yepCore.h
*    library/sources/core/
*    bindings/java/sources-jni/core/
*    bindings/java/sources-java/info/yeppp/Core.java
*    bindings/clr/sources-csharp/core/
*    bindings/fortran/sources/yepCore.f90
*    unit-tests/sources/core/

Similarly generate yepMath module by executing `python codegen/math.py`.

Building the build system
-------------------------

**This step is needed only if you use sources from a Yeppp! repository.** Yeppp! release distributions already include pre-built build system (CLIBuild.jar).

Check out [EBUILDA](https://bitbucket.org/MDukhan/ebuilda), open its project file in Intellij IDEA, and build the EBUILDA.jar artifact.

Open the Yeppp! build system project in the *build* directory in Yeppp! tree in Intellij IDEA. Specify where to find EBUILDA.jar dependency, and build the CLIBuild.jar artifact. The CLIBuild.jar will appear in the root directory of Yeppp!

Building the runtime library
----------------------------

In order to maintain high degree of binary compatibility across target platforms, Yeppp! uses its own runtime library (in part based on compiler-rt from Clang project). Navigate to *runtime* directory in Yeppp! tree.

To build runtime library for Windows, use Visual Studio Command Prompt and run `x86-windows-default-i586.cmd` (to build runtime library for x86/32-bit architecture; requires Visual Studio x86 Command Prompt) or `x64-windows-ms-default.cmd` (to build runtime library for x86-64/64-bit architecture; requires Visual Studio x64 Command Prompt). Note: the runtime library can not be build with Visual Studio 2012.

To build runtime library for Linux or Android, use the provided Makefile. On a system with GNU Make run
```
make <platform>
```
where platform can be:

*    **x86-linux-pic-i586** to build runtime library for Linux/x86
*    **x64-linux-sysv-default** to build runtime library for Linux/x86-64
*    **x64-linux-k1om-default** to build runtime library for Linux/k1om (Xeon Phi)
*    **arm-linux-softeabi-v5t** to build runtime library for Linux/armel
*    **arm-linux-hardeabi-v7a** to build runtime library for Linux/armhf
*    **x86-linux-pic-android** to build runtime library for Android (x86 ABI)
*    **arm-linux-softeabi-android** to build runtime library for Android (ARMEABI ABI)
*    **arm-linux-softeabi-androidv7a** to build runtime library for Android (ARMEABI-V7A ABI)
*    **mips-linux-o32-android** to build runtime library for Android (MIPS ABI)

On Mac OS X Yeppp! uses the default runtime library, so nothing needs to be built.

Building Yeppp!
---------------

To build Yeppp! navigate to the root of Yeppp! tree and execute
```
java -jar CLIBuild.jar <platform>
```
where platform is

*    **x86-windows-default-i586** to build Yeppp! for Windows/x86
*    **x64-windows-ms-default** to build runtime library for Windows/x86-64
*    **x86-macosx-pic-default** to build Yeppp! for Mac OS X/x86
*    **x64-macosx-sysv-default** to build runtime library for Mac OS X/x86-64
*    **x86-linux-pic-i586** to build runtime library for Linux/x86
*    **x64-linux-sysv-default** to build runtime library for Linux/x86-64
*    **x64-linux-k1om-default** to build runtime library for Linux/k1om (Xeon Phi)
*    **arm-linux-softeabi-v5t** to build runtime library for Linux/armel
*    **arm-linux-hardeabi-v7a** to build runtime library for Linux/armhf
*    **x86-linux-pic-android** to build runtime library for Android (x86 ABI)
*    **arm-linux-softeabi-android** to build runtime library for Android (ARMEABI ABI)
*    **arm-linux-softeabi-androidv7a** to build runtime library for Android (ARMEABI-V7A ABI)
*    **mips-linux-o32-android** to build runtime library for Android (MIPS ABI)

The build system will put the compiled binaries into *binaries* directory in Yeppp! tree.

Buliding Java Bindings
----------------------

Java bindings consist of two parts: glue functions in C which implement the JNI interface and Java classes which describe the functionality implemented by native library. The C functions are compiled as a part of Yeppp! and linked into the library binary. The Java classes need to be build separately.

Open a terminal or Command Prompt, navigate to *bindings/java* directory in Yeppp! and run
```
ant package
```

Ant will put the compiled JARs into *binaries/java-1.5* directory in Yeppp! tree.

Building CLR Bindings
---------------------

To build CLR bindings open the Visual Studio Command Prompt, navigate to directory *bindings/clr* in Yeppp! tree and run
```
msbuild /t:Package
```

MSBuild will put the compiled platform-independent managed DLL into *binaries/clr-2.0* directory in Yeppp! tree.

Building the documentation
--------------------------

Yeppp! uses Doxygen for auto-documenting the library from comments in the code.
You will find Doxyfile files at the following paths:

*    *library/sources/Doxyfile* for C/C++ documentation
*    *bindings/java/Doxyfile* for Java documentation
*    *bindings/clr/Doxyfile* for C# documentation
*    *bindings/fortran/Doxyfile* for FORTRAN documentation

To build the documentation navigate to the directories with about Doxyfile files and run `doxygen`.

The documentation will be generated in the *docs* directory in Yeppp! tree.

Building the MSI installer
--------------------------

To make an MSI installer for Yeppp! SDK, open Visual Studio Command Prompt, navigate to root Yeppp! directory and execute `SetVars.bat`. Then switch to directory *installer/windows* in Yeppp! tree, and run
```
nmake /A
```.

The generated MSI installer will be placed in *installer/windows* directory in the Yeppp! tree.
