/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundMixer.h"
//----------------------------------------------------
ofxSoundMixer::ofxSoundMixer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	chanMod = OFX_SOUND_OBJECT_CHAN_MIXER;
    masterVolume = 1.0f;
    masterPan = 0.5f;
	
	setName ("Sound Mixer");
	
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
	connections.clear();
	connectionVolume.clear();
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMixer::getConnectionSource(int connectionNumber){
	if (connectionNumber < connections.size()) {
		return connections[connectionNumber];
	}else{
		return nullptr;
	}
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMixer::getChannelSource(int channelNumber){
	return getConnectionSource(channelNumber);
}
//----------------------------------------------------
void ofxSoundMixer::disconnectInput(ofxSoundObject * input){
    for (int i =0; i<connections.size(); i++) {
        if (input == connections[i]) {
            connections.erase(connections.begin() + i);
            connectionVolume.erase(connectionVolume.begin() + i);
            break;
        }
    }
}
//----------------------------------------------------
void ofxSoundMixer::setInput(ofxSoundObject *obj){
    for (int i =0; i<connections.size(); i++) {
        if (obj == connections[i]) {
            ofLogNotice("ofxSoundMixer::setInput") << " already connected" << endl;
            return;
        }
    }
    connections.push_back(obj);
    connectionVolume.push_back(1);//default volume for channel is 1
}
//----------------------------------------------------
size_t ofxSoundMixer::getNumChannels(){
	return connections.size();
}
size_t ofxSoundMixer::getNumConnections(){
    return connections.size();
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
    for (int i =0; i<connections.size(); i++) {
        if (&obj == connections[i]) {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------
void ofxSoundMixer::setChannelVolume(int channelNumber, float vol){
	setConnectionVolume(channelNumber, vol);
}
//----------------------------------------------------
void  ofxSoundMixer::setConnectionVolume(int channelNumber, float vol){
	if (channelNumber < connectionVolume.size()) {
		connectionVolume[channelNumber] = vol;
	}
}
//----------------------------------------------------
float ofxSoundMixer::getChannelVolume(int channelNumber){
	return getConnectionVolume(channelNumber);
}
//----------------------------------------------------
float ofxSoundMixer::getConnectionVolume(int channelNumber){
    if (channelNumber < connectionVolume.size()) {
        return connectionVolume[channelNumber];
    }
    return 0;
}

//----------------------------------------------------
// this pulls the audio through from earlier links in the chain and sums up the total output
void ofxSoundMixer::audioOut(ofSoundBuffer &output) {
    if(connections.size()>0) {
		output.set(0);//clears the output buffer as its memory might come with junk
        for(int i = 0; i < connections.size(); i++){
            if (connections[i] != nullptr && connectionVolume[i] > 0) {
                ofSoundBuffer tempBuffer;
                tempBuffer.resize(output.size());
                tempBuffer.setNumChannels(output.getNumChannels());
                tempBuffer.setSampleRate(output.getSampleRate());
                connections[i]->audioOut(tempBuffer);
                
                float v = connectionVolume[i];
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
