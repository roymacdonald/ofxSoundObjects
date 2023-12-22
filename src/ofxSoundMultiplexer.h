//
//  ofxSoundMultiplexer.h
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#pragma once

#include "ofxSoundObject.h"
#include "ofLog.h"
#include <map>
#include <set>
//--------------------------------------------------------------
//  ofxSoundInputMultiplexer
//--------------------------------------------------------------

class ofxSoundBaseMultiplexer{

public:
    /// Create a new channel group. This is mostly useful when using a multi input audio interface.
    /// The vector that is passed as argument contains the indexes of the inputs that will make the group.
    /// This function outputs the index at which the new group will be stored. If trying to add a group already present, it will return the position of the group that is already present and will not add a new one.
    /// The groups are then used to create independent ofSoundBuffer instances that will only contain the audio data of its corresponding channels, which will be passed to the subsequent ofxSoundObject chain. This not only allows flexible routing but also reduces the amount of data being passed in the processing chain.
    ofxSoundObject& getOrCreateChannelGroup(const std::vector<int>& group);
    ofxSoundObject& getOrCreateChannel(int channel);
    
    
    bool deleteChannelGroup(const std::vector<int>& group);
    bool deleteChannel(int chan);
    
    ///\brief will clear all channels
    void clear();
    
    std::map < std::vector<int>, ofxSoundObject>& getChannelGroups();
    
    const std::map < std::vector<int>, ofxSoundObject>& getChannelGroups() const ;
        
    
    
    
protected:
    
    virtual void setObjectsInput(ofxSoundObject &soundObject) = 0;
    virtual void connectToThis(ofxSoundObject &soundObject) = 0;
    
//    ofxSoundObject &_connectTo(ofxSoundObject &soundObject) {
        
//        this->setObjectsInput(soundObject);
//        soundObject.setInput(this);
        // if we find an infinite loop, we want to disconnect and provide an error
//        if(!checkForInfiniteLoops()) {
//            ofLogError("ofxSoundObject") << "There's an infinite loop in your chain of ofxSoundObjects";
//            disconnect();
//        }
//        return soundObject;
//    }
    //--------------------------------------------------------------
    void _disconnectInput(ofxSoundObject * input){
        
        std::vector<int> toRemove;
        for(auto& c: channelsMap){
            if(&c.second == input){
                toRemove = c.first;
                break;
            }
        }
        if(toRemove.size()){
            deleteChannelGroup(toRemove);
        }
    }
    //--------------------------------------------------------------
    void _disconnect(){
        clear();
    }
    
    
    void pullAndMuxBuffer(ofSoundBuffer& buffer);
    void demuxBuffer(ofSoundBuffer& buffer);
    
    std::map < std::vector<int>, ofxSoundObject> channelsMap;
	std::set<int> channelsSet;
//private:
    uint64_t  lastTick = 0;
    ofxSoundObjectsChannelCountModifier muxType;
	
};

class ofxSoundDemultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundObject{
public:
    ofxSoundDemultiplexer():ofxSoundObject(){
        muxType = chanMod = OFX_SOUND_OBJECT_CHAN_DEMUX;
        setName ("Sound Demultiplexer");
    }

    /// this will connect a specific channel to the passes sound object.
    /// It will work as a kind of splitter.
    ofxSoundObject &connectChannelTo(int channel,ofxSoundObject &soundObject);
    
    /// Disconnect a specific channel, one connected using connectChannelTo(...)
    OF_DEPRECATED_MSG("Use deleteChannel(int channel).", void disconnectChannel(int channel));
    
    
    virtual ofxSoundObject &connectTo(ofxSoundObject &soundObject)override {
        ofLogWarning("ofxSoundDemultiplexer::connectTo") << "Object's name: " << getName() << ". This function should not be called. Instead use the getOrCreateChannel function to make a connection from this object. ";
        return soundObject;
    }
    
    virtual size_t getNumChannels() override{
        return channelsSet.size();
    }
    
    virtual void audioOut(ofSoundBuffer &output) override;
protected:
    virtual void setObjectsInput(ofxSoundObject &soundObject) override{
        // setting this will allow the connected object to call audioOut
        soundObject.setInput(this);
    }
    virtual void connectToThis(ofxSoundObject &soundObject) override{}
private:
//    ofSoundBuffer workBuffer;
    
};

//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
class ofxSoundInputMultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundInput{
public:
	ofxSoundInputMultiplexer():ofxSoundInput(){
        muxType = chanMod = OFX_SOUND_OBJECT_CHAN_DEMUX;
		setName ("Sound Input Demultiplexer");
	}
    virtual ofxSoundObject &connectTo(ofxSoundObject &soundObject)override {
        ofLogWarning("ofxSoundInputMultiplexer::connectTo") << "Object's name: " << getName() << ". This function should not be called. Instead use the getOrCreateChannel function to make a connection from this object. ";
        return soundObject;
    }
	/// this will connect a specific channel to the passes sound object.
	/// It will work as a kind of splitter.
	ofxSoundObject &connectChannelTo(int channel,ofxSoundObject &soundObject);
	
	/// Disconnect a specific channel, one connected using connectChannelTo(...) 
	void disconnectChannel(int channel);
    
    virtual size_t getNumChannels() override{
        return channelsSet.size();
    }
	
	virtual void audioIn(ofSoundBuffer &input) override;
    
protected:
    virtual void setObjectsInput(ofxSoundObject &soundObject) override{
        //we dont need to set the input because the buffers of each object will be filled by the audio in call.
//        soundObject.setInput(this);
    }
    virtual void connectToThis(ofxSoundObject &soundObject) override{}

};
//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
class ofxSoundOutputMultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundOutput{
public:
	ofxSoundOutputMultiplexer():ofxSoundOutput(){
        muxType = chanMod = OFX_SOUND_OBJECT_CHAN_MUX;
		setName ("Sound Output Multiplexer");
	}
	virtual size_t getNumChannels() override;

	virtual void audioOut(ofSoundBuffer &output) override;
private:
protected:
    virtual void disconnectInput(ofxSoundObject * input)override{
        
    }
    virtual void setObjectsInput(ofxSoundObject &soundObject) override{
//        soundObject.setInput(this);
    }
    virtual void connectToThis(ofxSoundObject &soundObject) override{
        soundObject.connectTo(*this);
    }
    
};

