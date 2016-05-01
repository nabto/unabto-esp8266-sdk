@echo off
rem This script copies the relevant files from the uNabto submodule.

xcopy /q /s /y unabto\src\unabto src\unabto\ >NUL
xcopy /q /s /y unabto\src\modules\crypto\generic src\modules\crypto\generic\ >NUL
xcopy /q /y unabto\src\platforms\unabto_common_types.h src\platforms\ >NUL
xcopy /q /y unabto\src\unabto_version.h src\ >NUL

echo Relevant files from uNabto submodule copied to src folder.