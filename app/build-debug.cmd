@echo off
call ndk-build.cmd NDK_DEBUG=1 %*
if %errorlevel% neq 0 exit /b %errorlevel%
call ..\gradlew.bat assembleDebug
if %errorlevel% neq 0 exit /b %errorlevel%
adb install -r build/outputs/apk/app-debug.apk
