/*
 * ofxSoundPlayerObject.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxSoundPlayerObject.h"
#include <algorithm>
#include <float.h>
#include "ofxSoundUtils.h"
#include "ofxSimpleSoundPlayer.h"


//int ofxSoundPlayerObject::maxSoundsTotal=128;
//int ofxSoundPlayerObject::maxSoundsPerPlayer=16;

//--------------------------------------------------------------
ofxSoundPlayerObject::ofxSoundPlayerObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
	
	bMultiplay = false;
	bIsPlayingAny = false;
	
	setName ("Player");
	setNumInstances(0);

	volume.set("Volume", 1, 0, 1);
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::canPlayInstance(){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if (!instances.size()){// < maxSounds -1) {
		return false;
	}
	for(auto& i: instances){
		if(!i->isPlaying()){
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::loadAsync(std::filesystem::path filePath, bool bAutoplay){
	setNumInstances(1);
	return instances[0]->loadAsync(filePath, bAutoplay);
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::load(std::filesystem::path filePath, bool _stream){
	setNumInstances(1);
	return instances[0]->load(filePath, _stream);
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::load(const ofSoundBuffer& loadBuffer, const std::string& name){
	setNumInstances(1);
	return instances[0]->load(loadBuffer, name);
}


//--------------------------------------------------------------
void ofxSoundPlayerObject::unload(){
	bIsPlayingAny = false;
	setNumInstances(0);
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::play() {
	if(!isLoaded()) return -1;
	
	size_t index = 0;
	
		if (bMultiplay){
			bool bFound = false;
			{
				std::lock_guard<std::mutex> lock(instacesMutex);
				for (auto& i : instances) {
					if (!i->isPlaying()) {
						index = i->getId();
						bFound = true;
						break;
					}
				}
				if(!bFound){
					setNumInstances(instances.size() + 1);
//					instances.push_back(ofxSimpleSoundPlayer());
//					index =instances.size() - 1;
//					instances.back().setId(index);
//					instances.back().setLoop(bDefaultlLooping);
				}
			}
		} else {
			if (instances.size() == 0) {
				setNumInstances(1);
			}
			{std::lock_guard<std::mutex> lock(instacesMutex);
				instances[0]->setId(0);
			}
		}
		setPosition(0,index);//Should the position be set to zero here? I'm not sure.
		setSpeed(1, index);
		setPaused(false, index);
		return index;
	
	
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::stop(size_t index){
	setPaused(true, index);
	setNumInstances(1);
	setPosition(0);
}

//--------------------------------------------------------------
void ofxSoundPlayerObject::drawDebug(float x, float y){
	stringstream ss;
	if(isLoaded() && instances.size() > 0){
	
		ss << instances[0]->getSourceInfo() << "\n";
	
		ss << "INSTANCES" << endl;
	
		for (auto &i: instances) {
			ss << i->getPlaybackInfo();
		}
	}else{
		ss << "File not loaded" << endl;
	}
	
	ofDrawBitmapString(ss.str(), x, y);
}

//========================BEGIN RUNNING ON AUDIO THREAD===============================
//--------------------------------------------------------------


//--------------------------------------------------------------
void ofxSoundPlayerObject::audioOut(ofSoundBuffer& outputBuffer){
	if(isLoaded() && bIsPlayingAny){
		_mixer.audioOut(outputBuffer);
	}else{
		outputBuffer.set(0);//if not playing clear the passed buffer, because it might contain junk data
	}
}


//========================END RUNNING ON AUDIO THREAD===============================
//========================SETTERS===============================
void ofxSoundPlayerObject::setVolume(float vol, int index){
	if(index <= -1){
		volume = vol;
	}else{
		updateInstance([&](ofxSimpleSoundPlayer* inst){
			inst->volume = vol;
		},index, "ofxSoundPlayerObject::setVolume");
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPan(float _pan, int index){
	updateInstance([&](ofxSimpleSoundPlayer* inst){
		inst->setPan(_pan);
	},index, "ofxSoundPlayerObject::setPan");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setSpeed(float spd, int index){
	updateInstance([&](ofxSimpleSoundPlayer* inst){
		inst->setSpeed(spd);
	},index, "ofxSoundPlayerObject::setSpeed");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPaused(bool bP, int index){
	updateInstance([&](ofxSimpleSoundPlayer* inst){
		instances[index]->setPaused(bP);
	},index, "ofxSoundPlayerObject::setPaused");
	checkPaused();
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setLoop(bool bLp, int index){
	if(index == -1)bDefaultlLooping = bLp;
	updateInstance([&](ofxSimpleSoundPlayer* inst){
		inst->setLoop(bLp);
	},index, "ofxSoundPlayerObject::setLoop");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setMultiPlay(bool bMp){
	bMultiplay = bMp;
	if(!bMultiplay){
		setNumInstances(1);
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPosition(float pct, size_t index){
	pct = ofClamp(pct, 0, 1);
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		instances[index]->setPosition(pct);
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPositionMS(int ms, size_t index){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		instances[index]->setPositionMS(ms);
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setNumInstances(const size_t & num){
	std::lock_guard<std::mutex> lock(instacesMutex);
	auto prevSize  = instances.size();
	/// in case that that the numbers of instaces to be set are less than the current number
	/// the instances will get automatically disconnected from the mixer upon their destruction.
	instances.resize(num);
	for(size_t i = prevSize; i < instances.size(); i++){
		instances[i] = make_unique<ofxSimpleSoundPlayer>();
		instances[i]->setId(i);
		instances[i]->setLoop(bDefaultlLooping);
		instances[i]->connectTo(_mixer);
		if(prevSize > 0){
			if(instances[0]->getSharedSoundFile()){
				instances[i]->load(instances[0]->getSharedSoundFile());
			}else{
				instances[i]->load(instances[0]->getSharedBuffer(), getName() + "_" + ofToString(i));
			}
		}
	}
	
	
	
}
//========================GETTERS===============================
//size_t ofxSoundPlayerObject::_getNumFrames(size_t index) const{
//	auto& i = instances[index];
//	return (i.bUsePreprocessedBuffer?i.preprocessedBuffer.getNumFrames(): sourceNumFrames);
//}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getPosition(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPosition();
	}
	return 0;
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::getPositionMS(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPositionMS();
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isPlaying(int index) const {
	if(!isLoaded()) return false;
	std::lock_guard<std::mutex> lock(instacesMutex);
	if (index <= -1){
		for (auto& i : instances) {
			if (i->isPlaying())return true;
		}
	}else{
		if(index < instances.size()){
			return instances[index]->isPlaying();
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isLooping(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index == -1) return bDefaultlLooping;
	if(index < instances.size()){
		return instances[index]->getIsLooping();
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::getIsLooping(size_t index) const{
	return isLooping(index);
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getRelativeSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getRelativeSpeed();
	}
	return 0;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getSpeed();
	}
	return 0;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getPan(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPan();
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isLoaded() const{
	if(instances.size() > 0) return instances[0]->isLoaded();
	return false;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getVolume(int index) const{
	if(index <= -1){
		return volume.get();
	}else if(index < instances.size()){
		std::lock_guard<std::mutex> lock(instacesMutex);
		return instances[index]->volume;
	}
	return 0;	
}
//--------------------------------------------------------------
unsigned long ofxSoundPlayerObject::getDurationMS(){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(instances.size() > 0){
		return instances[0]->getDurationMS();
	}
	return 0;
}
//--------------------------------------------------------------
const ofSoundBuffer & ofxSoundPlayerObject::getBuffer() const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(instances.size() > 0){
		return instances[0]->getBuffer();
	}
	return ofxSimpleSoundPlayer::_dummyBuffer;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxSoundPlayerObject::checkPaused(){
	bIsPlayingAny = false;
	std::lock_guard<std::mutex> lock(instacesMutex);
	for (auto& i : instances) {
		if (i->isPlaying()) {
			bIsPlayingAny = true;
			break;
		}
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::updateInstance(std::function<void(ofxSimpleSoundPlayer* inst)> func, int index, string methodName) {
	
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index <= -1){
		for(auto& i: instances){
			func(i.get());
		}
	}else if(index < instances.size()){
		func(instances[index].get());
	}else{
		ofLogVerbose(methodName) << "index out of range" << endl;
	}
}
//--------------------------------------------------------------
size_t ofxSoundPlayerObject::getNumChannels() {
	if(isLoaded()) return instances[0]->getNumChannels();
	return 0;
}
//--------------------------------------------------------------
ofEvent<void>& ofxSoundPlayerObject::getAsyncLoadEndEvent(){
	return getSoundFile().loadAsyncEndEvent;
}
//--------------------------------------------------------------
const std::string ofxSoundPlayerObject::getFilePath() const{
	return getSoundFile().getPath();
}

const ofxSoundFile& ofxSoundPlayerObject::getSoundFile() const{
	
	if(instances.size() == 0){
		return ofxSimpleSoundPlayer::_getDummySoundFile();
	}
	return instances[0]->getSoundFile();
}

ofxSoundFile& ofxSoundPlayerObject::getSoundFile(){
	if(instances.size() == 0){
		setNumInstances(1);
	}
	return instances[0]->getSoundFile();
}
