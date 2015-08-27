/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundMixer.h"
//----------------------------------------------------
// ofxSoundMixer

ofxSoundMixer::ofxSoundMixer(){
	masterVolume = 1.0f;
	masterPan = 0.5f;
}

ofxSoundMixer::~ofxSoundMixer(){}

ofxSoundMixer &ofGetSystemSoundMixer(){
	static ofxSoundMixer systemSoundMixer;
    return systemSoundMixer;
}

shared_ptr<ofBaseSoundOutput> ofxSoundMixer::getChannelSource(int channelNumber){
    if (channelNumber < channels.size()) {
        return shared_ptr<ofBaseSoundOutput>(channels[channelNumber]);
    }else{
        return shared_ptr<ofBaseSoundOutput>();
    }
}

void ofxSoundMixer::disconnectInput(ofxSoundObject * input){
    for (int i =0; i<channels.size(); i++) {
        if (input == channels[i]) {
            channels.erase(channels.begin() + i);
            channelVolume.erase(channelVolume.begin() + i);
            break;
        }
    }
}

void ofxSoundMixer::setInput(ofxSoundObject *obj){
    for (int i =0; i<channels.size(); i++) {
        if (obj == channels[i]) {
            ofLogNotice("ofxSoundMixer::setInput") << " already connected" << endl;
            return;
        }
    }
    channels.push_back(obj);
    channelVolume.push_back(1);//default volume for channel is 1
}

int ofxSoundMixer::getNumChannels(){
    return channels.size();
}

void ofxSoundMixer::setMasterVolume(float vol){
    masterVolume = vol;
}

float ofxSoundMixer::getMasterVolume(){
    return masterVolume;
}

float ofxSoundMixer::getMasterPan(){
    return masterPan;
}

void ofxSoundMixer::setMasterPan(float pan){
    masterPan = pan;
}

bool ofxSoundMixer::isConnectedTo(ofxSoundObject& obj){
    for (int i =0; i<channels.size(); i++) {
        if (&obj == channels[i]) {
            return true;
        }
    }
    return false;
}

void ofxSoundMixer::setChannelVolume(int channelNumber, float vol){
 if (channelNumber < channelVolume.size()) {
     channelVolume[channelNumber] = vol;
 }
}

float ofxSoundMixer::getChannelVolume(int channelNumber){
    if (channelNumber < channelVolume.size()) {
        return channelVolume[channelNumber];
    }
    return 0;
}


// this pulls the audio through from earlier links in the chain and sums up the total output
void ofxSoundMixer::audioOut(ofSoundBuffer &output) {
	if(channels.size()>0) {
        for(int i = 0; i < channels.size(); i++){
            if (channels[i] != NULL && channelVolume[i] > 0) {
                ofSoundBuffer tempBuffer;
                tempBuffer.resize(output.size());
                tempBuffer.setNumChannels(output.getNumChannels());
                tempBuffer.setSampleRate(output.getSampleRate());
                channels[i]->audioOut(tempBuffer);

                float v = channelVolume[i];
                for (int i = 0; i < tempBuffer.size(); i++) {
                    output.getBuffer()[i] += tempBuffer.getBuffer()[i] * v;
                }
            }
		}
		if(output.getNumChannels() == 2) {
			output.stereoPan(1-masterPan, masterPan);
		}
        output*=masterVolume;
	}
}
