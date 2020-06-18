@ECHO off
ECHO ^<?xml version=^"1.0^" encoding=^"utf-8^"?^> > %2
FOR /F "tokens=* delims=" %%A IN (%1) DO ECHO %%A >> %2
