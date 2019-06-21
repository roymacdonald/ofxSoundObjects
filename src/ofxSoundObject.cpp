/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundObject.h"
#include "ofxSoundUtils.h"
#include "ofSoundStream.h"
#include "ofLog.h"
//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
ofxSoundObject::ofxSoundObject() {
	ofLogWarning("ofxSoundObject::ofxSoundObject()", "the ofxSoundObjects' no args constructor should not be called. ");
}
ofxSoundObject::ofxSoundObject(ofxSoundObjectsType t){
	type = t;
}
//--------------------------------------------------------------
ofxSoundObject &ofxSoundObject::connectTo(ofxSoundObject &soundObject) {
    if (outputObject != nullptr) {
        disconnect();
    }
    outputObject = &soundObject;
	soundObject.setInput(this);

	// if we find an infinite loop, we want to disconnect and provide an error
	if(!checkForInfiniteLoops()) {
		ofLogError("ofxSoundObject") << "There's an infinite loop in your chain of ofxSoundObjects";
		disconnect();
	}
	checkSignalFlowMode();
	return soundObject;
}
//--------------------------------------------------------------
void ofxSoundObject::disconnectInput(ofxSoundObject * input){
    if (inputObject != nullptr) {
		inputObject = nullptr;
    }
}
//--------------------------------------------------------------
void ofxSoundObject::disconnect(){
	if(outputObject != nullptr){
		outputObject->disconnectInput(this);
		outputObject =nullptr;
	}
}
//--------------------------------------------------------------
void ofxSoundObject::setInput(ofxSoundObject *obj) {
	inputObject = obj;
}
//--------------------------------------------------------------
ofxSoundObject *ofxSoundObject::getInputObject() {
	return inputObject;
}
//--------------------------------------------------------------
ofxSoundObject *ofxSoundObject::getOutputObject(){
	return outputObject;
}
//--------------------------------------------------------------
bool ofxSoundObject::checkForInfiniteLoops() {
	ofxSoundObject *prev = inputObject;

	// move up the dsp chain until we find ourselves or the beginning of the chain (input==nullptr)
	while(prev!=this && prev!=nullptr) {
		prev = prev->getInputObject();
	}

	// if we found ourselves, return false (to indicate there's an infinite loop)
	return (prev==nullptr);
}
//--------------------------------------------------------------
ofxSoundObject* ofxSoundObject::getSignalSourceObject(){
	if(type == OFX_SOUND_OBJECT_SOURCE )return this;
	//I kinda dont like this. It needs some reworking.
	if(chanMod == OFX_SOUND_OBJECT_CHAN_MUX || chanMod == OFX_SOUND_OBJECT_CHAN_MIXER) return this;
	if(inputObject == nullptr){
		// there's nothing connected to this object so it should be the first, although this does not assure it is a source object. 
		return this;
	}else{
		return inputObject->getSignalSourceObject();
	}
	ofLogWarning("ofxSoundObject::getSignalSourceObject", "There is no source on your signal chain so most probaly you will get no sound " + this->getName());
	return nullptr;
}
//--------------------------------------------------------------
ofxSoundObject* ofxSoundObject::getSignalDestinationObject(){
	if(type == OFX_SOUND_OBJECT_DESTINATION)return this; // it is a destination object like an ofSoundOutput

	if(outputObject == nullptr) return this; // it is at the end of the signal chain, so it most probably is the output.
	
	return outputObject->getSignalDestinationObject();
	
	
	ofLogWarning("ofxSoundObject::getSignalDestinationObject", "There is no destination on your signal chain so most probaly you will get no sound");
	return nullptr;
}
//--------------------------------------------------------------
// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioOut(ofSoundBuffer &output) {
	if(signalFlowMode == OFX_SOUND_OBJECT_PULL){
		ofxSoundUtils::checkBuffers(output, workingBuffer);
		if(inputObject!=nullptr) {
			inputObject->audioOut(workingBuffer);
		}
		this->process(workingBuffer, output);
	}
}
//--------------------------------------------------------------
// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioIn(ofSoundBuffer &input) {
	ofxSoundUtils::checkBuffers(input, inputBuffer);
	if(signalFlowMode == OFX_SOUND_OBJECT_PUSH){
		this->process(input, inputBuffer);
		if(outputObject!=nullptr) {
			outputObject->audioIn(inputBuffer);
		}
	}else{
		input.copyTo(inputBuffer);
	}
}
//--------------------------------------------------------------
size_t ofxSoundObject::getNumChannels(){
	if(getOutputStream()){
		auto ss = getOutputStream()->getSoundStream();
		if(ss) return ss->getOutDevice().outputChannels;
	}
    return workingBuffer.getNumChannels();
}
//--------------------------------------------------------------
ofSoundBuffer& ofxSoundObject::getBuffer(){
return workingBuffer;
}
//--------------------------------------------------------------
const ofSoundBuffer& ofxSoundObject::getBuffer() const{
return workingBuffer;    
}
//--------------------------------------------------------------
void ofxSoundObject::setOutputStream(ofSoundStream& stream){
	setOutputStream(&stream);
}
//--------------------------------------------------------------
void ofxSoundObject::setOutputStream(ofSoundStream* stream){
	if(stream != nullptr){
		outputStream = stream;
		outputStream->setOutput(this);
	}
}
//--------------------------------------------------------------
ofSoundStream* ofxSoundObject::getOutputStream(){
	return outputStream; 
}

