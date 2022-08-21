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
#include "ofMath.h"

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
	
	void fadeBuffer(ofSoundBuffer& buffer, bool bFadeOut);

};
//--------------------------------------------------------------

class ofxCircularSoundBuffer: public ofSoundBuffer{
public:
	
	void push(ofSoundBuffer& buffer){
        if(size() == 0 || getNumChannels() == 0 || getNumFrames() != buffer.getNumFrames() * numBuffers){
            allocate(buffer.getNumFrames() * numBuffers, buffer.getNumChannels());
            setSampleRate(buffer.getSampleRate());
        }
		if(getBuffer().size() > 0){
            pushIndex += lastPushSize;
            
            lastPushSize = buffer.getBuffer().size();
            if(pushIndex + lastPushSize <= getBuffer().size()){
                memcpy(&getBuffer()[pushIndex], &buffer.getBuffer()[0], sizeof(float) * lastPushSize);
            }else{
                size_t n = getBuffer().size() - pushIndex;
                memcpy(&getBuffer()[pushIndex], &buffer.getBuffer()[0], sizeof(float) * n);
                memcpy(&getBuffer()[0], &buffer.getBuffer()[0], sizeof(float) * (lastPushSize - n));
            }
            pushIndex %= getBuffer().size();


		}
	}
	// returns the index at which the last push was done. This means, the index of the newest data.
    size_t getPushIndex() const {return pushIndex; }
    
    
    /// get/set the number of buffers stored by the circular buffer. this is the amount of times push can be called before starting to overrite the oldest data
    void setNumBuffersToStore(size_t n){numBuffers = n;}
    size_t getNumBuffersToStore(){return numBuffers;}
	
private:
    size_t numBuffers = 100;
    
    size_t lastPushSize = 0;
    
	size_t pushIndex = 0;
};
