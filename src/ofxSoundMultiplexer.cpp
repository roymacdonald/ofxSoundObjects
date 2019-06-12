//
//  ofxSoundBufferMultiplexer.cpp
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#include "ofxSoundMultiplexer.h"
#include "ofxSoundUtils.h"
#include "ofUtils.h"
#include "ofLog.h"

//--------------------------------------------------------------
//  ofxSoundBaseMultiplexer
//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::getOrCreateChannel(int channel){
	return getOrCreateChannelGroup({channel});
}
//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::getOrCreateChannelGroup(const std::vector<int>& group){
	ofLogVerbose("ofxSoundBaseMultiplexer::getOrCreateChannelGroup") << ofToString(group) << " size: " << group.size();
    auto it = channelsMap.find(group);
    if(it != channelsMap.end()){
        ofLogNotice("ofxSoundInput::makeChannelGroup") << "channel group already present.";
        return it->second;
    }
	for(auto& g: group){
		channelsSet.insert(g);
	}
	channelsMap.emplace(group, ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR));
	return channelsMap[group];
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelGroup(const std::vector<int>& group){
	for(auto& g: group){
		channelsSet.erase(g);
	}
    return (bool) channelsMap.erase(group);
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannel(int chan){
	return deleteChannelGroup({chan});
}
//--------------------------------------------------------------
std::map < std::vector<int>, ofxSoundObject>& ofxSoundBaseMultiplexer::getChannelGroups(){
    return channelsMap;
}
//--------------------------------------------------------------
const std::map < std::vector<int>, ofxSoundObject>& ofxSoundBaseMultiplexer::getChannelGroups() const {
    return channelsMap;
}

//--------------------------------------------------------------
//  ofxSoundInputMultiplexer
//--------------------------------------------------------------

void ofxSoundInputMultiplexer::audioIn(ofSoundBuffer &input) {
    ofxSoundUtils::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
    
    for(auto & m: channelsMap){
        ofxSoundUtils::getBufferFromChannelGroup(inputBuffer, m.second.getBuffer(), m.first);
    }
}
//--------------------------------------------------------------
ofxSoundObject & ofxSoundInputMultiplexer::connectChannelTo(int channel,ofxSoundObject &soundObject){
	return getOrCreateChannelGroup({channel}).connectTo(soundObject);	
}

//-------------------------------------------------------------- 
void ofxSoundInputMultiplexer::disconnectChannel(int channel){
	deleteChannelGroup({channel});
}
//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
size_t ofxSoundOutputMultiplexer::getNumChannels(){
	return channelsSet.size();
}
//--------------------------------------------------------------
void ofxSoundOutputMultiplexer::audioOut(ofSoundBuffer &output){
	// this could be multithreaded as in the matrix mixer
    //pull the audio from each channel group
    for(auto & m: channelsMap){
        ofSoundBuffer temp;
        temp.allocate(output.getNumFrames(), m.first.size());
        temp.setSampleRate(output.getSampleRate());
        m.second.audioOut(temp);
    }
    //muxing all groups
    for(auto & m: channelsMap){
        ofxSoundUtils::setBufferFromChannelGroup(m.second.getBuffer(), output, m.first);
    }
}

