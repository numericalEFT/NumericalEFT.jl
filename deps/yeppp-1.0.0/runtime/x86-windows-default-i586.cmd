set CXX=cl
set CXXFLAGS=/nologo /Zl /O2 /GR- /EHs- /I../library/headers/ /c
set AS=nasm
set ASFLAGS=-f win32
set RM=del
set RMFLAGS=/F /Q /S
set LIB=lib
set LIBFLAGS=/NOLOGO /NODEFAULTLIB /MACHINE:X86

%RM% %RMFLAGS% "binaries/x86-windows-default-i586" >NUL 2>NUL
mkdir "binaries/x86-windows-default-i586/" >NUL 2>NUL

%CXX% %CXXFLAGS% /Fobinaries/x86-windows-default-i586/ms.o sources/x86/ms.cpp

%LIB% %LIBFLAGS% /OUT:binaries/x86-windows-default-i586/yeprt.lib binaries/x86-windows-default-i586/*.o
