@echo off
rem This script packs all relevant files into a zip library.

set NAME=unabto-esp8266-sdk

xcopy /q /s /y src %NAME%\src\ >NUL
xcopy /q /s /y examples %NAME%\examples\ >NUL
xcopy /q /y keywords.txt %NAME%\ >NUL
xcopy /q /y library.properties %NAME%\ >NUL

echo Please create a zip file of the folder %NAME% and then press any key to continue.
pause >NUL

rmdir /s /q %NAME%

echo Library %NAME%.zip created!