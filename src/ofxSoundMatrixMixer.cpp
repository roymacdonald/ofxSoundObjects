//
//  ofxSoundMatrixMixer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 4/28/19.
//
//

#include "ofxSoundMatrixMixer.h"

//----------------------------------------------------
ofxSoundMatrixMixer::ofxSoundMatrixMixer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	masterVolume = 1.0f;	
	masterVol.set("Master Vol", 1, 0, 1);
	masterVol.addListener(this, &ofxSoundMatrixMixer::masterVolChanged);
}
//----------------------------------------------------
void ofxSoundMatrixMixer::masterVolChanged(float& f) {
	mutex.lock();
	masterVolume = masterVol;
	mutex.unlock();
}
//----------------------------------------------------
ofxSoundMatrixMixer::~ofxSoundMatrixMixer(){
	masterVol.removeListener(this, &ofxSoundMatrixMixer::masterVolChanged);
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMatrixMixer::getInputObject(size_t objectNumber){
	if (objectNumber < inObjects.size()) {
		return inObjects[objectNumber].obj;
	}else{
		return nullptr;
	}
}
//----------------------------------------------------
size_t ofxSoundMatrixMixer::getNumInputChannels(){
}
//----------------------------------------------------
size_t ofxSoundMatrixMixer::getNumInputObjects(){
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMatrixMixer::getInputChannelSource(size_t channelNumber){
	if (channelNumber < inObjects.size()) {
		return inObjects[channelNumber].obj;
	}else{
		return nullptr;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::disconnectInput(ofxSoundObject * input){
	ofRemove(inObjects, [&](MatrixInputObject& o){return input == o.obj;});
//	for (int i =0; i<input.size(); i++) {
//		if (input == channels[i]) {
//			channels.erase(channels.begin() + i);
////			channelVolume.erase(channelVolume.begin() + i);
//			break;
//		}
//	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setInput(ofxSoundObject *obj){
	if(obj){
	for (int i =0; i<inObjects.size(); i++) {
		if (obj == inObjects[i].obj) {
			ofLogNotice("ofxSoundMatrixMixer::setInput") << " already connected" ;
			return;
		}
	}
	inObjects.push_back(MatrixInputObject(obj, numOutputChannels));
	updateNumInputChannels();
//		for(auto & i : inObjects){
			
//			i.resize(numInputChannels);
//			inObjectsVolumes.push_back(std::vector<float>( numInputChannels, 0.0f));
//		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumOutputChannels(const size_t & nc){
	if(numOutputChannels != nc){
		numOutputChannels = nc;
		for(auto& i : inObjects){
			i.updateChanVolsSize(numOutputChannels);
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumInputChannels(){

	numInputChannels = 0;
	
	for(auto& i : inObjects){
		numInputChannels += i.obj->getBuffer().getNumChannels();
	}
	
	
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setMasterVolume(float vol){
	mutex.lock();
	masterVolume = vol;
	mutex.unlock();
}
//----------------------------------------------------
float ofxSoundMatrixMixer::getMasterVolume(){
	return masterVolume;
}
//----------------------------------------------------
bool ofxSoundMatrixMixer::isConnected(ofxSoundObject& obj){
	for (int i =0; i<inObjects.size(); i++) {
		if (&obj == inObjects[i].obj) {
			return true;
		}
	}
	return false;
}
//----------------------------------------------------
// this pulls the audio through from earlier links in the chain and sums up the total output
void ofxSoundMatrixMixer::audioOut(ofSoundBuffer &output) {
	updateNumOutputChannels(output.getNumChannels());
	if(inObjects.size()>0) {
		for(int i = 0; i < inObjects.size(); i++){
			if (inObjects[i].obj != NULL ) {
				ofSoundBuffer tempBuffer;
				tempBuffer.resize(output.size());
//				tempBuffer.setNumChannels(inObjects[i].obj);
				tempBuffer.setSampleRate(output.getSampleRate());
				inObjects[i].obj->audioOut(tempBuffer);//could this be done on different threads? maybe threadChannels?
				
				for(auto& c : inObjects[i].channelsVolumes){
					
				}
				
//				float v = channelVolume[i];
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
//	numOutputChannels = output.getNumChannels();
}
//----------------------------------------------------
