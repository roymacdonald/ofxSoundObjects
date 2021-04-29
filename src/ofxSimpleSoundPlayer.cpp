//
//  ofxSimpleSoundPlayer.cpp
//  example-soundPlayerObject_multi
//
//  Created by Roy Macdonald on 4/15/21.
//

#include "ofxSimpleSoundPlayer.h"
#include <algorithm>
#include <float.h>
#include "ofxSoundUtils.h"

//int ofxSimpleSoundPlayer::maxSoundsTotal=128;
//int ofxSimpleSoundPlayer::maxSoundsPerPlayer=16;
ofSoundBuffer ofxSimpleSoundPlayer::_dummyBuffer = ofSoundBuffer();
ofxSoundFile ofxSimpleSoundPlayer::_dummySoundFile = {};

size_t makeUniqueId(){
	static unique_ptr<size_t> count;
	if(!count){
		count = make_unique<size_t>(0);
	}
	(*count.get())++;
	return *count.get();
}


//--------------------------------------------------------------
ofxSimpleSoundPlayer::ofxSimpleSoundPlayer():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
	bStreaming = false;
	bListeningUpdate = false;

	bNotifyEnd = false;

	
	setName ("Player");
	
	volume.set("Volume", 1, 0, 1);

	volumeListener = volume.newListener(this, &ofxSimpleSoundPlayer::volumeChanged);
	
	resetValues();
	
	setState(UNLOADED);
	_id = makeUniqueId();
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::resetValues(){

	sourceSampleRate = 0;
	sourceNumFrames = 0;
	sourceNumChannels = 0;
	sourceDuration = 0;
	outputSampleRate = 0;
	bIsPlaying =false;
	loop = false;
	speed = 1;
	pan = 0;
	relativeSpeed =1;
	position=0;
	volume = 1;

	bNotifyEnd = false;

	bNeedsRelativeSpeedUpdate = false;
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::ofxSimpleSoundPlayer::volumeChanged(float&){
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setState(State newState){
	state = newState;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::isState(State compState){
	return state == compState;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::loadAsync(std::filesystem::path filePath, bool bAutoplay){
	if(isLoaded())unload();
	_makeSoundFile();
	
	setState(bAutoplay?LOADING_ASYNC_AUTOPLAY:LOADING_ASYNC);
	bStreaming = false;
	ofAddListener(soundFile->loadAsyncEndEvent, this, &ofxSimpleSoundPlayer::initFromSoundFile);
	soundFile->loadAsync(filePath.string());
	
	return true;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::load(std::filesystem::path filePath, bool _stream){
	if(isLoaded())unload();
	
	_makeSoundFile();
	
	if(_stream){
		if(soundFile->openFileStream(filePath.string())){
			bStreaming = true;
//			initFromSoundFile();
		}else{
			return false;
		}
	}else if(soundFile->load(filePath.string())){
		bStreaming = false;
	}else{
		return false;
	}
	
	initFromSoundFile();
	return isLoaded();
	
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::load(const ofSoundBuffer& loadBuffer, const std::string& name){
	if(isLoaded())unload();

	bStreaming = false;
	
	if(loadBuffer.size()){
		volume.setName(name);
		_makeSoundBuffer();
		loadBuffer.copyTo(*buffer);
		buffer->setSampleRate(loadBuffer.getSampleRate());
		initFromSoundBuffer();
	}
	
	
	return isLoaded();
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::load(shared_ptr<ofSoundBuffer> sharedBuffer, const std::string& name){
	if(buffer == nullptr ){
		ofLogError("ofxSimpleSoundPlayer::load") << "the passed shared buffer is null";
		return false;
	}
	if(isLoaded())unload();
	buffer = sharedBuffer;
	bStreaming = false;
		
	initFromSoundBuffer();
	return isLoaded();
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::load(shared_ptr<ofxSoundFile> sharedFile){
	if(soundFile == nullptr ){
		ofLogError("ofxSimpleSoundPlayer::load") << "the passed shared file is null";
		return false;
	}
	if(isLoaded())unload();
	soundFile = sharedFile;
	bStreaming = soundFile->isStreaming();
		
	initFromSoundFile();
	return isLoaded();
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::initFromSoundBuffer(){
	if(!buffer ){
		ofLogError("ofxSimpleSoundPlayer::initFromSoundBuffer") << "initing from sound buffer failed. This should not happen.";
		return;
	}
	sourceNumFrames = buffer->getNumFrames();
	sourceNumChannels = buffer->getNumChannels();
	sourceSampleRate = buffer->getSampleRate();
	sourceDuration   = 1000* float(buffer->getNumFrames()) / float(buffer->getSampleRate());
	
	setState(LOADED);
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::initFromSoundFile(){
	if(soundFile == nullptr ){
		ofLogError("ofxSimpleSoundPlayer::initFromSoundFile") << "initing from sound file failed. This should not happen.";
		return;
	}
	bool bLoaded = soundFile->isLoaded();
	if(bLoaded){
		stringstream ss;

		if(isState(LOADING_ASYNC) ||
		   isState(LOADING_ASYNC_AUTOPLAY)){
			ofRemoveListener(soundFile->loadAsyncEndEvent, this, &ofxSimpleSoundPlayer::initFromSoundFile);
		}
		
		
		auto name = ofFilePath::getBaseName(soundFile->getPath());
		if(!bStreaming){
			if(ofGetLogLevel() == OF_LOG_VERBOSE){
				ss << "Not streaming; Reading whole file into memory!\n";
			}
		}
//			load(soundFile->getBuffer(), name);
//		}else{
			volume.setName(name);
			

			sourceNumFrames = soundFile->getNumFrames();
			sourceNumChannels = soundFile->getNumChannels();
			sourceSampleRate = soundFile->getSampleRate();
			sourceDuration   = 1000* float(soundFile->getNumFrames()) / float(soundFile->getSampleRate());

			setState(LOADED);
		
//		}
		
		if(ofGetLogLevel() == OF_LOG_VERBOSE){
			ss << "Loading file : " << soundFile->getPath() << "\n";
			ss << getSourceInfo();
		}

		
		if(isState(LOADING_ASYNC_AUTOPLAY)){
			setState(LOADED);
			play();
		}else{
			setState(LOADED);
		}

		ofLogVerbose("ofxSimpleSoundPlayer::initFromSoundFile\n") << ss.str();

			
	}
}


//--------------------------------------------------------------
void ofxSimpleSoundPlayer::unload(){
	setState(UNLOADED);
	stop();
	resetValues();
	disableUpdateListener();
	{
		std::lock_guard<std::mutex> lock(mutex);
		if(buffer){
//			buffer->clear();
//			buffer.reset();
			buffer = nullptr;
		}
		if(soundFile){
//			soundFile.reset();
			soundFile =nullptr;
		}
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::play() {
	if (isLoaded()) {

		setPosition(0);//Should the position be set to zero here? I'm not sure.
		setSpeed(1);
		setPaused(false);
		
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::stop(){
	setPaused(true);
	setPosition(0);
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::update(ofEventArgs&){
	if(bNotifyEnd){
		ofNotifyEvent(endEvent, _id, this);
		bNotifyEnd = false;
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::drawDebug(float x, float y){
	stringstream ss;
	if(isLoaded()){
		ss << getSourceInfo() << "\n";
		ss << getPlaybackInfo();
		
	
	}else{
		ss << "File not loaded" << endl;
	}
	
	ofDrawBitmapString(ss.str(), x, y);
}

//========================BEGIN RUNNING ON AUDIO THREAD===============================
//--------------------------------------------------------------
void ofxSimpleSoundPlayer::checkBuffer(const ofSoundBuffer& outputBuffer){
	auto sr = outputBuffer.getSampleRate();
	if(sourceSampleRate != sr && sr != outputSampleRate ){
		outputSampleRate = sr;
		updateRelativeSpeed();
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::audioOut(ofSoundBuffer& outputBuffer){
	if(isLoaded() && bIsPlaying){
		
		checkBuffer(outputBuffer);
		auto nFrames = outputBuffer.getNumFrames();
//		auto nChannels = outputBuffer.getNumChannels();
//		if (playerNumChannels != nChannels || playerNumFrames != nFrames || playerSampleRate != outputBuffer.getSampleRate()) {
//			audioOutBuffersChanged(nFrames, nChannels, outputBuffer.getSampleRate());
//		}
		if(bNeedsRelativeSpeedUpdate){
			updateRelativeSpeed();
		}
		
		if(soundFile != nullptr){
			if(bStreaming){
				soundFile->readTo(outputBuffer, nFrames, position, loop);
			}else{
				processBuffers(soundFile->getBuffer(), outputBuffer);
			}
		}else if(buffer != nullptr){
				processBuffers(*buffer, outputBuffer);
		}
		else{
			return;
		}
		
		if(outputBuffer.getNumChannels() == 2){
			outputBuffer.stereoPan(volumeLeft, volumeRight );
		}else{
			outputBuffer *= getVolume();//using getVolume instead of volume because the former is thread safe
		}
		if(bNeedsFade ){
			if(position > 0){
				ofxSoundUtils::fadeBuffer(outputBuffer, !bFadeIn);
			}
			if(bFadeIn) bNeedsFade = false;
		}

				
		updatePositions(nFrames);
		
	}else{
		outputBuffer.set(0);//if not playing clear the passed buffer, because it might contain junk data
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::processBuffers(const ofSoundBuffer& sourceBuffer, ofSoundBuffer& outputBuffer){
	
	if (ofIsFloatEqual( relativeSpeed.load(),  1.0f)) {
		sourceBuffer.copyTo(outputBuffer, outputBuffer.getNumFrames(), outputBuffer.getNumChannels(), position, loop);
	}
	else {
		sourceBuffer.resampleTo(outputBuffer, position, outputBuffer.getNumFrames(), relativeSpeed, loop, ofSoundBuffer::Linear);
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::updatePositions(int nFrames){
	if (isLoaded()) {
		if(bIsPlaying){
			size_t nf = getNumFrames();
			size_t pos = position;
			pos += nFrames*relativeSpeed.load();
			if (loop) {
				pos %= nf;
			} else {
				pos = ofClamp(pos, 0, nf-1);
				if (pos == nf-1) {	// finished?
					bIsPlaying = false;
					bNotifyEnd = true;
				}
			}
			position = pos;
			if(bNeedsFade && !bFadeIn){
				bNeedsFade = false;
				bIsPlaying = false;
			}
		}
	}
}
//========================END RUNNING ON AUDIO THREAD===============================
//========================SETTERS===============================
void ofxSimpleSoundPlayer::setVolume(float vol){
	volume = vol;
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setPan(float _pan){
	pan = _pan;
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setSpeed(float spd){
	if ( bStreaming && !ofIsFloatEqual(spd, 1.0f) ){
		ofLogWarning("ofxSoundPlayerObject") << "setting speed is not supported on bStreaming sounds";
		return;
	}
	
	speed = spd;
	updateRelativeSpeed();
		
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::updateRelativeSpeed(){
	if(outputSampleRate != 0 && sourceSampleRate != 0) {
		relativeSpeed = speed.load() *(double(sourceSampleRate)/double(outputSampleRate));
		bNeedsRelativeSpeedUpdate = false;
	}else{
		bNeedsRelativeSpeedUpdate = true;
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setPaused(bool bP){
	if(!bP) bIsPlaying = true;
		
	bNeedsFade = true;
	bFadeIn =  !bP;
	if( !bP && !loop){
		enableUpdateListener();
	}else if(bP){
		disableUpdateListener();
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setLoop(bool bLp){
	loop = bLp;
	if(!loop && bIsPlaying){
		enableUpdateListener();
	}else{
		disableUpdateListener();
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setPosition(float pct){
	position = ofClamp(pct, 0, 1) * sourceNumFrames;
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setPositionMS(int ms){
	setPosition(float(sourceSampleRate * ms)/ (1000.0f* sourceNumFrames ));
}
//========================GETTERS===============================
//--------------------------------------------------------------
float ofxSimpleSoundPlayer::getPosition() const{
	return float(position.load())/float(sourceNumFrames);
}

//--------------------------------------------------------------
int ofxSimpleSoundPlayer::getPositionMS() const{
	return position*1000./(float)sourceSampleRate;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::isPlaying() const {
	if(!isLoaded()) return false;
	return bIsPlaying;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::getIsLooping() const{
	return loop;
}

//--------------------------------------------------------------
float ofxSimpleSoundPlayer::getRelativeSpeed() const{
	return relativeSpeed;
}

//--------------------------------------------------------------
float ofxSimpleSoundPlayer::getSpeed() const{
	return speed;
}

//--------------------------------------------------------------
float ofxSimpleSoundPlayer::getPan() const{
	return pan;
}

//--------------------------------------------------------------
bool ofxSimpleSoundPlayer::isLoaded() const{
	return state == LOADED;
}

//--------------------------------------------------------------
float ofxSimpleSoundPlayer::getVolume() const{
	float vol;
	{
		std::lock_guard<std::mutex> lock(volumeMutex);
		vol = volume.get();
	}
	return vol;
}

//--------------------------------------------------------------
unsigned long ofxSimpleSoundPlayer::getDurationMS(){
	return sourceDuration;
}

//--------------------------------------------------------------
const ofSoundBuffer & ofxSimpleSoundPlayer::getBuffer() const{
	if(buffer != nullptr){
		return *buffer;
	}else if (soundFile != nullptr){
		return soundFile->getBuffer();
	}
	ofLogError("ofxSimpleSoundPlayer::getBuffer()") << "Can not return buffer as there is none loaded. returning an empty buffer";
	return _dummyBuffer;
}

//--------------------------------------------------------------
shared_ptr<ofSoundBuffer> ofxSimpleSoundPlayer::getSharedBuffer() const{
	return buffer;
}

//--------------------------------------------------------------
const ofxSoundFile& ofxSimpleSoundPlayer::getSoundFile() const{
	if(soundFile != nullptr){
		return *soundFile;
	}
	return _getDummySoundFile();
}

//--------------------------------------------------------------
ofxSoundFile& ofxSimpleSoundPlayer::getSoundFile(){
	if(soundFile != nullptr){
		return *soundFile;
	}
	return _getDummySoundFile();
}

//--------------------------------------------------------------
ofxSoundFile& ofxSimpleSoundPlayer::_getDummySoundFile()
{
	ofLogError("ofxSimpleSoundPlayer::getSoundFile()") << "Can not return sound file as there is none loaded. returning an empty one";
	return _dummySoundFile;
}
//--------------------------------------------------------------
shared_ptr <ofxSoundFile> ofxSimpleSoundPlayer::getSharedSoundFile() const{
	return soundFile;
}

//--------------------------------------------------------------
size_t ofxSimpleSoundPlayer::getNumChannels() {
	return sourceNumChannels;
}

//--------------------------------------------------------------
ofEvent<void>& ofxSimpleSoundPlayer::getAsyncLoadEndEvent(){
	_makeSoundFile();
	
	return soundFile->loadAsyncEndEvent;
	
}

//--------------------------------------------------------------
std::string ofxSimpleSoundPlayer::getFilePath() const{
	if(soundFile == nullptr) return "";
	
	return soundFile->getPath();
}

//--------------------------------------------------------------
size_t ofxSimpleSoundPlayer::getNumFrames() const{
	return sourceNumFrames;
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::updateVolumes(){
	float vl, vr;
	ofStereoVolumes(volume.get(), pan.load(), vl, vr);
	volumeLeft = vl;
	volumeRight = vr;
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::enableUpdateListener(){
	if(!bListeningUpdate){
		updateListener = ofEvents().update.newListener(this, &ofxSimpleSoundPlayer::update);
		bListeningUpdate = true;
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::disableUpdateListener(){
	if(bListeningUpdate){
		updateListener.unsubscribe();
		bListeningUpdate = false;
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::_makeSoundFile(){
	if(soundFile == nullptr){
		soundFile = make_shared<ofxSoundFile>();
	}
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::_makeSoundBuffer(){
	if(buffer == nullptr){
		buffer = make_shared<ofSoundBuffer>();
	}
}

//--------------------------------------------------------------
std::string  ofxSimpleSoundPlayer::getSourceInfo() const{
	stringstream ss;
	if(isLoaded()){
		ss << "DurationMS   : " << sourceDuration << endl;
		ss << "Channels     : " << sourceNumChannels << endl;
		ss << "SampleRate   : " << sourceSampleRate << endl;
		ss << "Num Samples  : " << sourceNumFrames << endl;
	}
	return ss.str();
}

//--------------------------------------------------------------
std::string  ofxSimpleSoundPlayer::getPlaybackInfo() const {
	stringstream ss;
	ss << "Position: " << getPosition() << endl;
	ss << "PositionMS: " << getPositionMS() << endl;
	ss << "Playing: " << boolalpha << isPlaying() << endl;
	ss << "Speed: " << getSpeed() << endl;
	ss << "Rel. Speed: " << getRelativeSpeed() << endl;
	ss << "Pan: " << getPan() << endl;
	ss << "Volume: " << getVolume() << endl;
	ss << "IsLooping: " << boolalpha << getIsLooping() << endl;

	return ss.str();
	
}

//--------------------------------------------------------------
size_t ofxSimpleSoundPlayer::getId() const{
	return _id;
}

//--------------------------------------------------------------
void ofxSimpleSoundPlayer::setId(size_t id){
	_id = id;
}
