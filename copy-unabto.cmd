@echo off
rem This script copies the relevant files from the uNabto submodule.

xcopy /q /s /y unabto\src\unabto src\unabto\ >NUL
xcopy /q /s /y unabto\src\modules\crypto\generic src\modules\crypto\ >NUL
xcopy /q /y unabto\src\modules\fingerprint_acl\fp_acl.* src\modules\fingerprint_acl\ >NUL
xcopy /q /y unabto\src\modules\fingerprint_acl\fp_acl_ae.* src\modules\fingerprint_acl\ >NUL
xcopy /q /y unabto\src\modules\fingerprint_acl\fp_acl_memory.* src\modules\fingerprint_acl\ >NUL
xcopy /q /y unabto\src\modules\util\read_hex.* src\modules\util\ >NUL
xcopy /q /y unabto\src\platforms\unabto_common_types.h src\platforms\ >NUL
xcopy /q /y unabto\src\unabto_version.h src\ >NUL

echo Relevant files from uNabto submodule copied to src folder.
