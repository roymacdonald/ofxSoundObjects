//
//  ofxSoundUtils.h
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#pragma once
#include "ofSoundBuffer.h"
#include "ofSoundBaseTypes.h" 
namespace ofxSoundUtils{
    void getBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, std::vector<int> group);
    void setBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, const std::vector<int>& group);
    bool checkBuffers(const ofSoundBuffer& src, ofSoundBuffer& dst, bool bSetDst = true);
	
	bool getBufferPeaks(ofSoundBuffer& buffer, std::vector<float>& currentPeaks, std::vector<float>& prevPeaks); // returns true if a new peak was found
	
	//--------------------------------------------------------------
	ofSoundDevice getSoundDeviceInfo(int id);
	std::string getSoundDeviceString(ofSoundDevice soundDevice, bool bInputs, bool bOutputs);
	std::vector<ofSoundDevice>getInputSoundDevices();
	std::vector<ofSoundDevice>getOutputSoundDevices();
	void printInputSoundDevices();
	void printOutputSoundDevices();
	
	template<class T>
	static inline void resize_vec(std::vector<T>& vec, const size_t& newSize){
		if(vec.size() != newSize) vec.resize(newSize);
	}
	
};
//--------------------------------------------------------------

class ofxCircularSoundBuffer: public ofSoundBuffer{
public:
	
	void push(ofSoundBuffer& buffer){
		if(getBuffer().size() > 0){
			pushIndex %= getBuffer().size();
			
			memcpy(&getBuffer()[pushIndex], &buffer.getBuffer()[0], sizeof(float) * buffer.getBuffer().size());
			
			pushIndex += buffer.getBuffer().size();
		}
	}
	size_t getPushIndex() const {return pushIndex; }
	
private:
	
	size_t pushIndex = 0;
};
