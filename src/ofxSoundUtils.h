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
#include <atomic>
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

    static size_t millisToSamples(float millis, float sampleRate){
        return (millis*sampleRate/1000.0);
    }

    static float samplesToMillis(size_t samples, float sampleRate){
        return (samples*1000.0/sampleRate);
    }
};
//--------------------------------------------------------------
class ofxCircularSoundBuffer: public ofSoundBuffer{
public:
    ofxCircularSoundBuffer():ofSoundBuffer(),
    bNeedsAllocation(false)
//    newAllocSize(0)
    {
        
    }
    
    void push(const float* src, const size_t& srcSizePerChannel, int numChannels, int sampleRate){
        if(size() == 0 || getNumChannels() == 0 || bNeedsAllocation.load()){//} || getNumFrames() != buffer.getNumFrames() * numBuffers){
//            if(bufferLengthInMs != 0){
            if(!bSetNumBuffers && bufferLengthInMs == 0){
                numBuffers = 100;
                bSetNumBuffers = true;
            }
            if(bSetNumBuffers){
                bufferLengthInMs = (srcSizePerChannel *numBuffers)/(sampleRate/1000.0f);
                bSetNumBuffers = false;
            }
            
                size_t allocSize = (sampleRate/1000.0f)* bufferLengthInMs;
                allocate(allocSize, numChannels);
                numBuffers = allocSize/srcSizePerChannel;
                
//            }else{
//                if(numBuffers == 0) numBuffers = 100;
//                size_t allocSize = srcSizePerChannel* numBuffers;
//                allocate(allocSize, numChannels);
//            }
            setSampleRate(sampleRate);
            if(  getBuffer().size()){
            bNeedsAllocation = false;
            pushIndex %= getBuffer().size();
            }
        }
        if(getBuffer().size() > 0){
            
            lastPushSize = srcSizePerChannel * numChannels;
            if(pushIndex + lastPushSize <= getBuffer().size()){
                memcpy(&getBuffer()[pushIndex], src, sizeof(float) * lastPushSize);
            }else{
                size_t n = getBuffer().size() - pushIndex;
                memcpy(&getBuffer()[pushIndex], src, sizeof(float) * n);
                if(lastPushSize - n > 0){
                    memcpy(&getBuffer()[0], &(src[n]), sizeof(float) * (lastPushSize - n));
                }
            }
            pushIndex += lastPushSize;

            pushIndex %= getBuffer().size();
        }
        
    }
    
	void push(const ofSoundBuffer& buffer){
        push(buffer.getBuffer().data(), buffer.getNumFrames(), buffer.getNumChannels(), buffer.getSampleRate());
	}
	// returns the index at which the last push was done. This means, the index of the newest data.
    size_t getPushIndex() const {return pushIndex; }
    
    size_t getLastPushSize() const {return lastPushSize;}
    
    /// get/set the number of buffers stored by the circular buffer. this is the amount of times push can be called before starting to overrite the oldest data
    void setNumBuffersToStore(size_t n){
        if(numBuffers != n){
            bNeedsAllocation = true;
            bSetNumBuffers = true;
            numBuffers = n;
        }
    }
    
    size_t getNumBuffersToStore(){
        return numBuffers;
    }
    
    size_t getBufferLengthInMs(){return bufferLengthInMs; }
    void setBufferLengthInMs(size_t lengthMs){
        if(bufferLengthInMs != lengthMs){
            bNeedsAllocation = true;
            bSetNumBuffers = false;
            bufferLengthInMs = lengthMs;
        }
    }
    
    
    // extract part of a channel from the circular buffer into an ofSoundBuffer.
    void copyIntoBuffer( std::shared_ptr<ofSoundBuffer>& dest, size_t samplesToCopy, size_t channel, size_t startFrame, bool bAppend){
        if(!dest){
            ofLogError("ofxCircularSoundBuffer::copyIntoBuffer") << "Destination buffer is null";
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
    
    /// copy part of the circularBuffer into an ofSoundBuffer
    void copyIntoBuffer( ofSoundBuffer& dest, size_t startFrame){
//        auto samplesToCopy = dest.size();
        
//        if(samplesToCopy > this->size()){
//            ofLogWarning("ofxCircularSoundBuffer::copyIntoBuffer") << "samples to copy is larger than the circular buffersize. will only copy whole circular buffer";
//            samplesToCopy = this->size();
//        }
        auto nc = this->getNumChannels();
        if(nc != dest.getNumChannels()){
            dest.allocate(dest.getNumFrames(), nc);
        }
        
//        size_t startDestIndex = 0;
//        if(dest.size() != samplesToCopy){
//            dest.resize(samplesToCopy);
//        }
        
//        auto nc = this->getNumChannels();
        ///TODO: optimize this with a memcpy
        size_t j = 0;
        for(size_t i = startFrame * nc ; i < this->size() && j < dest.size(); i++){
            dest.getBuffer()[j] = buffer[i];
            j++;
        }
        
        if(j < dest.size()){
            for(size_t i = 0; i < this->size() && j < dest.size(); i++){
                dest.getBuffer()[j] = buffer[i];
                j++;
            }
        }
    }
    
    
    
    
	
private:
    std::atomic<bool> bNeedsAllocation;
    bool bSetNumBuffers = false;
//    std::atomic<size_t> newAllocSize;
    size_t numBuffers = 0;
    size_t bufferLengthInMs = 0;
    
    size_t lastPushSize = 0;
    
	size_t pushIndex = 0;
};
