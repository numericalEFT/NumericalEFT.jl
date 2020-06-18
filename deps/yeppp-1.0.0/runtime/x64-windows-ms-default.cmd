set CXX=cl
set CXXFLAGS=/nologo /Zl /O2 /GR- /EHs- /I../library/headers/ /c
set AS=nasm
set ASFLAGS=-f win64
set RM=del
set RMFLAGS=/F /Q /S
set LIB=lib
set LIBFLAGS=/NOLOGO /NODEFAULTLIB /MACHINE:X64

%RM% %RMFLAGS% "binaries/x64-windows-ms-default" >NUL 2>NUL
mkdir "binaries/x64-windows-ms-default/" >NUL 2>NUL

%CXX% %CXXFLAGS% /Fobinaries/x64-windows-ms-default/ms.o sources/x86/ms.cpp

%LIB% %LIBFLAGS% /OUT:binaries/x64-windows-ms-default/yeprt.lib binaries/x64-windows-ms-default/*.o
