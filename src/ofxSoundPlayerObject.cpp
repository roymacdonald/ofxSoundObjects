/*
 * ofxSoundPlayerObject.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxSoundPlayerObject.h"
#include <algorithm>
#include <float.h>

//int ofxSoundPlayerObject::maxSoundsTotal=128;
//int ofxSoundPlayerObject::maxSoundsPerPlayer=16;

//--------------------------------------------------------------
ofxSoundPlayerObject::ofxSoundPlayerObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
	bStreaming = false;
	bMultiplay = false;
	bIsPlayingAny = false;
	bListeningUpdate = false;
	
	setNumInstances(1);

	volume.set("Volume", 1, 0, 1);
	
	setState(UNLOADED);
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setState(State newState){
	state = newState;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isState(State compState){
	return state == compState;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::canPlayInstance(){
	std::lock_guard<std::mutex> lock(instacesMutex);
	if (!instances.size()){// < maxSounds -1) {
		return false;
	}
	for(auto& i: instances){
		if(!i.bIsPlaying){
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::loadAsync(std::filesystem::path filePath, bool bAutoplay){
	if(isLoaded())unload();	
	setState(bAutoplay?LOADING_ASYNC_AUTOPLAY:LOADING_ASYNC);
	bStreaming = false;
	soundFile.loadAsync(filePath.string());
	ofAddListener(soundFile.loadAsyncEndEvent, this, &ofxSoundPlayerObject::initFromSoundFile);
	return true;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::load(std::filesystem::path filePath, bool _stream){
	if(isLoaded())unload();
	if(_stream){
		if(soundFile.openFileStream(filePath.string())){
			bStreaming = true;
//			initFromSoundFile();
		}else{
			return false;
		}
	}else if(soundFile.load(filePath.string())){
		bStreaming = false;
	}else{
		return false;
	}
	
	initFromSoundFile();
	return isLoaded();
	
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::load(const ofSoundBuffer& loadBuffer, const std::string& name){
	if(isLoaded())unload();

	bStreaming = false;
	
	if(loadBuffer.size()){
		volume.setName(name);
		buffer = loadBuffer;
		setNumInstances(1);
		
		playerNumChannels = buffer.getNumChannels();
		playerSampleRate = buffer.getSampleRate();
		
		sourceNumFrames = buffer.getNumFrames();
		sourceNumChannels = buffer.getNumChannels();
		sourceSampleRate = buffer.getSampleRate();
		sourceDuration   = 1000* float(buffer.getNumFrames()) / float(buffer.getSampleRate());
		
		setState(LOADED);
		
	}
	
	
	return isLoaded();
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::initFromSoundFile(){
	
	bool bLoaded = soundFile.isLoaded();
	if(bLoaded){	
		stringstream ss;

		if(isState(LOADING_ASYNC) ||
		   isState(LOADING_ASYNC_AUTOPLAY)){ 
			ofRemoveListener(soundFile.loadAsyncEndEvent, this, &ofxSoundPlayerObject::initFromSoundFile);
		}
		setNumInstances(1);
		
		auto name = ofFilePath::getBaseName(soundFile.getPath());
		if(!bStreaming){
			
			if(ofGetLogLevel() == OF_LOG_VERBOSE){
				ss << "Not streaming; Reading whole file into memory!\n";
			}
			load(soundFile.getBuffer(), name);
		}else{
			volume.setName(name);
			setNumInstances(1);
			
			playerNumChannels = soundFile.getNumChannels();
			playerSampleRate = soundFile.getSampleRate();
			
			sourceNumFrames = soundFile.getNumFrames();
			sourceNumChannels = soundFile.getNumChannels();
			sourceSampleRate = soundFile.getSampleRate();
			sourceDuration   = 1000* float(soundFile.getNumFrames()) / float(soundFile.getSampleRate());
			
			setState(LOADED);
		
		}
		if(ofGetLogLevel() == OF_LOG_VERBOSE){
			ss << "Loading file : " << soundFile.getPath() << "\n";
			ss << "Duration     : " << sourceDuration << "\n";
			ss << "Channels     : " << sourceNumChannels << "\n";
			ss << "SampleRate   : " << sourceSampleRate << "\n";
			ss << "Num Frames   : " << sourceNumFrames << "\n";
		}

		
		if(isState(LOADING_ASYNC_AUTOPLAY)){
			setState(LOADED);
			play();
		}else{
			setState(LOADED);
		}

		ofLogVerbose("ofxSoundPlayerObject::initFromSoundFile\n") << ss.str();

			
	}
}


//--------------------------------------------------------------
void ofxSoundPlayerObject::unload(){
	bIsPlayingAny = false;
	setState(UNLOADED);
	{
		std::lock_guard<std::mutex> lock(mutex);
		buffer.clear();
		soundFile.reset();
	}
	setNumInstances(1);
	clearInstanceEndNotificationQueue();
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::play() {
	size_t index = 0;
	if (isLoaded()) {

		if (bMultiplay){
			bool bFound = false;
			{
				std::lock_guard<std::mutex> lock(instacesMutex);
				for (auto& i : instances) {
					if (!i.bIsPlaying) {
						index = i.id;
						bFound = true;
						break;
					}
				}
				if(!bFound){
					instances.push_back(soundPlayInstance());
					index =instances.size() - 1;
					instances.back().id = index;
				}
			}
		} else {
			if (instances.size() == 0) {
				setNumInstances(1);
			}
			setPosition(0,0);//Should the position be set to zero here? I'm not sure.
			{std::lock_guard<std::mutex> lock(instacesMutex);
				instances[0].id = 0;
			}
			setSpeed(1, 0);
		}
		setSpeed(1, index);
		setPaused(false, index);
		return index;
	}
	return -1;
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::stop(size_t index){
	setPaused(true, index);
	setNumInstances(1);
	setPosition(0);
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::clearInstanceEndNotificationQueue(){
	updateListener.unsubscribe();
	bListeningUpdate = false;
	std::lock_guard<std::mutex> lock(instanceEndQueueMutex);
	endedInstancesToNotify.clear();
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::addInstanceEndNotification(const size_t & id){
	{
		std::lock_guard<std::mutex> lock(instanceEndQueueMutex);
		endedInstancesToNotify.push_back(id);
	}
	if(!bListeningUpdate){
		updateListener = ofEvents().update.newListener(this, &ofxSoundPlayerObject::update);
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::update(ofEventArgs&){
	if(endedInstancesToNotify.size()){
		for(auto& i: endedInstancesToNotify){
			ofNotifyEvent(endEvent, i);
		}
		clearInstanceEndNotificationQueue();
	}
}

//--------------------------------------------------------------
void ofxSoundPlayerObject::drawDebug(float x, float y){
	stringstream ss;
	if(isLoaded()){
	ss << "Duration     : " << sourceDuration << endl;
	ss << "DurationMS   : " << getDurationMS() << endl;
	ss << "Channels     : " << sourceNumChannels << endl;
	ss << "SampleRate   : " << sourceSampleRate << endl;
	ss << "Num Samples  : " << sourceNumFrames << endl;
	
	
	ss << "INSTANCES" << endl;
	
	for (int i =0; i< instances.size(); i++) {
		ss << i << "------------------------" <<endl;
		
		ss << "    id: " << instances[i].id << endl;
		ss << "    Position: " << getPosition(i) << endl;
		ss << "    PositionMS: " << getPositionMS(i) << endl;
		ss << "    Playing: " << boolalpha << isPlaying(i) << endl;
		ss << "    Speed: " << getSpeed(i) << endl;
		ss << "    Rel. Speed: " << getRelativeSpeed(i) << endl;
		ss << "    Pan: " << getPan(i) << endl;
		ss << "    Volume: " << getVolume(i) << endl;
		ss << "    IsLooping: " << boolalpha << getIsLooping(i) << endl;
		
	}
	}else{
		ss << "File not loaded" << endl;
	}
	
	ofDrawBitmapString(ss.str(), x, y);
}

//========================BEGIN RUNNING ON AUDIO THREAD===============================
//--------------------------------------------------------------
void ofxSoundPlayerObject::audioOutBuffersChanged(int nFrames, int nChannels, int sampleRate){
	if(bStreaming){
		ofLogVerbose("ofxSoundPlayerObject::audioOutBuffersChanged") << "Resizing buffer ";
		buffer.resize(nFrames*nChannels,0);
	}
	playerNumFrames = nFrames;
	playerNumChannels = nChannels;
	playerSampleRate = sampleRate;
}

//--------------------------------------------------------------
void ofxSoundPlayerObject::audioOut(ofSoundBuffer& outputBuffer){
	if(isLoaded() && bIsPlayingAny){
		
		auto nFrames = outputBuffer.getNumFrames();
		auto nChannels = outputBuffer.getNumChannels();
		if (playerNumChannels != nChannels || playerNumFrames != nFrames || playerSampleRate != outputBuffer.getSampleRate()) {
			audioOutBuffersChanged(nFrames, nChannels, outputBuffer.getSampleRate());
		}
		float vol;
		{
			std::lock_guard<std::mutex> lock(volumeMutex);
			vol = volume.get();
		}
		if (instances.size() == 1){
			processBuffers(outputBuffer, instances[0], vol, nFrames, nChannels);
		}
		else {
			for(auto& inst : instances){
				processBuffers(resampledBuffer, inst, vol, nFrames, nChannels);
				resampledBuffer.addTo(outputBuffer, 0, inst.loop);
			}
		}
		updatePositions(nFrames);
		
		
	}else{
		outputBuffer.set(0);//if not playing clear the passed buffer, because it might contain junk data
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::processBuffers(ofSoundBuffer& buf, soundPlayInstance& i, const float& vol, const std::size_t& nFrames, const std::size_t& nChannels){
	//assert( resampledBuffer.getNumFrames() == bufferSize*relativeSpeed[i] );
	if(bStreaming) {
		soundFile.readTo(buf, nFrames, i.position, i.loop);
	}else{
		if (ofIsFloatEqual( i.speed,  1.0f)) {
			buffer.copyTo(buf, nFrames, nChannels, i.position, i.loop);
		}
		else {
			buffer.resampleTo(buf, i.position, nFrames, i.relativeSpeed, i.loop, ofSoundBuffer::Linear);
		}
	}
	
	if(buf.getNumChannels() == 2){
		buf.stereoPan(i.volumeLeft * vol,i.volumeRight * vol);
	}else{
		buf *= vol*i.volume;
	}

}
//--------------------------------------------------------------
void ofxSoundPlayerObject::updatePositions(int nFrames){
	if (isLoaded()) {
		size_t nf;
		if(bStreaming){
			nf = soundFile.getNumFrames();
		}else{
			nf = buffer.getNumFrames();
		}
		for (auto& i : instances){
			if(i.bIsPlaying){
				i.position += nFrames*i.relativeSpeed;
				if (i.loop) {
					i.position %= nf;
				} else {
					i.position = ofClamp(i.position, 0, nf-1);
					if (i.position == nf-1) {	// finished?
						i.bIsPlaying = false;
						addInstanceEndNotification(i.id);
					}
				}
			}
		}
	}
}
//========================END RUNNING ON AUDIO THREAD===============================
//========================SETTERS===============================
void ofxSoundPlayerObject::setVolume(float vol, int index){
	if(index <= -1){
	
		volume = vol;
		
	}else{
		updateInstance([&](soundPlayInstance& inst){
			inst.volume = vol;
			inst.updateVolumes();
		},index, "ofxSoundPlayerObject::setVolume");
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPan(float _pan, int index){
	updateInstance([&](soundPlayInstance& inst){
		inst.pan = _pan;
		inst.updateVolumes();
	},index, "ofxSoundPlayerObject::setPan");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setSpeed(float spd, int index){
	if ( bStreaming && !ofIsFloatEqual(spd, 1.0f) ){
		ofLogWarning("ofxSoundPlayerObject") << "setting speed is not supported on bStreaming sounds";
		return;
	}
	updateInstance([&](soundPlayInstance& inst){
		inst.speed = spd;
		inst.relativeSpeed = spd*(double(sourceSampleRate)/double(playerSampleRate));
	},index, "ofxSoundPlayerObject::setSpeed");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPaused(bool bP, int index){
	updateInstance([&](soundPlayInstance& inst){
		inst.bIsPlaying = !bP;
	},index, "ofxSoundPlayerObject::setPaused");
	checkPaused();
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setLoop(bool bLp, int index){ 
	updateInstance([&](soundPlayInstance& inst){
		inst.loop = bLp;
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
	updateInstance([&](soundPlayInstance& inst){
		inst.position = pct* sourceNumFrames;
	},index, "ofxSoundPlayerObject::setPosition");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPositionMS(int ms, size_t index){
	setPosition(float(sourceSampleRate * ms)/ (1000.0f* sourceNumFrames ), index);
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setNumInstances(const size_t & num){
	std::lock_guard<std::mutex> lock(instacesMutex);
	instances.resize(num);
}
//========================GETTERS===============================
float ofxSoundPlayerObject::getPosition(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return float(instances[index].position)/float(sourceNumFrames);
	}
	return 0;
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::getPositionMS(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return float(instances[index].position)*1000./(float)sourceSampleRate;
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isPlaying(int index) const {
	if(!isLoaded()) return false;
	std::lock_guard<std::mutex> lock(instacesMutex);
	if (index <= -1){
		for (auto& i : instances) {
			if (i.bIsPlaying)return true;
		}
	}else{
		if(index < instances.size()){
			return instances[index].bIsPlaying;
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::getIsLooping(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index].loop;
	}
	return false;
}

//--------------------------------------------------------------
float ofxSoundPlayerObject::getRelativeSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index].relativeSpeed;
	}
	return 0;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getSpeed(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index].speed;
	}
	return 0;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getPan(size_t index) const{
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index < instances.size()){
		return instances[index].pan;
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isLoaded() const{
	return state == LOADED; 
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getVolume(int index) const{
	if(index <= -1){
		return volume.get();
	}else if(index < instances.size()){
		std::lock_guard<std::mutex> lock(instacesMutex);
		return instances[index].volume;
	}
	return 0;	
}
//--------------------------------------------------------------
unsigned long ofxSoundPlayerObject::getDurationMS(){
	return sourceDuration;
}
//--------------------------------------------------------------
const ofSoundBuffer & ofxSoundPlayerObject::getCurrentBuffer() const{
	if(bStreaming){
		return buffer;
	}else{
		return resampledBuffer;
	}
}
//--------------------------------------------------------------
const ofSoundBuffer & ofxSoundPlayerObject::getBuffer() const{
	return buffer;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxSoundPlayerObject::checkPaused(){
	bIsPlayingAny = false;
	std::lock_guard<std::mutex> lock(instacesMutex);
	for (auto& i : instances) {
		if (i.bIsPlaying) {
			bIsPlayingAny = true;
			break;
		}
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::updateInstance(std::function<void(soundPlayInstance& inst)> func, int index, string methodName) {
	
	std::lock_guard<std::mutex> lock(instacesMutex);
	if(index <= -1){
		for(auto& i: instances){
			func(i);
		}
	}else if(index < instances.size()){
		func(instances[index]);
	}else{
		ofLogVerbose(methodName) << "index out of range" << endl;
	}
}
//--------------------------------------------------------------
size_t ofxSoundPlayerObject::getNumChannels() {
	return sourceNumChannels;
}
//--------------------------------------------------------------
ofEvent<void>& ofxSoundPlayerObject::getAsyncLoadEndEvent(){
	return soundFile.loadAsyncEndEvent;
}
//--------------------------------------------------------------
const std::string ofxSoundPlayerObject::getFilePath() const{
	return soundFile.getPath();
}
