#!/bin/bash
/Users/Ben/Qt/5.6/clang_64/bin/macdeployqt build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app

dylibbundler -od -b -x build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app/Contents/MacOS/asIDE -d build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app/Contents/libs/ -p @executable_path/../libs/ <<< "build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/intellisense"

otool -L build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app/Contents/MacOS/asIDE

cp images/app-icon/app.icns build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app/Contents/Resources

defaults write /Users/Ben/Documents/QtProjects/asIDE/build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app/Contents/Info.plist CFBundleIconFile -string "app"

mkdir tmp
cp -r build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-Release/src/app/asIDE.app tmp

/Users/Ben/Qt/5.6/clang_64/bin/macdeployqt tmp/asIDE.app -dmg
mv tmp/asIDE.dmg asIDE.dmg

rm -r tmp/*
rmdir tmp