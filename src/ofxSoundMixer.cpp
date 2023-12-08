/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundMixer.h"
#include "ofxSoundUtils.h"
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
    std::lock_guard<std::mutex> lck(connectionMutex);
	connections.clear();
	connectionVolume.clear();
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMixer::getConnectionSource(size_t connectionNumber){
    ofxSoundObject* src = nullptr;
    {
        std::lock_guard<std::mutex> lck(connectionMutex);
        if (connectionNumber < connections.size()) {
            src = connections[connectionNumber];
        }
    }
    return src;
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMixer::getChannelSource(int channelNumber){
	return getConnectionSource(channelNumber);
}
//----------------------------------------------------
void ofxSoundMixer::disconnectInput(ofxSoundObject * input){
    std::lock_guard<std::mutex> lck(connectionMutex);
    
    for (size_t i =0; i<connections.size(); i++) {
        if (input == connections[i]) {
            connections.erase(connections.begin() + i);
            connectionVolume.erase(connectionVolume.begin() + i);
            break;
        }
    }
}
//----------------------------------------------------
void ofxSoundMixer::setInput(ofxSoundObject *obj){
    std::lock_guard<std::mutex> lck(connectionMutex);
    for (size_t i =0; i<connections.size(); i++) {
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
    std::lock_guard<std::mutex> lck(connectionMutex);
	return connections.size();
}
size_t ofxSoundMixer::getNumConnections(){
    std::lock_guard<std::mutex> lck(connectionMutex);
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
bool ofxSoundMixer::getObjectConnectionIndex(ofxSoundObject& obj, size_t& index){
    std::lock_guard<std::mutex> lck(connectionMutex);
    for (size_t i =0; i<connections.size(); i++) {
        if (&obj == connections[i]) {
            index = i;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------
bool ofxSoundMixer::isConnectedTo(ofxSoundObject& obj){
    std::lock_guard<std::mutex> lck(connectionMutex);
    for (size_t i =0; i<connections.size(); i++) {
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
void  ofxSoundMixer::setConnectionVolume(size_t connectionIndex, float vol){
    std::lock_guard<std::mutex> lck(connectionMutex);
	if (connectionIndex < connectionVolume.size()) {
		connectionVolume[connectionIndex] = vol;
	}
}
//----------------------------------------------------
float ofxSoundMixer::getChannelVolume(int channelNumber){
	return getConnectionVolume(channelNumber);
}
//----------------------------------------------------
float ofxSoundMixer::getConnectionVolume(size_t connectionIndex){
    std::lock_guard<std::mutex> lck(connectionMutex);
    if (connectionIndex < connectionVolume.size()) {
        return connectionVolume[connectionIndex];
    }
    return 0;
}

//----------------------------------------------------
// this pulls the audio through from earlier links in the chain and sums up the total output
void ofxSoundMixer::audioOut(ofSoundBuffer &output) {
    if(connections.size()>0) {
		output.set(0);//clears the output buffer as its memory might come with junk
		ofSoundBuffer tempBuffer;
        ofxSoundUtils::checkBuffers(output, tempBuffer,true);
        
        float v;
        //create a temporary vector to avoid threading issues.
        vector<ofxSoundObject*> tempConnections ;
        {
            std::lock_guard<std::mutex> lck(connectionMutex);
            tempConnections = connections;
        }
        
        for(int i = 0; i < tempConnections.size(); i++){
            {
                std::lock_guard<std::mutex> lck(connectionMutex);
                v = connectionVolume[i];
            }
            
            if (tempConnections[i] != nullptr && v > 0) {
				tempBuffer.set(0);
                tempConnections[i]->audioOut(tempBuffer);

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
