#!/bin/bash

if [ "$1" != "Debug" ] && [ "$1" != "Release" ] ; then
	echo "You must specify either 'Debug' or 'Release'"
	exit 1
fi

MACDEPLOYQT=/Users/Ben/Qt/5.6/clang_64/bin/macdeployqt
PATH_TO_APP=build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-$1/src/app/asIDE.app

$MACDEPLOYQT $PATH_TO_APP

dylibbundler -od -b -x $PATH_TO_APP/Contents/MacOS/asIDE -d $PATH_TO_APP/Contents/libs/ -p @executable_path/../libs/ <<< "build/build-asIDE-Desktop_Qt_5_6_0_clang_64bit-$1/src/intellisense"

cp $PATH_TO_APP/../../intellisense/libIntellisense.1.dylib $PATH_TO_APP/Contents/Frameworks/libIntellisense.1.dylib

otool -L $PATH_TO_APP/Contents/MacOS/asIDE

cp images/app-icon/app.icns $PATH_TO_APP/Contents/Resources

defaults write $PATH_TO_APP/Contents/Info.plist CFBundleIconFile -string "app"

mkdir tmp
cp -r $PATH_TO_APP tmp

if [ "$2" = "-dmg" ] ; then
	$MACDEPLOYQT tmp/asIDE.app -dmg
	mv tmp/asIDE.dmg asIDE.dmg
fi

rm -r tmp/*
rmdir tmp