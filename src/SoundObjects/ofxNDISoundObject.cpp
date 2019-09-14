//
//  ofxNDISoundObject.cpp
//
//  Created by Roy Macdonald on 9/14/19.
//
//

#include "ofxNDISoundObject.h"
//--------------------------------------------------------------------------
//---------- NDI SENDER
//--------------------------------------------------------------------------
ofxNDISenderSoundObject::ofxNDISenderSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	bMute = true;
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setup(const std::string& name, const std::string & group){
	if(sender_.setup(name, group)) {
		audio_.setup(sender_);
	}
	else {
		ofLogError("NDISenderObject::setup") << "NDI setup failed.";
	}
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
	if(bMute){
		output.set(0);
	}else{
		output = input;
	}
	if(sender_.isSetup()){
		audio_.send(input);
	}
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setMuteOutput(bool bMute){
	this->bMute = bMute;
}
//--------------------------------------------------------------------------
bool ofxNDISenderSoundObject::isMuteOutput(){
	return bMute;
}
//--------------------------------------------------------------------------
//---------- NDI RECEIVER
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::setup(const std::string& name_or_url, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string> extra_ips){
	
	bool bFound = false;
	
	if(name_or_url == "") {
		bFound = false;
	}else{
		auto sources = ofxNDI::listSources(waittime_ms, location, group, extra_ips);
		
		auto found = find_if(begin(sources), end(sources), [name_or_url](const ofxNDI::Source &s) {
			return ofIsStringInString(s.p_ndi_name, name_or_url) || ofIsStringInString(s.p_url_address, name_or_url);
		});
		
		if(found == end(sources)) {
			ofLogWarning("ofxNDI") << "no NDI source found by string:" << name_or_url;
			bFound = false;
		}else{
			source = *found;
			bFound = true;
		}
	}
	if(bFound){
		bAudioNeedsSetup = receiver_.setup(source);
	}else{
		bAudioNeedsSetup = receiver_.setup();
	}
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
	if(receiver_.isSetup()){
		if(bAudioNeedsSetup){
			bAudioNeedsSetup = false;
			audio_.setup(receiver_);
			audio_.setSampleRate(output.getSampleRate());
			audio_.setNumChannels(output.getNumChannels());
			audio_.setNumSamples(output.getNumFrames());
		}
		if(receiver_.isConnected()) {
			audio_.update();
			if(audio_.isFrameNew()) {
				audio_.decodeTo(output);
			}
		}
	}else{
		// if the receiver_ object is not setup just place silence in the out buffer
		output.set(0);
	}
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::isConnected(){
	return receiver_.isConnected();
}
//--------------------------------------------------------------------------
std::string ofxNDIReceiverSoundObject::getSourceName(){
	return source.p_ndi_name;
}
//--------------------------------------------------------------------------
std::string ofxNDIReceiverSoundObject::getSourceUrl(){
	return source.p_url_address;
}
//--------------------------------------------------------------------------
