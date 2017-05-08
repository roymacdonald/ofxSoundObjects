/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundObject.h"

//----------------------------------------------------
// ofxSoundObject

ofxSoundObject::ofxSoundObject() {
	inputObject = NULL;
    outputObjectRef = NULL;
}


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

void ofxSoundObject::disconnectInput(ofxSoundObject * input){
    if (inputObject != NULL) {
    inputObject = NULL;
    }
}

void ofxSoundObject::disconnect(){
    outputObjectRef->disconnectInput(this);
    outputObjectRef =NULL;
}

void ofxSoundObject::setInput(ofxSoundObject *obj) {
	inputObject = obj;
}

ofxSoundObject *ofxSoundObject::getInputObject() {
	return inputObject;
}

bool ofxSoundObject::checkForInfiniteLoops() {
	ofxSoundObject *prev = inputObject;

	// move up the dsp chain until we find ourselves or the beginning of the chain (input==NULL)
	while(prev!=this && prev!=NULL) {
		prev = prev->getInputObject();
	}

	// if we found ourselves, return false (to indicate there's an infinite loop)
	return (prev==NULL);
}

// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioOut(ofSoundBuffer &output) {
	if(inputObject!=NULL) {
		if(workingBuffer.size()!=output.size()) {
            ofLogVerbose("ofxSoundObject") << "working buffer size != output buffer size. " << workingBuffer.size() << " != " <<output.size();
            workingBuffer.resize(output.size());
            workingBuffer.setNumChannels(output.getNumChannels());
            workingBuffer.setSampleRate(output.getSampleRate());
		}

		inputObject->audioOut(workingBuffer);
	}
	this->process(workingBuffer, output);
}

//----------------------------------------------------
// ofSoundInput

ofxSoundInput::ofxSoundInput() {

}

// copy audio in to internal buffer
void ofxSoundInput::audioIn(ofSoundBuffer &input) {
	if(inputBuffer.size()!=input.size()) {
        ofLogVerbose("ofSoundinput::audioIn") << "input buffer size != output buffer size.";
		inputBuffer.resize(input.size());
        inputBuffer.setNumChannels(input.getNumChannels());
        inputBuffer.setSampleRate(input.getSampleRate());
	}
	input.copyTo(inputBuffer);
}

void ofxSoundInput::audioOut(ofSoundBuffer &output) {
    if(output.getNumFrames()==inputBuffer.getNumFrames()){
        ofLogVerbose("ofSoundinput::audioOut") << "input buffer size != output buffer size.";
        inputBuffer.resize(output.size());
        inputBuffer.setNumChannels(output.getNumChannels());
        inputBuffer.setSampleRate(output.getSampleRate());
	}
	inputBuffer.copyTo(output);
}


