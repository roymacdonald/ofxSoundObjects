//
//  ofxSoundObjectsUtils.h
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#pragma once
#include "ofxSoundObjects.h"
namespace ofxSoundObjects{
    void getBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, std::vector<int> group);
    void setBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, const std::vector<int>& group);
    bool checkBuffers(const ofSoundBuffer& src, ofSoundBuffer& dst, bool bSetDst = true);
	
	void getBufferPeaks(ofSoundBuffer& buffer, std::vector<float>& peaks);
	
	std::string getSoundDeviceString(ofSoundDevice soundDevice, bool bInputs, bool bOutputs);
	std::vector<ofSoundDevice>getInputSoundDevices();
	std::vector<ofSoundDevice>getOutputSoundDevices();
	void printInputSoundDevices();
	void printOutputSoundDevices();
};
