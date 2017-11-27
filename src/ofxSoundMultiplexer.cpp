//
//  ofxSoundBufferMultiplexer.cpp
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#include "ofxSoundMultiplexer.h"
#include "ofxSoundObjectsUtils.h"

//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::getOrCreateChannelGroup(const std::vector<int>& group){
    std::cout << "ofxSoundBaseMultiplexer::getOrCreateChannelGroup " << ofToString(group) << " size: " << group.size() << endl;
    auto it = channelsMap.find(group);
    if(it != channelsMap.end()){
        ofLogNotice("ofxSoundInput::makeChannelGroup") << "channel group already present.";
        return it->second;
    }
    return channelsMap[group];
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelGroup(const std::vector<int>& group){
    return (bool) channelsMap.erase(group);
}
//--------------------------------------------------------------
void ofxSoundInputMultiplexer::audioIn(ofSoundBuffer &input) {
    ofxSoundObjects::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
    
    for(auto & m: channelsMap){
        ofxSoundObjects::getBufferFromChannelGroup(inputBuffer, m.second.getBuffer(), m.first);
    }
}
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
        ofxSoundObjects::setBufferFromChannelGroup(m.second.getBuffer(), output, m.first);
    }
}