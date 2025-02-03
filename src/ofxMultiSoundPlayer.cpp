/*
 * ofxMultiSoundPlayer.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxMultiSoundPlayer.h"
#include <algorithm>
#include <float.h>
#include "ofxSoundUtils.h"


//--------------------------------------------------------------
ofxMultiSoundPlayer::ofxMultiSoundPlayer() {
	
	bMultiplay = false;
	bIsPlayingAny = false;
	
	setName ("Player");
	setNumInstances(0);

	volume.set("Volume", 1, 0, 1);
    volListener = volume.newListener(this, &ofxMultiSoundPlayer::volumeChanged);
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::canPlayInstance(){
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
bool ofxMultiSoundPlayer::loadAsync(std::filesystem::path filePath, bool bAutoplay){
    
	setNumInstances(1);
	
    bool ret =  instances[0]->loadAsync(filePath, bAutoplay);
    ofLogVerbose("ofxMultiSoundPlayer::loadAsync" ) << filePath << " success: " << std::boolalpha << ret;
    return ret;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::load(std::filesystem::path filePath, bool _stream){
	setNumInstances(1);
    bool ret = instances[0]->load(filePath, _stream);
    ofLogVerbose("ofxMultiSoundPlayer::load" ) << filePath << " success: " << std::boolalpha << ret;
    return ret;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::load(const ofSoundBuffer& loadBuffer, const std::string& name){
	setNumInstances(1);
	return instances[0]->load(loadBuffer, name);
}


//--------------------------------------------------------------
void ofxMultiSoundPlayer::unload(){
	bIsPlayingAny = false;
	setNumInstances(0);
}
//--------------------------------------------------------------
int ofxMultiSoundPlayer::play() {
	if(!isLoaded()) return -1;
	
	size_t index = 0;
	
		if (bMultiplay){
			bool bFound = false;
			{
				std::lock_guard<std::mutex> lock(instacesMutex);
				for (size_t i = 0; i < instances.size(); i++) {
					if (!(instances[i]->isPlaying()) && !(instances[i]->isReplaying())) {
						index = i;
						bFound = true;
						break;
					}
				}
			}
			if(!bFound){
				setNumInstances(instances.size() + 1);
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

		setPaused(false, index);
		return index;
	
	
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::stop(size_t index){
	setPaused(true, index);
	setNumInstances(1);
	setPosition(0);
}

//--------------------------------------------------------------
void ofxMultiSoundPlayer::drawDebug(float x, float y){
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
void ofxMultiSoundPlayer::audioOut(ofSoundBuffer& outputBuffer){
    _tickCount = outputBuffer.getTickCount();
	if(isLoaded() && bIsPlayingAny){
		_mixer.audioOut(outputBuffer);
	}else{
		outputBuffer.set(0);//if not playing clear the passed buffer, because it might contain junk data
	}
}


//========================END RUNNING ON AUDIO THREAD===============================
//========================SETTERS===============================
void ofxMultiSoundPlayer::setVolume(float vol, int index){
//	if(index <= -1){
		volume = vol;
//	}else{
		updateInstance([&](ofxSingleSoundPlayer* inst){
			inst->volume = vol;
		},index, "ofxMultiSoundPlayer::setVolume");
//	}
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setPan(float _pan, int index){
	updateInstance([&](ofxSingleSoundPlayer* inst){
		inst->setPan(_pan);
	},index, "ofxMultiSoundPlayer::setPan");
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setSpeed(float spd, int index){
	updateInstance([&](ofxSingleSoundPlayer* inst){
		inst->setSpeed(spd);
	},index, "ofxMultiSoundPlayer::setSpeed");
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setPaused(bool bP, int index){
	updateInstance([&](ofxSingleSoundPlayer* inst){
		instances[index]->setPaused(bP);
	},index, "ofxMultiSoundPlayer::setPaused");
	checkPaused();
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setLoop(bool bLp, int index){
	if(index == -1)bDefaultlLooping = bLp;
	updateInstance([&](ofxSingleSoundPlayer* inst){
		inst->setLoop(bLp);
	},index, "ofxMultiSoundPlayer::setLoop");
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setMultiPlay(bool bMp){
	bMultiplay = bMp;
	if(!bMultiplay){
		setNumInstances(1);
	}
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setPosition(float pct, size_t index){
	pct = ofClamp(pct, 0, 1);
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		instances[index]->setPosition(pct);
	}
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setPositionMS(int ms, size_t index){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		instances[index]->setPositionMS(ms);
	}
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::setNumInstances(const size_t & num){
	std::lock_guard<std::mutex> lock(instacesMutex);
	auto prevSize  = instances.size();
	/// in case that that the numbers of instaces to be set are less than the current number
	/// the instances will get automatically disconnected from the mixer upon their destruction.
	instances.resize(num);
	for(size_t i = prevSize; i < instances.size(); i++){
		instances[i] = std::make_unique<ofxSingleSoundPlayer>();
		instances[i]->setId(i);
		instances[i]->setLoop(bDefaultlLooping);
		instances[i]->connectTo(_mixer);
		if(prevSize > 0){
			if(instances[0]->getSharedSoundFile()){
				instances[i]->load(instances[0]->getSharedSoundFile());
			}else{
				instances[i]->load(instances[0]->getBuffer(), getName() + "_" + ofToString(i));
			}
		}
		endEventListeners.push(instances[i]->endEvent.newListener(this, &ofxMultiSoundPlayer::onInstanceEnd));
	}
	if(prevSize > num){
		endEventListeners.unsubscribeAll();
		for(size_t i = 0; i < instances.size(); i++){
			endEventListeners.push(instances[i]->endEvent.newListener(this, &ofxMultiSoundPlayer::onInstanceEnd));
		}
	}
	
}
void ofxMultiSoundPlayer::onInstanceEnd(size_t& i){
	checkPaused();
	ofNotifyEvent(endEvent, i, this);
}
//========================GETTERS===============================
//--------------------------------------------------------------
float ofxMultiSoundPlayer::getPosition(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPosition();
	}
	return 0;
}
//--------------------------------------------------------------
int ofxMultiSoundPlayer::getPositionMS(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPositionMS();
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::isPlaying(int index) const {
	if(!isLoaded()) return false;

	
	if (index <= -1){
		if(bIsPlayingAny) return true;
//		for (auto& i : instances) {
//			if (i->isPlaying())return true;
//		}
	}else{
		std::lock_guard<std::mutex> lock(instacesMutex);
		if(index < instances.size()){
			return instances[index]->isPlaying();
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::isLooping(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index == -1) return bDefaultlLooping;
	if(index < instances.size()){
		return instances[index]->isLooping();
	}
	return false;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::getIsLooping(size_t index) const{
	return isLooping(index);
}
//--------------------------------------------------------------
float ofxMultiSoundPlayer::getRelativeSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getRelativeSpeed();
	}
	return 0;
}
//--------------------------------------------------------------
float ofxMultiSoundPlayer::getSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getSpeed();
	}
	return 0;
}
//--------------------------------------------------------------
float ofxMultiSoundPlayer::getPan(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index]->getPan();
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxMultiSoundPlayer::isLoaded() const{
	if(instances.size() > 0 && instances[0]) return instances[0]->isLoaded();
	return false;
}
//--------------------------------------------------------------
float ofxMultiSoundPlayer::getVolume(int index) const{
	if(index <= -1){
		return volume.get();
	}else if(index < instances.size()){
		std::lock_guard<std::mutex> lock(instacesMutex);
		return instances[index]->volume;
	}
	return 0;	
}
//--------------------------------------------------------------
unsigned long ofxMultiSoundPlayer::getDurationMS(){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(instances.size() > 0){
		return instances[0]->getDurationMS();
	}
	return 0;
}
//--------------------------------------------------------------
const ofSoundBuffer & ofxMultiSoundPlayer::getBuffer() const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(instances.size() > 0){
		return instances[0]->getBuffer();
	}
	return ofxSingleSoundPlayer::_dummyBuffer;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxMultiSoundPlayer::checkPaused(){
	bIsPlayingAny = false;
	std::lock_guard<std::mutex> lock(instacesMutex);
	for (auto& i : instances) {
		if (i->isPlaying() || i->isReplaying()) {
			bIsPlayingAny = true;
			break;
		}
	}
}
//--------------------------------------------------------------
void ofxMultiSoundPlayer::updateInstance(std::function<void(ofxSingleSoundPlayer* inst)> func, int index, string methodName) {
	
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
size_t ofxMultiSoundPlayer::getNumChannels() {
	if(isLoaded()) return instances[0]->getNumChannels();
	return 0;
}

//--------------------------------------------------------------
ofEvent<void>& ofxMultiSoundPlayer::getAsyncLoadEndEvent(){
	return getSoundFile().loadAsyncEndEvent;
}

//--------------------------------------------------------------
std::string ofxMultiSoundPlayer::getFilePath() const{
	return getSoundFile().getPath();
}

const ofxSoundFile& ofxMultiSoundPlayer::getSoundFile() const{
	
	if(instances.size() == 0){
		return ofxSingleSoundPlayer::_getDummySoundFile();
	}
	return instances[0]->getSoundFile();
}

ofxSoundFile& ofxMultiSoundPlayer::getSoundFile(){
	if(instances.size() == 0){
		setNumInstances(1);
	}
	return instances[0]->getSoundFile();
}

ofxSingleSoundPlayer& ofxMultiSoundPlayer::getPlayInstance(size_t index){
	return *instances[index];
	
}
const ofxSingleSoundPlayer& ofxMultiSoundPlayer::getPlayInstance(size_t index) const{
	return *instances[index];
}
size_t ofxMultiSoundPlayer::getNumPlayInstances() const{
	return instances.size();
}

bool ofxMultiSoundPlayer::isReplaying() const{
	for(auto& i: instances){
		if(i->isReplaying()){
			return true;
		}
	}
	return false;
}

int ofxMultiSoundPlayer::getSourceSampleRate() const {
	if(instances.size()){
		return instances[0]->getSourceSampleRate();
	}
	return 0;
}

size_t ofxMultiSoundPlayer::getNumFrames() const {
	if(instances.size()){
		return instances[0]->getNumFrames();
	}
	return 0;
}


void ofxMultiSoundPlayer::volumeChanged(float&){
    if(bVolumeChangedByParameter)return;
    bVolumeChangedByParameter = true;
    setVolume(volume.get());
    
    bVolumeChangedByParameter = false;
}

