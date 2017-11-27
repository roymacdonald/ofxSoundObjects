/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundObject.h"
#include "ofxSoundObjectsUtils.h"
//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
ofxSoundObject::ofxSoundObject() {
	inputObject = NULL;
    outputObjectRef = NULL;
}
//--------------------------------------------------------------
ofxSoundObject &ofxSoundObject::connectTo(ofxSoundObject &soundObject) {
    if (outputObjectRef != NULL) {
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
    if (inputObject != NULL) {
    inputObject = NULL;
    }
}
//--------------------------------------------------------------
void ofxSoundObject::disconnect(){
    outputObjectRef->disconnectInput(this);
    outputObjectRef =NULL;
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

	// move up the dsp chain until we find ourselves or the beginning of the chain (input==NULL)
	while(prev!=this && prev!=NULL) {
		prev = prev->getInputObject();
	}

	// if we found ourselves, return false (to indicate there's an infinite loop)
	return (prev==NULL);
}
//--------------------------------------------------------------
// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioOut(ofSoundBuffer &output) {
    ofxSoundObjects::checkBuffers(output, workingBuffer);
	if(inputObject!=NULL) {
		inputObject->audioOut(workingBuffer);
	}
	this->process(workingBuffer, output);
}
////--------------------------------------------------------------
//void ofxSoundObject::setNumChannels(int num){
//    numChannels = num;
//}
////--------------------------------------------------------------
//int ofxSoundObject::getNumChannels() const{
//    return numChannels;
//}
//--------------------------------------------------------------
ofSoundBuffer& ofxSoundObject::getBuffer(){
return workingBuffer;
}
//--------------------------------------------------------------
const ofSoundBuffer& ofxSoundObject::getBuffer() const{
return workingBuffer;    
}
//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
ofxSoundInput::ofxSoundInput() {
}
//--------------------------------------------------------------
// copy audio in to internal buffer
void ofxSoundInput::audioIn(ofSoundBuffer &input) {
    ofxSoundObjects::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
}
//--------------------------------------------------------------
void ofxSoundInput::audioOut(ofSoundBuffer &output) {
    ofxSoundObjects::checkBuffers(output, inputBuffer);
	inputBuffer.copyTo(output);
}
