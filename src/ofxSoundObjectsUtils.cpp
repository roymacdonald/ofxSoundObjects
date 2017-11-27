//
//  ofxSoundObjectsUtils.cpp
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#include "ofxSoundObjectsUtils.h"

//--------------------------------------------------------------
void ofxSoundObjects::getBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, std::vector<int> group){
	auto channels = sourceBuffer.getNumChannels();
    if(channels == 0) {
		ofLogWarning("ofxSoundBaseMultiplexer") << "getChannels requested on empty buffer";
		return;
	}
    ofRemove(group, [&](int & i){return i >= channels;});
    if(group.size() == 0){
        ofLogWarning("ofxSoundBaseMultiplexer") << "no valid group indices";
        return;
    }
	if (group.size() >= channels){
		ofLogNotice("ofxSoundBaseMultiplexer") << "getChannels requested more channels than available ";
	}

//	targetBuffer.setNumChannels(group.size());
	targetBuffer.setSampleRate(sourceBuffer.getSampleRate());
    auto nFrames = sourceBuffer.getNumFrames();
//	if(channels == 1){
//		sourceBuffer.copyTo(targetBuffer, nFrames, 0, 0);
//	}else{
        auto & buffer = sourceBuffer.getBuffer();
		targetBuffer.allocate(nFrames, group.size());
        for (std::size_t k = 0; k < group.size(); k++) {
            for(std::size_t i = 0; i < nFrames; i++){
                targetBuffer[k + i * group.size()] = buffer[group[k] + i * channels];
            }
        }
//	}
}
//--------------------------------------------------------------
void ofxSoundObjects::setBufferFromChannelGroup(const ofSoundBuffer & sourceBuffer, ofSoundBuffer & targetBuffer, const std::vector<int>& group){
//    auto channels = std::max(targetBuffer.getNumChannels(), (std::size_t)*std::max_element(group.begin(), group.end()));
//    targetBuffer.setNumChannels(channels);
//	targetBuffer.resize(sourceBuffer.getNumFrames() * channels);
    if(sourceBuffer.getSampleRate() != targetBuffer.getSampleRate()){
        ofLogWarning("ofxSoundBaseMultiplexer::setChannels") << "Different sample rates in buffers!! sourceBuffer: " <<sourceBuffer.getSampleRate() << "  targetBuffer: " << targetBuffer.getSampleRate();
    }
    auto nFrames = targetBuffer.getNumFrames();
    auto & buffer = targetBuffer.getBuffer();
    for (std::size_t k = 0; k < group.size(); k++) {
        for(std::size_t i = 0; i < nFrames; i++){
            buffer[group[k]+ i * targetBuffer.getNumChannels()] = sourceBuffer[k + i * group.size()];
        }
    }
}
//--------------------------------------------------------------
bool ofxSoundObjects::checkBuffers(const ofSoundBuffer& src, ofSoundBuffer& dst, bool bSetDst){
    if(dst.size()!=src.size()) {
        ofLogVerbose("ofxSoundObject") << "working buffer size != output buffer size. " << dst.size() << " != " <<src.size();
        if(bSetDst){
            dst.resize(src.size());
            dst.setNumChannels(src.getNumChannels());
            dst.setSampleRate(src.getSampleRate());
        }
        return true;
    }
    return false;
}
