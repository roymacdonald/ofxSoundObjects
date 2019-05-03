//
//  ofxSoundMultiplexer.h
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#pragma once
#include "ofxSoundObject.h"
#include <map>
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
    
    bool deleteChannelGroup(const std::vector<int>& group);
    
    std::map < std::vector<int>, ofxSoundObject>& getChannelGroups();
    
    const std::map < std::vector<int>, ofxSoundObject>& getChannelGroups() const ;
protected:
    std::map < std::vector<int>, ofxSoundObject> channelsMap;
    
};

//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
class ofxSoundInputMultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundInput{
public:
	virtual void audioIn(ofSoundBuffer &input) override;
};
//--------------------------------------------------------------
//  ofxSoundBaseMultiplexer
//--------------------------------------------------------------
class ofxSoundOutputMultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundOutput{
public:
	virtual void audioOut(ofSoundBuffer &output) override;
private:

};
