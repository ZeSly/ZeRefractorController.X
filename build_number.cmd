echo off
setlocal enabledelayedexpansion

set version_major=1
set version_minor=1
set buildnum=

for /f "tokens=3" %%i in (build_number.h) do set buildnum=%%i

if not exist release (
    mkdir release
) else (
    del /Q release \*.*
)

copy dist\default\production\ZeRefractorController.X.production.hex release\
move release\ZeRefractorController.X.production.hex release\ZeRefractorController_%version_major%_%version_minor%_%buildnum%.hex

set /a buildnum=%buildnum% + 1

@echo #define VERSION_MAJOR %version_major% > build_number.h
@echo #define VERSION_MINOR %version_minor% >> build_number.h
@echo #define BUILD_NUMBER %buildnum% >> build_number.h