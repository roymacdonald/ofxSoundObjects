#!/bin/bash
set -e


cd ..
of_root=${PWD}/openFrameworks
snd_obj_root=${PWD}/openFrameworks/addons/ofxSoundObjects

git clone --depth=1 https://github.com/openframeworks/openFrameworks
#cp not move so github actions can do cleanup without error
cp -r ofxSoundObjects openFrameworks/addons/
cd openFrameworks
git clone --depth=1 --branch=master https://github.com/npisanti/ofxAudioFile addons/ofxAudioFile
git clone --depth=1 --branch=master https://github.com/kylemcdonald/ofxFft addons/ofxFft

cd ${of_root}
scripts/osx/download_libs.sh

# Compile commandline tool
cd ${snd_obj_root}
echo "Building ofxSoundObjects example - OSX"
example=${snd_obj_root}/example-soundMixer

if [ ! -e "$example"/$(basename $example).xcodeproj ]; then
      echo "-----------------------------------------------------------------"
      echo "no xcode project for ${example}"
      exit 1
fi

echo "-----------------------------------------------------------------"
echo "building $example Debug"
xcodebuild -configuration Debug -target $(basename $example) CODE_SIGN_IDENTITY="" UseModernBuildSystem=NO -project $example/$(basename $example).xcodeproj
ret=$?
if [ $ret -ne 0 ]; then
        echo failed building $example Debug
        exit
fi
