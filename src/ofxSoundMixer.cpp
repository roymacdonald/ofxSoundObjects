/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundMixer.h"
//----------------------------------------------------
ofxSoundMixer::ofxSoundMixer(){
    masterVolume = 1.0f;
    masterPan = 0.5f;
	masterVol.set("Master Vol", 1, 0, 1);
	masterVol.addListener(this, &ofxSoundMixer::masterVolChanged);
}
//----------------------------------------------------
void ofxSoundMixer::masterVolChanged(float& f) {
	mutex.lock();
	masterVolume = masterVol;
	mutex.unlock();
}
//----------------------------------------------------
ofxSoundMixer::~ofxSoundMixer(){
	channels.clear();
	channelVolume.clear();
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMixer::getChannelSource(int channelNumber){
    if (channelNumber < channels.size()) {
        return channels[channelNumber];
    }else{
        return nullptr;
    }
}
//----------------------------------------------------
void ofxSoundMixer::disconnectInput(ofxSoundObject * input){
    for (int i =0; i<channels.size(); i++) {
        if (input == channels[i]) {
            channels.erase(channels.begin() + i);
            channelVolume.erase(channelVolume.begin() + i);
            break;
        }
    }
}
//----------------------------------------------------
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
//----------------------------------------------------
int ofxSoundMixer::getNumChannels(){
    return channels.size();
}
//----------------------------------------------------
void ofxSoundMixer::setMasterVolume(float vol){
	mutex.lock();
    masterVolume = vol;
	mutex.unlock();
}
//----------------------------------------------------
float ofxSoundMixer::getMasterVolume(){
    return masterVolume;
}
//----------------------------------------------------
float ofxSoundMixer::getMasterPan(){
    return masterPan;
}
//----------------------------------------------------
void ofxSoundMixer::setMasterPan(float pan){
	mutex.lock();
    masterPan = pan;
	mutex.unlock();
}
//----------------------------------------------------
bool ofxSoundMixer::isConnectedTo(ofxSoundObject& obj){
    for (int i =0; i<channels.size(); i++) {
        if (&obj == channels[i]) {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------
void ofxSoundMixer::setChannelVolume(int channelNumber, float vol){
    if (channelNumber < channelVolume.size()) {
        channelVolume[channelNumber] = vol;
    }
}
//----------------------------------------------------
float ofxSoundMixer::getChannelVolume(int channelNumber){
    if (channelNumber < channelVolume.size()) {
        return channelVolume[channelNumber];
    }
    return 0;
}

//----------------------------------------------------
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
                for (int j = 0; j < tempBuffer.size(); j++) {
                    output.getBuffer()[j] += tempBuffer.getBuffer()[j] * v;
                }
            }
        }
     /*   if(output.getNumChannels() == 2) {
            output.stereoPan(1-masterPan, masterPan);
        }*/
        output*=masterVolume;
    }
}
//----------------------------------------------------