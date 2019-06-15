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
//	maxSounds = maxSoundsPerPlayer;
	{
		std::lock_guard<std::mutex> lock(mutex);
		instances.resize(1);
		volume.set("Volumen", 1, 0, 1);	
	}
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
	ofAddListener(soundFile.loadAsyncEndEvent, this, &ofxSoundPlayerObject::init);

}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::load(std::filesystem::path filePath, bool _stream){
	if(isLoaded())unload();
	if(soundFile.load(filePath.string())){
		//	bStreaming = _stream;
		
		bStreaming = false; // temporarily unavailable, until properly implementing in ofxSoundFile
		init();
		
		
		return isLoaded();
	}
	return false;
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::init(){
	
	bool bLoaded = soundFile.isLoaded();
	if(bLoaded){	
		stringstream ss;

		if(isState(LOADING_ASYNC) ||
		   isState(LOADING_ASYNC_AUTOPLAY)){ 
			ofRemoveListener(soundFile.loadAsyncEndEvent, this, &ofxSoundPlayerObject::init);
		}
		instances.resize(1);
		if(ofGetLogLevel() == OF_LOG_VERBOSE){
			ss << "Loading file : " << soundFile.getPath() << "\n";
			ss << "Duration     : " << soundFile.getDuration() << "\n";
			ss << "Channels     : " << soundFile.getNumChannels() << "\n";
			ss << "SampleRate   : " << soundFile.getSampleRate() << "\n";
			ss << "Num Samples  : " << soundFile.getNumSamples() << "\n";
		}
			
		if(!bStreaming){
			
			buffer = soundFile.getBuffer(); 
			
			if(ofGetLogLevel() == OF_LOG_VERBOSE){
				ss << "Not streaming; Reading whole file into memory!\n";
			}
		}
		volume.setName(ofFilePath::getBaseName(soundFile.getPath()));
		playerNumChannels = soundFile.getNumChannels();
		playerSampleRate = soundFile.getSampleRate();
		
		
		
		if(isState(LOADING_ASYNC_AUTOPLAY)){
			setState(LOADED);
			play();
		}else{
			setState(LOADED);
		}

		ofLogVerbose("ofxSoundPlayerObject::init\n") << ss.str();

			
	}
}

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
void ofxSoundPlayerObject::unload(){
	bIsPlayingAny = false;
	setState(UNLOADED);
	std::lock_guard<std::mutex> lock(mutex);
	buffer.clear();
	soundFile.reset();
	instances.resize(1);
	clearInstanceEndNotificationQueue();
	
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::play() {
	size_t index = 0;
	if (isLoaded()) {

//		bool bCanPlay = true;
		if (bMultiplay){
			bool bFound = false;
			for (auto& i : instances) {
				if (!i.bIsPlaying) {
					index = i.id;
					setSpeed(1, index);
					bFound = true;
					break;
				}
			}
			if(!bFound){// && maxSounds > instances.size()){
				instances.push_back(soundPlayInstance());
				index =instances.size() - 1;
				instances.back().id = index;
				setSpeed(1, index);
//			} else {
//				bCanPlay = false;
			}
		} else {
			if (instances.size() == 0) {
				instances.resize(1);
			}
			setPosition(0,0);//Should the position be set to zero here? I'm not sure.
			instances[0].id = 0;
			setSpeed(1, 0);
			index = 0;
		}
//		if(bCanPlay){
//			cout << "playing" << endl;
			setPaused(false, index);
			return index;
//		}
	}
	return -1;
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::stop(size_t index){
	setPaused(true, index);
	instances.resize(1);
	setPosition(0);
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::clearInstanceEndNotificationQueue(){
	updateListener.unsubscribe();
	bListeningUpdate = false;
	std::lock_guard<std::mutex> lock(mutex);
	endedInstancesToNotify.clear();
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::addInstanceEndNotification(const size_t & id){
	{
		std::lock_guard<std::mutex> lock(mutex);
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
void ofxSoundPlayerObject::updatePositions(int nFrames){
	if (isLoaded()) {
		for (auto& i : instances){
			if(i.bIsPlaying){
				i.position += nFrames*i.relativeSpeed;
				if (i.loop) {
					i.position %= buffer.getNumFrames();
				} else {
					i.position = ofClamp(i.position, 0, buffer.getNumFrames()-1);
					if (i.position == buffer.getNumFrames()-1) {	// finished?
						setPaused(true, i.id);
//						ofNotifyEvent(endEvent, i.id);
						addInstanceEndNotification(i.id);
						
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::drawDebug(float x, float y){
	stringstream ss;
	
	ss << "Duration     : " << soundFile.getDuration() << endl;
	ss << "Channels     : " << soundFile.getNumChannels() << endl;
	ss << "SampleRate   : " << soundFile.getSampleRate() << endl;
	ss << "Num Samples  : " << soundFile.getNumSamples() << endl;
	
	ss << "INSTANCES" << endl;
	
	for (int i =0; i< instances.size(); i++) {
		ss << i << "------------------------" <<endl;
		
		ss << "    volume: " << instances[i].volume << endl;
		ss << "    bIsPlaying: " << instances[i].bIsPlaying << endl;
		ss << "    loop: " << instances[i].loop << endl;
		ss << "    speed: " << instances[i].speed << endl;
		ss << "    pan: " << instances[i].pan << endl;
		ss << "    relativeSpeed: " << instances[i].relativeSpeed << endl;
		ss << "    position: " << instances[i].position << endl;
		ss << "    position Norm: " << getPosition(i) <<endl;
		ss << "    position MS: " << getPositionMS(i) <<endl; 
		ss << "    volumeLeft: " << instances[i].volumeLeft << endl;
		ss << "    volumeRight: " << instances[i].volumeRight << endl;
		ss << "    id: " << instances[i].id << endl;
	}
	
	ofDrawBitmapString(ss.str(), x, y);
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::audioOut(ofSoundBuffer& outputBuffer){
	if(isLoaded() && bIsPlayingAny){
		
		auto nFrames = outputBuffer.getNumFrames();
		auto nChannels = outputBuffer.getNumChannels();
		if (playerNumChannels != nChannels || playerNumFrames != nFrames || playerSampleRate != outputBuffer.getSampleRate()) {
			audioOutBuffersChanged(nFrames, nChannels, outputBuffer.getSampleRate());
		}
		if(bStreaming){
			//			int samplesRead = soundFile.readTo(buffer,nFrames);
			//			if ( samplesRead==0 ){
			//				stop();
			//			}else{
			//				buffer.copyTo(outputBuffer);
			//			//	newBufferE.notify(this,buffer);// is there any need to notify this?
			//			}
		}else{
			if (buffer.size()) {
				auto processBuffers = [&](ofSoundBuffer& buf, soundPlayInstance& i){
					//assert( resampledBuffer.getNumFrames() == bufferSize*relativeSpeed[i] );
					if (abs(i.speed - 1) < FLT_EPSILON) {
						buffer.copyTo(buf, nFrames, nChannels, i.position, i.loop);
					}
					else {
						buffer.resampleTo(buf, i.position, nFrames, i.relativeSpeed, i.loop, ofSoundBuffer::Linear);
					}
					if(buf.getNumChannels() == 2){
						buf.stereoPan(i.volumeLeft*volume,i.volumeRight*volume);
					}
				};
				if (instances.size() == 1){
					processBuffers(outputBuffer, instances[0]);
				}
				else {
					for(auto& inst : instances){
						processBuffers(resampledBuffer, inst);
						//					newBufferE.notify(this, resampledBuffer);
						resampledBuffer.addTo(outputBuffer, 0, inst.loop);
					}
				}
				updatePositions(nFrames);
			}
			else {
				setPaused(-1);
			}
		}
	}else{
		outputBuffer.set(0);//if not playing clear the passed buffer, because it might contain junk data
	}
}

//========================SETTERS===============================
void ofxSoundPlayerObject::setVolume(float vol, int index){
	updateInstance([&](soundPlayInstance& inst){
		inst.volume = vol;
		inst.updateVolumes();
	},index, "ofxSoundPlayerObject::setVolume");
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
	if ( bStreaming && fabsf(spd-1.0f)<FLT_EPSILON ){
		ofLogWarning("ofxSoundPlayerObject") << "setting speed is not supported on bStreaming sounds";
		return;
	}
	updateInstance([&](soundPlayInstance& inst){
		inst.speed = spd;
		inst.relativeSpeed = spd*(double(soundFile.getSampleRate())/double(playerSampleRate));
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
		instances.resize(1);
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPosition(float pct, size_t index){
	pct = ofClamp(pct, 0, 1);
	updateInstance([&](soundPlayInstance& inst){
		inst.position = pct* soundFile.getNumSamples(); 
		if(bStreaming){
			//soundFile.seekTo(inst.position);
		}
	},index, "ofxSoundPlayerObject::setPosition");
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::setPositionMS(int ms, size_t index){
	setPosition(float(buffer.getSampleRate() * ms)/ (1000.0f* soundFile.getNumSamples() ), index);
}
////--------------------------------------------------------------
//void ofxSoundPlayerObject::setMaxSoundsTotal(int max){ maxSoundsTotal = max; }
////--------------------------------------------------------------
//void ofxSoundPlayerObject::setMaxSoundsPerPlayer(int max){ maxSoundsPerPlayer = max; }
////--------------------------------------------------------------
//void ofxSoundPlayerObject::setMaxSounds(int max){ maxSounds = max; }
//========================GETTERS===============================
float ofxSoundPlayerObject::getPosition(size_t index) const{ 
	if(index < instances.size()){
		return float(instances[index].position)/float( soundFile.getNumSamples()); 
	}
	return 0;
}
//--------------------------------------------------------------
int ofxSoundPlayerObject::getPositionMS(size_t index) const{
	if(index < instances.size()){
		return float(instances[index].position)*1000./buffer.getSampleRate(); 	
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxSoundPlayerObject::isPlaying(int index) const {
	if(!isLoaded()) return false;
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
	if(index < instances.size()){
		return instances[index].loop;
	}
	return false;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getSpeed(size_t index) const{
	if(index < instances.size()){
		return instances[index].speed;
	}
	return 0;
}
//--------------------------------------------------------------
float ofxSoundPlayerObject::getPan(size_t index) const{
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
float ofxSoundPlayerObject::getVolume(size_t index) const{ 
	if(index < instances.size()){
		return instances[index].volume;
	}
	return 0;	
}
//--------------------------------------------------------------
unsigned long ofxSoundPlayerObject::getDurationMS(){
	return soundFile.getDuration();
}
//--------------------------------------------------------------
ofSoundBuffer & ofxSoundPlayerObject::getCurrentBuffer(){
	if(bStreaming){
		return buffer;
	}else{
		return resampledBuffer;
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxSoundPlayerObject::checkPaused(){
	bIsPlayingAny = false;
	for (auto& i : instances) {
		if (i.bIsPlaying) {
			bIsPlayingAny = true;
			break;
		}
	}
}
//--------------------------------------------------------------
void ofxSoundPlayerObject::updateInstance(std::function<void(soundPlayInstance& inst)> func, int index, string methodName) {
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
	return soundFile.getNumChannels();
}
//--------------------------------------------------------------
ofEvent<void>& ofxSoundPlayerObject::getAsyncLoadEndEvent(){
	return soundFile.loadAsyncEndEvent;
}