//--------------------------------------------------------------
void ofxSoundObject::setInputStream(ofSoundStream& stream){
	setInputStream(&stream);
}
//--------------------------------------------------------------
void ofxSoundObject::setInputStream(ofSoundStream* stream){
	if(stream != nullptr){
		inputStream = stream;
		inputStream->setInput(this);
	}
}
//--------------------------------------------------------------
ofSoundStream* ofxSoundObject::getInputStream(){
	return inputStream;
}

//--------------------------------------------------------------
int ofxSoundObject::getInputDeviceId(){
	if(getInputStream()){
		auto ss = getInputStream()->getSoundStream();
		if(ss) return ss->getInDevice().deviceID;
	}
	return inputBuffer.getDeviceID();

}
//--------------------------------------------------------------
int ofxSoundObject::getOutputDeviceId(){
	if(getOutputStream()){
		auto ss = getOutputStream()->getSoundStream();
		if(ss) return ss->getOutDevice().deviceID;
	}
	return getBuffer().getDeviceID();
}
//--------------------------------------------------------------
ofSoundDevice ofxSoundObject::getInputDeviceInfo(){
	return ofxSoundUtils::getSoundDeviceInfo(this->getInputDeviceId());
}
//--------------------------------------------------------------
ofSoundDevice ofxSoundObject::getOutputDeviceInfo(){
	return ofxSoundUtils::getSoundDeviceInfo(this->getOutputDeviceId());
}
//--------------------------------------------------------------
void ofxSoundObject::setSignalFlowMode(const  ofxSoundObjectsMode & newMode){
	signalFlowMode = newMode;
	if(signalFlowMode == OFX_SOUND_OBJECT_PULL && inputObject != nullptr){
		inputObject->setSignalFlowMode(signalFlowMode);
		return;
	}
	if(signalFlowMode == OFX_SOUND_OBJECT_PUSH && outputObject != nullptr){
		outputObject->setSignalFlowMode(signalFlowMode);
		return;
	}
}
//--------------------------------------------------------------
void ofxSoundObject::checkSignalFlowMode(){
	ofxSoundObject* dest = getSignalDestinationObject();
	if(dest != nullptr){
		if(dest->getType() == OFX_SOUND_OBJECT_DESTINATION){
			dest->setSignalFlowMode(OFX_SOUND_OBJECT_PULL);
			return;
		}
	}
	ofxSoundObject* src = getSignalSourceObject();
	if(src != nullptr){
		if(src->getType() == OFX_SOUND_OBJECT_SOURCE){
			src->setSignalFlowMode(OFX_SOUND_OBJECT_PUSH);
			return;
		}
	}
}
//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
size_t ofxSoundInput::getNumChannels(){
	if(getInputStream()){
		auto ss = getInputStream()->getSoundStream();
		if(ss) return ss->getInDevice().inputChannels;
	}
	return inputBuffer.getNumChannels();
}
//--------------------------------------------------------------
// copy audio in to internal buffer
void ofxSoundInput::audioIn(ofSoundBuffer &input) {
    ofxSoundUtils::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
}
//--------------------------------------------------------------
void ofxSoundInput::audioOut(ofSoundBuffer &output) {
    ofxSoundUtils::checkBuffers(output, inputBuffer);
	inputBuffer.copyTo(output);
}

//--------------------------------------------------------------
//  ofxSoundOutput
//--------------------------------------------------------------
//ofxSoundOutput::ofxSoundOutput():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION) {
//}

//--------------------------------------------------------------
