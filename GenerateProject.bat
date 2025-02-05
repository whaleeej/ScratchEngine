@ECHO OFF

SET CMAKE=cmake
SET CMAKE_DESTINATION="Build"
SET CMAKE_GENERATOR="Unknown Generator"

reg query "HKEY_CLASSES_ROOT\VisualStudio.DTE.17.0" >> nul 2>&1
if %ERRORLEVEL% EQU 0 (
	SET CMAKE_GENERATOR="Visual Studio 17 2022"
	goto STARTBUILDSTAGE
)
reg query "HKEY_CLASSES_ROOT\VisualStudio.DTE.16.0" >> nul 2>&1
if %ERRORLEVEL% EQU 0 (
	SET CMAKE_GENERATOR="Visual Studio 16 2019"
	goto STARTBUILDSTAGE
)

:STARTBUILDSTAGE

PUSHD %~dp0
MKDIR %CMAKE_DESTINATION% 2>NUL
PUSHD %CMAKE_DESTINATION%
%CMAKE% -A x64 -G %CMAKE_GENERATOR% "%~dp0"

pause