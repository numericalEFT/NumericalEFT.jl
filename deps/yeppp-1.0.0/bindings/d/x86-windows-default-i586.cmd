@del /f /s /q "binaries/x86-windows-default-i586" >NUL 2>NUL
@mkdir "binaries/x86-windows-default-i586/" >NUL 2>NUL
@mkdir "binaries/x86-windows-default-i586/yeppp" >NUL 2>NUL

dmd -c -Isources -ofbinaries/x86-windows-default-i586/yeppp/library.obj sources/yeppp/library.d
dmd -c -Isources -ofbinaries/x86-windows-default-i586/yeppp/core.obj sources/yeppp/core.d
dmd -c -Isources -ofbinaries/x86-windows-default-i586/yeppp/math.obj sources/yeppp/math.d
dmd -c -Isources -ofbinaries/x86-windows-default-i586/yeppp/types.obj sources/yeppp/types.d

lib -c -p32 binaries/x86-windows-default-i586/yeppp-d.lib binaries/x86-windows-default-i586/yeppp/types.obj binaries/x86-windows-default-i586/yeppp/library.obj binaries/x86-windows-default-i586/yeppp/core.obj binaries/x86-windows-default-i586/yeppp/math.obj

dmd -c -Isources -ofbinaries/x86-windows-default-i586/main.obj sources/main.d
dmd -c -Isources -ofexamples/binaries/x86-windows-default-i586/entropy.obj examples/sources/entropy.d

set LIB=%CD%\binaries\x86-windows-default-i586

"c:\Program Files (x86)\D\dm\bin\implib.exe" /s %CD%\binaries\x86-windows-default-i586\yeppp.lib %CD%\..\..\library\binaries\x86-windows-default-i586\yeppp.dll

link binaries\x86-windows-default-i586\main.obj, binaries\x86-windows-default-i586\main.exe
link examples\binaries\x86-windows-default-i586\entropy.obj, examples\binaries\x86-windows-default-i586\entropy.exe
