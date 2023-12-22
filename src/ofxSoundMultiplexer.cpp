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
    std::stringstream groupName;
	for(auto& g: group){
		channelsSet.insert(g);
        groupName << "_" << g;
	}
	channelsMap.emplace(group, ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR));
    
    
    
    if(muxType == OFX_SOUND_OBJECT_CHAN_DEMUX){
        this->setObjectsInput(channelsMap[group]);
        groupName << "_DEMUX";
    }else if(muxType == OFX_SOUND_OBJECT_CHAN_MUX){
        this->connectToThis(channelsMap[group]);
        groupName << "_MUX";
    }else{
        ofLogError("ofxSoundBaseMultiplexer::getOrCreateChannelGroup") << "muxType is set wrong!.";
    }
    
    channelsMap[group].setName(  groupName.str() );
    
	return channelsMap[group];
}

//--------------------------------------------------------------
void ofxSoundBaseMultiplexer::clear(){
    channelsMap.clear();
    channelsSet.clear();
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
void ofxSoundBaseMultiplexer::pullAndMuxBuffer(ofSoundBuffer& buffer){
    for(auto & m: channelsMap){
        ofSoundBuffer temp;
        temp.allocate(buffer.getNumFrames(), m.first.size());
        temp.setSampleRate(buffer.getSampleRate());
        
        temp.setTickCount(buffer.getTickCount());
        temp.setDeviceID(buffer.getDeviceID());

        m.second.audioOut(temp);
    }
    //muxing all groups
    for(auto & m: channelsMap){
        ofxSoundUtils::setBufferFromChannelGroup(m.second.getBuffer(), buffer, m.first);
    }
}

//--------------------------------------------------------------
void ofxSoundBaseMultiplexer::demuxBuffer(ofSoundBuffer& buffer){
    for(auto & m: channelsMap){
        ofxSoundUtils::getBufferFromChannelGroup(buffer, m.second.getBuffer(), m.first);
    }
}


//--------------------------------------------------------------
//  ofxSoundDemultiplexer
//--------------------------------------------------------------
void ofxSoundDemultiplexer::audioOut(ofSoundBuffer &output) {
    //this might get called several times by each connected object
    _tickCount = output.getTickCount();
    if(lastTick < output.getTickCount()){
        lastTick = output.getTickCount();
        if(workingBuffer.size() != output.getNumFrames() * getNumChannels() || workingBuffer.getNumChannels() != getNumChannels()){
            workingBuffer.allocate(output.getNumFrames(), getNumChannels());
        }
        workingBuffer.setSampleRate(output.getSampleRate());
        workingBuffer.setTickCount(output.getTickCount());
        workingBuffer.setDeviceID(output.getDeviceID());
        
        ofxSoundObject::audioOut(workingBuffer);
        demuxBuffer(workingBuffer);
    }
}
//--------------------------------------------------------------
//  ofxSoundInputMultiplexer
//--------------------------------------------------------------

void ofxSoundInputMultiplexer::audioIn(ofSoundBuffer &input) {
    inputBuffer = input;
    demuxBuffer(inputBuffer);
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
//    printAudioOut();
    
    _tickCount = output.getTickCount();
    pullAndMuxBuffer(output);
}

