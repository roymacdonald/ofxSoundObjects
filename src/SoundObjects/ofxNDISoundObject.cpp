//
//  ofxNDISoundObject.cpp
//
//  Created by Roy Macdonald on 9/14/19.
//
//

#include "ofxNDISoundObject.h"
#include "ofLog.h"
//--------------------------------------------------------------------------
//---------- NDI SENDER
//--------------------------------------------------------------------------
ofxNDISenderSoundObject::ofxNDISenderSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	bMute = true;
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setup(const std::string& name, const std::string & group){
#ifndef OFX_SOUND_OBJECTS_USE_OFX_NDI
	ofLogError("ofxNDISenderSoundObject::setup") << "Use of ofxNDI is disabled.";
#else
	if(sender_.setup(name, group)) {
		audio_.setup(sender_);
	}
	else {
		ofLogError("NDISenderObject::setup") << "NDI setup failed.";
	}
#endif
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
	if(bMute){
		output.set(0);
	}else{
		output = input;
	}
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	if(sender_.isSetup()){
		audio_.send(input);
	}
#endif
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
#ifndef OFX_SOUND_OBJECTS_USE_OFX_NDI
void ofxNDIReceiverSoundObject::setup(const std::string& name_or_url, const std::string &group){
	ofLogError("ofxNDIReceiverSoundObject::setup") << "Use of ofxNDI is disabled.";
}
#else
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
#endif
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
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
	}else
#endif
	{
		// if the receiver_ object is not setup just place silence in the out buffer
		output.set(0);
	}
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::isConnected(){
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	return receiver_.isConnected();
#else
	return false;
#endif
}
//--------------------------------------------------------------------------
std::string ofxNDIReceiverSoundObject::getSourceName(){
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	return source.p_ndi_name;
#else
	return "ofxNDI is disabled";
#endif
}
//--------------------------------------------------------------------------
std::string ofxNDIReceiverSoundObject::getSourceUrl(){
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	return source.p_url_address;
#else
	return "ofxNDI is disabled";
#endif
}
//--------------------------------------------------------------------------
