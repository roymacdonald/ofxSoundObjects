/*
 * ofxBasicSoundPlayer.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxBasicSoundPlayer.h"
#include "ofSoundUtils.h"
#include "ofSoundStream.h"
#include <float.h>

int ofxBasicSoundPlayer::maxSoundsTotal=128;
int ofxBasicSoundPlayer::maxSoundsPerPlayer=16;

//--------------------------------------------------------------
ofxBasicSoundPlayer::ofxBasicSoundPlayer() {
	bStreaming = false;
	bMultiplay = false;
	bIsLoaded = false;
	bIsPlayingAny = false;
	instances.resize(1);
	maxSounds = maxSoundsPerPlayer;
	volume.set("Volumen", 1, 0, 1);
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::canPlayInstance(){
	if (instances.size() < maxSounds -1) {
		return true;
	}
	for(auto& i: instances){
		if(!i.bIsPlaying){
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------
ofxBasicSoundPlayer::~ofxBasicSoundPlayer() {
	if (isLoaded()) {
		unload();
	}
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::load(std::filesystem::path filePath, bool _stream){
    ofLogVerbose() << "ofxBasicSoundPlayer::load" << endl << "Loading file: " << filePath.string() << endl;
	instances.clear();
	bIsLoaded = soundFile.load(filePath.string());
	if(!bIsLoaded) return false;

    ofLogVerbose() << "Duration     : " << soundFile.getDuration();
    ofLogVerbose() << "Channels     : " << soundFile.getNumChannels();
    ofLogVerbose() << "SampleRate   : " << soundFile.getSampleRate();
    ofLogVerbose() << "Num Samples  : " << soundFile.getNumSamples();
    
	bStreaming = _stream;
	if(!bStreaming){	
		soundFile.readTo(buffer);
		ofLogVerbose() << "Not streaming; Reading whole file into memory! ";
	}
	volume.setName(ofFilePath::getBaseName(filePath));
    playerNumChannels = soundFile.getNumChannels();
    playerSampleRate = soundFile.getSampleRate();
	return true;
}


//--------------------------------------------------------------
void ofxBasicSoundPlayer::audioOutBuffersChanged(int nFrames, int nChannels, int sampleRate){
	if(bStreaming){
		ofLogVerbose() << "Resizing buffer ";
		buffer.resize(nFrames*nChannels,0);
	}
	playerNumFrames = nFrames;
	playerNumChannels = nChannels;
	playerSampleRate = sampleRate;
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::unload(){
	buffer.clear();
	soundFile.close();
	bIsPlayingAny = false;
	bIsLoaded = false;
	instances.clear();
}
//--------------------------------------------------------------
size_t ofxBasicSoundPlayer::play() {
	size_t index = 0;
	if (bIsLoaded) {
	bool bCanPlay = true;
		if (instances.size() == 0) {
			instances.resize(1);
		}
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
			if(!bFound && maxSounds > instances.size()){
				instances.push_back(soundPlayInstance());
				index =instances.size() - 1;
				instances.back().id = index;
				setSpeed(1, index);
			}
			else {
				bCanPlay = false;
			}
		} else {
			if (bStreaming) {
				soundFile.seekTo(0);
			}
			instances[0].position = 0;
			setSpeed(1, 0);
			index = 0;
		}
		if(bCanPlay){
			setPaused(false, index);
		}
		cout << "Play: can play " << (bCanPlay ? "TRUE" : "FALSE") << endl;
	}
	return index;
}

//--------------------------------------------------------------
void ofxBasicSoundPlayer::stop(size_t index){
	if (bStreaming){
		soundFile.seekTo(0);
	}
	setPaused(true, index);
	
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::updatePositions(int nFrames){
	if (bIsLoaded) {
		for (auto& i : instances){
			i.position += nFrames*i.relativeSpeed;
			if (i.loop) {
				i.position %= buffer.getNumFrames();
			} else {
				i.position = ofClamp(i.position, 0, buffer.getNumFrames());
				if (i.position == buffer.getNumFrames()) {	// finished?
					i.bIsPlaying = false;
					checkPaused();
					ofNotifyEvent(endEvent, i.id);
				}
			}
		}
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::drawDebug(float x, float y){
	
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::audioOut(ofSoundBuffer& outputBuffer){
	if(bIsLoaded && bIsPlayingAny){
		
        int nFrames = outputBuffer.getNumFrames();
        int nChannels = outputBuffer.getNumChannels();
        if (playerNumChannels != nChannels || playerNumFrames != nFrames || playerSampleRate != outputBuffer.getSampleRate()) {
            audioOutBuffersChanged(nFrames, nChannels, outputBuffer.getSampleRate());
        }
		if(bStreaming){
			int samplesRead = soundFile.readTo(buffer,nFrames);
			if ( samplesRead==0 ){
//				bIsPlayingAny=false;
				stop();
	//			soundFile.seekTo(0);
			}else{
				newBufferE.notify(this,buffer);
				buffer.copyTo(outputBuffer);
			}
		}else{
			if (buffer.size()) {
				if (instances.size() == 1){
					if(abs(instances[0].speed - 1) < FLT_EPSILON) {
						buffer.copyTo(outputBuffer, nFrames, nChannels, instances[0].position, instances[0].loop);
						outputBuffer.stereoPan(instances[0].volumeLeft*volume,instances[0].volumeRight*volume);
					}
				}
				else {
					//for (int i = 0;i < (int)positions.size();i++) {
					for(auto& inst : instances){
						//assert( resampledBuffer.getNumFrames() == bufferSize*relativeSpeed[i] );
						if (abs(inst.speed - 1) < FLT_EPSILON) {
							buffer.copyTo(resampledBuffer, nFrames, nChannels, inst.position, inst.loop);
						}
						else {
							buffer.resampleTo(resampledBuffer, inst.position, nFrames, inst.relativeSpeed, inst.loop, ofSoundBuffer::Linear);
						}
						//                    resampledBuffer.stereoPan(volumesLeft[i],volumesRight[i]);
						outputBuffer.stereoPan(inst.volumeLeft*volume,inst.volumeRight*volume);
						newBufferE.notify(this, resampledBuffer);
						resampledBuffer.addTo(outputBuffer, 0, inst.loop);
					}
				}
				updatePositions(nFrames);
			}
			else {
				setPaused(instances.size());
			}
		}
        //if(bIsPlayingAny){
        //    float left, right;
        //    ofStereoVolumes(volume, pan, left, right);
        //    outputBuffer.stereoPan(left,right);
        //}
	}
}


//========================SETTERS===============================
void ofxBasicSoundPlayer::setVolume(float vol, size_t index){ 
	//cout << "setVolume: " << vol << "  index: " << index << endl;
	if(index < instances.size()){
		instances[index].volume = vol; 
		        ofStereoVolumes(instances[index].volume, instances[index].pan, instances[index].volumeLeft, instances[index].volumeRight);
	}else{
		cout << "index out of range" << endl;
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setPan(float _pan, size_t index){ 
	//cout << "setPan: " << _pan << "  index: " << index << endl;
	if(index < instances.size()){
		instances[index].pan = _pan;
        ofStereoVolumes(instances[index].volume, instances[index].pan, instances[index].volumeLeft, instances[index].volumeRight);
	}else{
		cout << "index out of range" << endl;
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setSpeed(float spd, size_t index){
	//cout << "setSpeed: " << spd << "  index: " << index << endl;
	if ( bStreaming && fabsf(spd-1.0f)<FLT_EPSILON ){
		ofLogWarning("ofxBasicSoundPlayer") << "setting speed is not supported on bStreaming sounds";
		return;
	}
	if(index < instances.size()){
		instances[index].speed = spd;
		instances[index].relativeSpeed = spd*(double(soundFile.getSampleRate())/double(playerSampleRate));
	}else{
		cout << "index out of range" << endl;
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setPaused(bool bP, size_t index){ 
	if(index < instances.size()){
		instances[index].bIsPlaying = !bP;
	}
	else if (index >= instances.size()) {
		for (auto& i : instances) {
			i.bIsPlaying = !bP;
		}
	}
	checkPaused();
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setLoop(bool bLp, size_t index){ 
	if(index < instances.size()){
		instances[index].loop = bLp;
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setMultiPlay(bool bMp){
	bMultiplay = bMp;
	if(!bMultiplay){
		instances.resize(1);
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setPosition(float pct, size_t index){
	pct = ofClamp(pct, 0, 1);
	if(index < instances.size()){
		if(bStreaming){
			index = 0;
			instances[index].position = pct*playerNumFrames;
			soundFile.seekTo(instances.back().position);
		}else{
			instances[index].position = pct*playerNumFrames;
		}
	}
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setPositionMS(int ms, size_t index){
	setPosition((float(ms)/ 1000.0f)* float(buffer.getSampleRate()), index);
	//TODO: implementar streaming
}
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setMaxSoundsTotal(int max){ maxSoundsTotal = max; }
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setMaxSoundsPerPlayer(int max){ maxSoundsPerPlayer = max; }
//--------------------------------------------------------------
void ofxBasicSoundPlayer::setMaxSounds(int max){ maxSounds = max; }
//========================GETTERS===============================
float ofxBasicSoundPlayer::getPosition(size_t index) const{ 
	if(index < instances.size()){
		return float(instances[index].position)/float(playerNumFrames); 
	}
	return 0;
}
//--------------------------------------------------------------
int ofxBasicSoundPlayer::getPositionMS(size_t index) const{
	if(index < instances.size()){
		return float(instances[index].position)*1000./buffer.getSampleRate(); 	
	}
	return 0;
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::isPlaying(size_t index) const{ 
	if(index < instances.size()){
		return instances[index].bIsPlaying; 
	}
	return false;
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::isPlaying() const{ 
	for (auto& i : instances) {
		if (i.bIsPlaying)return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::getIsLooping(size_t index) const{ 
	if(index < instances.size()){
	return instances[index].loop; 
	}
	return false;
}
//--------------------------------------------------------------
float ofxBasicSoundPlayer::getSpeed(size_t index) const{
	if(index < instances.size()){
		return instances[index].speed;
	}
	return 0;
}
//--------------------------------------------------------------
float ofxBasicSoundPlayer::getPan(size_t index) const{
	if(index < instances.size()){
		return instances[index].pan;
	}
	return 0.5;
}
//--------------------------------------------------------------
bool ofxBasicSoundPlayer::isLoaded() const{ return bIsLoaded; }
//--------------------------------------------------------------
float ofxBasicSoundPlayer::getVolume(size_t index) const{ 
	if(index < instances.size()){
		return instances[index].volume;
	}
	return 0;	
}
//--------------------------------------------------------------
unsigned long ofxBasicSoundPlayer::getDurationMS(){
	return buffer.getDurationMS();
}
//--------------------------------------------------------------
ofSoundBuffer & ofxBasicSoundPlayer::getCurrentBuffer(){
	if(bStreaming){
		return buffer;
	}else{
		return resampledBuffer;
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxBasicSoundPlayer::checkPaused(){
	bIsPlayingAny = false;
	for (auto& i : instances) {
		if (i.bIsPlaying) {
			bIsPlayingAny = true;
			break;
		}
	}
}