@ECHO off
REM                      Yeppp! library implementation
REM
REM This file is part of Yeppp! library and licensed under the New BSD license.
REM See LICENSE.txt for the full text of the license.

IF NOT "%2" == "" GOTO error

IF "%1" == ""    GOTO detect
IF "%1" == "x86" GOTO x86
IF "%1" == "x64" GOTO x64
IF "%1" == "/?"  GOTO help
GOTO error

:detect
IF /i %PROCESSOR_ARCHITECTURE% == AMD64 GOTO x64
IF /i %PROCESSOR_ARCHITEW6432% == AMD64 GOTO x64
IF /i %PROCESSOR_ARCHITECTURE% == x86 GOTO x86
ECHO Error: unknown system architecture %processor_architecture%
GOTO help

:universal
SET "PATH=%YEPBINARIES%;%PATH%"
IF EXIST %YEPROOT%binaries\java-1.5\yeppp.jar (
	IF "%CLASSPATH%" == "" (
		SET "CLASSPATH=%YEPROOT%binaries\java-1.5\yeppp.jar"
	) ELSE (
		SET "CLASSPATH=%YEPROOT%binaries\java-1.5\yeppp.jar;%CLASSPATH%"
	)
)
IF "%INCLUDE%" == "" (
	SET "INCLUDE=%YEPROOT%library\headers"
) ELSE (
	SET "INCLUDE=%YEPROOT%library\headers;%INCLUDE%"
)
IF "%LIB%" == "" (
	SET "LIB=%YEPROOT%library\headers"
) ELSE (
	SET "LIB=%YEPROOT%library\headers;%LIB%"
)
IF EXIST "%YEPROOT%binaries\android\yeppp\Android.mk" (
	IF "%NDK_MODULE_PATH%" == "" (
		SET "NDK_MODULE_PATH=%YEPROOT:\=/%binaries/android"
	) ELSE (
		SET "NDK_MODULE_PATH=%YEPROOT:\=/%binaries/android:%NDK_MODULE_PATH%"
	)
)
EXIT /B 0

:x86
SET "YEPROOT=%~dp0"
SET "YEPBINARIES=%YEPROOT%binaries\windows\x86"
SET YEPPLATFORM=x86-windows-default-i586
GOTO universal

:x64
SET "YEPROOT=%~dp0"
SET "YEPBINARIES=%YEPROOT%binaries\windows\amd64"
SET YEPPLATFORM=x64-windows-ms-default
GOTO universal

:error
ECHO Error: invalid command-line argument(s)
GOTO help

:help
ECHO Usage: SetVars.cmd [param]
ECHO Possible options for [param]
ECHO     x86 - set variables for 32-bit (x86) environment
ECHO     x64 - set variables for 64-bit (x86-64 aka x64) environment
ECHO.    /?  - show this help message
ECHO If neither option is specified, the variables are set based on OS architecture.
IF "%1" == "/?" (
	EXIT /B 0
) ELSE (
	EXIT /B 1
)
