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
	channelsMap.emplace(group, ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR));
	return channelsMap[group];
//	return (channelsMap[group] = ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR));
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelGroup(const std::vector<int>& group){
    return (bool) channelsMap.erase(group);
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
//----------------------------------------------------
//void ofxSoundInputMultiplexer::disconnectInput(ofxSoundObject * input){
//	auto it = channelsPtrMap.find(input);
//	if(it != channelsPtrMap.end()){
//		channelsPtrMap.erase(it);
//	}
//}
////----------------------------------------------------
//void ofxSoundInputMultiplexer::setInput(ofxSoundObject *obj){
//	auto it = channelsPtrMap.find(obj);
//	if(it != channelsPtrMap.end()){
//		ofLogNotice("ofxSoundInputMultiplexer::setInput") << " already connected";
//		return;
//	}
	
//	for (int i =0; i<channels.size(); i++) {
//		if (obj == channels[i]) {
//			ofLogNotice("ofxSoundMixer::setInput") << " already connected" << endl;
//			return;
//		}
//	}
//	channels.push_back(obj);
//	channelVolume.push_back(1);//default volume for channel is 1
//}
void ofxSoundInputMultiplexer::audioIn(ofSoundBuffer &input) {
    ofxSoundUtils::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
    
    for(auto & m: channelsMap){
        ofxSoundUtils::getBufferFromChannelGroup(inputBuffer, m.second.getBuffer(), m.first);
    }
//	for(auto& c : channelsPtrMap){
//		
//	}
	
	
}
//--------------------------------------------------------------
ofxSoundObject & ofxSoundInputMultiplexer::connectChannelTo(int channel,ofxSoundObject &soundObject){
//	auto it = channelsPtrMap.find(&soundObject);
//	if(it != channelsPtrMap.end()){
//		it->second = channel;
//	}else{
//		channelsPtrMap[&soundObject] = channel;
//	}
//	soundObject.setInput(this);
//	
	return getOrCreateChannelGroup({channel}).connectTo(soundObject);
	
}

//-------------------------------------------------------------- 
void ofxSoundInputMultiplexer::disconnectChannel(int channel){

//	for(auto it = channelsPtrMap.begin(); it != channelsPtrMap.end(); ){
//		if(it->second == channel){
//			it = channelsPtrMap.erase(it);
//		}else{
//			++it;
//		}
//	}
	deleteChannelGroup({channel});
}
//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
void ofxSoundOutputMultiplexer::audioOut(ofSoundBuffer &output){
    //ofxSoundObject::audioOut(output);
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

