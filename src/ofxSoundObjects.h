#pragma once

#include "ofMain.h"
#include "ofxSoundObject.h"
#include "ofxSoundMixer.h"
#include "ofxSoundFile.h"
#include "ofxSoundFile.h"

void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, ofBaseApp * appPtr = NULL);
void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, int sampleRate, int bufferSize, int nBuffers);
void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, ofBaseApp * appPtr, int sampleRate, int bufferSize, int nBuffers);

