/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundObject.h"
#include "ofxSoundUtils.h"
#include "ofSoundStream.h"
#include "ofLog.h"
#include "ofEvents.h"
//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
ofxSoundObject::ofxSoundObject(const ofxSoundObject& a): _bBypass(a._bBypass.load())
{
    inputObject = a.inputObject;
    outputObjectRef = a.outputObjectRef;
    
    type = a.type;
    chanMod = a.chanMod;
    objectName = a.objectName;
    outputStream = a.outputStream;
    signalFlowMode = a.signalFlowMode;
    workingBuffer = a.workingBuffer;
    
}
ofxSoundObject::ofxSoundObject()
:_bBypass(false)
{
	ofLogWarning("ofxSoundObject::ofxSoundObject()") << "the ofxSoundObjects' no args constructor should not be called. ";
}
ofxSoundObject::ofxSoundObject(ofxSoundObjectsType t)
:_bBypass(false)
{
	type = t;
}
ofxSoundObject::~ofxSoundObject() {
    disconnect();
}
//--------------------------------------------------------------
ofxSoundObject &ofxSoundObject::connectTo(ofxSoundObject &soundObject) {
    if (outputObjectRef != nullptr) {
        disconnect();
    }
    outputObjectRef = &soundObject;
	soundObject.setInput(this);

	// if we find an infinite loop, we want to disconnect and provide an error
	if(!checkForInfiniteLoops()) {
		ofLogError("ofxSoundObject") << "There's an infinite loop in your chain of ofxSoundObjects";
		disconnect();
	}
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
	if(outputObjectRef != nullptr){
		outputObjectRef->disconnectInput(this);
		outputObjectRef =nullptr;
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
	//ofLogWarning("ofxSoundObject::getSignalSourceObject", "There is no source on your signal chain so most probaly you will get no sound " + this->getName());
	return nullptr;
}
//--------------------------------------------------------------
ofxSoundObject* ofxSoundObject::getSignalDestinationObject(){
	if(type == OFX_SOUND_OBJECT_DESTINATION)return this; // it is a destination object like an ofSoundOutput

	if(outputObjectRef == nullptr) return this; // it is at the end of the signal chain, so it most probably is the output.
	
	return outputObjectRef->getSignalDestinationObject();
	
	
	//ofLogWarning("ofxSoundObject::getSignalDestinationObject", "There is no destination on your signal chain so most probaly you will get no sound");
	return nullptr;
}
//--------------------------------------------------------------
// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioOut(ofSoundBuffer &output) {
    _tickCount = output.getTickCount();
    printAudioOut();
    ofxSoundUtils::checkBuffers(output, workingBuffer);
	if(inputObject!=nullptr) {
        if(isBypassed()){
            inputObject->audioOut(output);
        }else{
            inputObject->audioOut(workingBuffer);
        }
	}
    if(!isBypassed()){
        this->process(workingBuffer, output);
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
int ofxSoundObject::getDeviceId(){
	if(getOutputStream()){
		auto ss = getOutputStream()->getSoundStream();
		if(ss) return ss->getOutDevice().deviceID;
	}
	return getBuffer().getDeviceID();
}
//--------------------------------------------------------------
ofSoundDevice ofxSoundObject::getDeviceInfo(){
	return ofxSoundUtils::getSoundDeviceInfo(this->getDeviceId());
}
//--------------------------------------------------------------
const std::string& ofxSoundObject::getName(){
	return objectName;
}
//--------------------------------------------------------------
void ofxSoundObject::setName(const std::string& name){
	objectName = name;
}

//--------------------------------------------------------------
bool ofxSoundObject::isBypassed(){
//#ifdef OFX_SOUND_OBJECT_USE_ATOMICS
//    return _bBypass.load();
//#else
    return _bBypass;
    
}
//--------------------------------------------------------------
void ofxSoundObject::setBypassed(bool bypassed){
    _bBypass = bypassed;
}

//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
ofxSoundInput::ofxSoundInput():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
    setName("Sound Input");
}
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
//    ofxSoundUtils::checkBuffers(input, inputBuffer);
//	input.copyTo(inputBuffer);
    inputBuffer = input;
}
//--------------------------------------------------------------
void ofxSoundInput::audioOut(ofSoundBuffer &output) {
    _tickCount = output.getTickCount();
//    ofxSoundUtils::checkBuffers(output, inputBuffer);
    //the copyTo function will adapt the number of channels of inputBuffer into output's channel count.
    //If inputBuffer has more channels than output, it will only copy the amount from output. If output has more channels than inputBuffer then chnnels will be duplicated/
	inputBuffer.copyTo(output);
}
//--------------------------------------------------------------
int ofxSoundInput::getDeviceId(){
	if(getInputStream()){
		auto ss = getInputStream()->getSoundStream();
		if(ss) return ss->getInDevice().deviceID;
	}
	return inputBuffer.getDeviceID();
}

//--------------------------------------------------------------
void ofxSoundInput::setInputStream(ofSoundStream& stream){
	setInputStream(&stream);
}
//--------------------------------------------------------------
void ofxSoundInput::setInputStream(ofSoundStream* stream){
	if(stream != nullptr){
		inputStream = stream;
		inputStream->setInput(this);
	}
}
//--------------------------------------------------------------
ofSoundStream* ofxSoundInput::getInputStream(){
	return inputStream;
}


//--------------------------------------------------------------
//  ofxSoundOutput
//--------------------------------------------------------------
ofxSoundOutput::ofxSoundOutput():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION) {
    setName("Sound Output");
}

//--------------------------------------------------------------
