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
#include "ofLog.h"

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
            
            lastPushSize = buffer.getBuffer().size();
            if(pushIndex + lastPushSize < getBuffer().size()){
                memcpy(&getBuffer()[pushIndex], &buffer.getBuffer()[0], sizeof(float) * lastPushSize);
            }else{
                size_t n = getBuffer().size() - pushIndex;
                memcpy(&getBuffer()[pushIndex], &buffer.getBuffer()[0], sizeof(float) * n);
                memcpy(&getBuffer()[0], &buffer.getBuffer()[n], sizeof(float) * (lastPushSize - n));
            }
            pushIndex += lastPushSize;

            pushIndex %= getBuffer().size();


		}
	}
	// returns the index at which the last push was done. This means, the index of the newest data.
    size_t getPushIndex() const {return pushIndex; }
    
    size_t getLastPushSize() const {return lastPushSize;}
    
    /// get/set the number of buffers stored by the circular buffer. this is the amount of times push can be called before starting to overrite the oldest data
    void setNumBuffersToStore(size_t n){numBuffers = n;}
    size_t getNumBuffersToStore(){return numBuffers;}
    
    
    // extract a part of the circular buffer into an ofSoundBuffer.
    void copyIntoBuffer( std::shared_ptr<ofSoundBuffer>& dest, size_t samplesToCopy, size_t channel, size_t startFrame, bool bAppend){
        if(!dest){
            ofLogError("ofxWebRTC_VAD::copyFromCircularBuffer") << "Destination buffer is null";
            return;
        }
        
        if(samplesToCopy > this->size()){
            ofLogWarning("ofxCircularSoundBuffer::copyIntoBuffer") << "samples to copy is larger than the circular buffersize. will only copy whole circular buffer";
            samplesToCopy = this->size();
        }
        
        size_t startDestIndex = 0;
        if(bAppend){
            startDestIndex = dest->size();
            dest->resize(dest->size() + samplesToCopy);
        }else{
            if(dest->size() != samplesToCopy){
                dest->resize(samplesToCopy);
            }
        }
        
        auto nc = this->getNumChannels();
        size_t j = startDestIndex;
        for(size_t i = startFrame * nc + channel; i < this->size() && j < dest->size(); i+= nc){
            dest->getBuffer()[j] = buffer[i];
            j++;
        }
        if(j < dest->size()){
            for(size_t i = channel; i < this->size() && j < dest->size(); i+= nc){
                dest->getBuffer()[j] = buffer[i];
                j++;
            }
        }
    }
    
	
private:
    size_t numBuffers = 100;
    
    size_t lastPushSize = 0;
    
	size_t pushIndex = 0;
};
