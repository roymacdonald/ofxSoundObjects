//
//  ofxSimpleSoundPlayer.cpp
//  example-soundPlayerObject_multi
//
//  Created by Roy Macdonald on 4/15/21.
//

#include "ofxSingleSoundPlayer.h"
#include <algorithm>
#include <float.h>
#include "ofxSoundUtils.h"


//int ofxSimpleSoundPlayer::maxSoundsTotal=128;
//int ofxSimpleSoundPlayer::maxSoundsPerPlayer=16;
ofSoundBuffer ofxSingleSoundPlayer::_dummyBuffer = ofSoundBuffer();
ofxSoundFile ofxSingleSoundPlayer::_dummySoundFile = {};

size_t makeUniqueId(){
	static unique_ptr<size_t> count;
	if(!count){
		count = make_unique<size_t>(0);
	}
	(*count.get())++;
	return *count.get();
}


//--------------------------------------------------------------
ofxSingleSoundPlayer::ofxSingleSoundPlayer():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
	bStreaming = false;
	bListeningUpdate = false;

	bNotifyEnd = false;
	
	
	setName ("Player");
	
	volume.set("Volume", 1, 0, 1);

	volumeListener = volume.newListener(this, &ofxSingleSoundPlayer::volumeChanged);
	
	resetValues();
	
	setState(UNLOADED);
	_id = makeUniqueId();
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::resetValues(){

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

	bNeedsPreprocessBuffer = false;
	bNotifyEnd = false;

	bNeedsRelativeSpeedUpdate = false;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::ofxSingleSoundPlayer::volumeChanged(float&){
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setState(State newState){
	state = newState;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::isState(State compState) const{
	return state == compState;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::loadAsync(std::filesystem::path filePath, bool bAutoplay){
	if(isLoaded())unload();
	_makeSoundFile();
	
	setState(bAutoplay?LOADING_ASYNC_AUTOPLAY:LOADING_ASYNC);
	bStreaming = false;
	ofAddListener(soundFile->loadAsyncEndEvent, this, &ofxSingleSoundPlayer::initFromSoundFile);
	soundFile->loadAsync(filePath.string());
	
	return true;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::load(std::filesystem::path filePath, bool _stream){
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
bool ofxSingleSoundPlayer::load(const ofSoundBuffer& loadBuffer, const std::string& name){
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
bool ofxSingleSoundPlayer::load(shared_ptr<ofxSoundFile>& sharedFile){
	if(sharedFile == nullptr ){
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
void ofxSingleSoundPlayer::initFromSoundBuffer(){
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
void ofxSingleSoundPlayer::initFromSoundFile(){
	if(soundFile == nullptr ){
		ofLogError("ofxSimpleSoundPlayer::initFromSoundFile") << "initing from sound file failed. This should not happen.";
		return;
	}
	bool bLoaded = soundFile->isLoaded();
	if(bLoaded){
		stringstream ss;

		if(isState(LOADING_ASYNC) ||
		   isState(LOADING_ASYNC_AUTOPLAY)){
			ofRemoveListener(soundFile->loadAsyncEndEvent, this, &ofxSingleSoundPlayer::initFromSoundFile);
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
void ofxSingleSoundPlayer::unload(){
	setState(UNLOADED);
	stop();
	resetValues();
	disableUpdateListener(true);
	{
		std::lock_guard<std::mutex> lock(mutex);
		if(buffer){
			buffer = nullptr;
		}
		if(soundFile){
			soundFile =nullptr;
		}
		if(preprocessedBuffer){
			preprocessedBuffer = nullptr;
		}
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::play() {
	if (isLoaded()) {

		setPosition(0);//Should the position be set to zero here? I'm not sure.
		setPaused(false);
		
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::stop(){
	setPaused(true);
	setPosition(0);
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::update(ofEventArgs&){
	if(bNotifyEnd){
		ofNotifyEvent(endEvent, _id, this);
		bNotifyEnd = false;
	}
	if(bNeedsPreprocessBuffer){
		if(preprocessBuffer()){
			disableUpdateListener();
		}
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::drawDebug(float x, float y){
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
void ofxSingleSoundPlayer::checkBuffer(const ofSoundBuffer& outputBuffer){
	auto sr = outputBuffer.getSampleRate();
	
	if(sourceSampleRate != sr || sr != outputSampleRate ){
		outputSampleRate = sr;
		updateRelativeSpeed();
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::audioOut(ofSoundBuffer& outputBuffer){
	if(isLoaded() && bIsPlaying){
		
		checkBuffer(outputBuffer);
		auto nFrames = outputBuffer.getNumFrames();
		
		if(bNeedsRelativeSpeedUpdate){
			updateRelativeSpeed();
		}
		
		if(bNeedsPreprocessBuffer){
			outputBuffer.set(0);
			return;
		}
		
		
		if(soundFile != nullptr && bStreaming){
				soundFile->readTo(outputBuffer, nFrames, position, loop);
		}else{
			auto &sourceBuffer = getBuffer();
			
			if ((preprocessedBuffer && !bNeedsPreprocessBuffer) || ofIsFloatEqual( relativeSpeed.load(),  1.0f)) {
				sourceBuffer.copyTo(outputBuffer, nFrames, outputBuffer.getNumChannels(), position, loop.load());
			}
			else {
#ifdef USE_OFX_SAMPLE_RATE
				if(sampleRateConverter == nullptr){
					sampleRateConverter = make_unique<ofxSamplerate>() ;
				}
				nFrames = sampleRateConverter->changeSpeed(sourceBuffer, outputBuffer, relativeSpeed.load(), position,loop );
#else
				sourceBuffer.resampleTo(outputBuffer, position, nFrames, relativeSpeed, loop.load(), ofSoundBuffer::Linear);
#endif
			}
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
void ofxSingleSoundPlayer::updatePositions(int nFrames){
	if (isLoaded()) {
		if(bIsPlaying){
			size_t nf = getNumFrames();
			size_t pos = position;
			auto prevPos = pos;
			pos += nFrames;//*relativeSpeed.load();
			if (loop) {
				pos %= nf;
				if (pos == nf-1 || prevPos > pos) {	// looped?
					bNotifyEnd = true;
				}
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
void ofxSingleSoundPlayer::setVolume(float vol){
	volume = vol;
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setPan(float _pan){
	pan = _pan;
	updateVolumes();
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setSpeed(float spd, bool preprocess){
	if ( bStreaming && !ofIsFloatEqual(spd, 1.0f) ){
		ofLogWarning("ofxSoundPlayerObject") << "setting speed is not supported on bStreaming sounds";
		return;
	}
	
	speed = spd;
	updateRelativeSpeed();
	if(preprocess){
		preprocessBuffer();
	}
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::preprocessBuffer(){
	bNeedsPreprocessBuffer = true;
	if(isLoaded() && !bNeedsRelativeSpeedUpdate && outputSampleRate != 0 && sourceSampleRate != 0){
		setState(RESAMPLING);
		preprocessedBuffer = make_unique<ofSoundBuffer>();
	
		if(buffer != nullptr){
			(*preprocessedBuffer) = (*buffer);
		}else if (soundFile != nullptr){
			*preprocessedBuffer =soundFile->getBuffer();
		}else{
			return false;
		}
		
		preprocessedBuffer->setSampleRate(sourceSampleRate);
		
		if(preprocessedBuffer->size()){
			preprocessedBuffer->resample(relativeSpeed);
		}else{
			cout << "preprocessed buffer size == 0\n";
		}
		bNeedsPreprocessBuffer = false;
		setState(LOADED);
		return true;
	}else{
		enableUpdateListener();
	}
	return false;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::updateRelativeSpeed(){
	if(outputSampleRate != 0 && sourceSampleRate != 0) {
		relativeSpeed = speed.load() *(double(sourceSampleRate)/double(outputSampleRate));
		bNeedsRelativeSpeedUpdate = false;
	}else{
		bNeedsRelativeSpeedUpdate = true;
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setPaused(bool bP){
	if(!bP) bIsPlaying = true;
		
	bNeedsFade = true;
	bFadeIn =  !bP;
	if( !bP ){
		enableUpdateListener();
	}else {
		disableUpdateListener();
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setLoop(bool bLp){
	loop = bLp;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setPosition(float pct){
	position = ofClamp(pct, 0, 1) * sourceNumFrames;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setPositionMS(int ms){
	setPosition(float(sourceSampleRate * ms)/ (1000.0f* sourceNumFrames ));
}
//========================GETTERS===============================
//--------------------------------------------------------------
float ofxSingleSoundPlayer::getPosition() const{
	return float(position.load())/float(sourceNumFrames);
}

//--------------------------------------------------------------
int ofxSingleSoundPlayer::getPositionMS() const{
	return position*1000./(float)sourceSampleRate;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::isPlaying() const {
	if(!isLoaded()) return false;
	return bIsPlaying;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::isLooping() const{
	return loop;
}

//--------------------------------------------------------------
float ofxSingleSoundPlayer::getRelativeSpeed() const{
	return relativeSpeed;
}

//--------------------------------------------------------------
float ofxSingleSoundPlayer::getSpeed() const{
	return speed;
}

//--------------------------------------------------------------
float ofxSingleSoundPlayer::getPan() const{
	return pan;
}

//--------------------------------------------------------------
bool ofxSingleSoundPlayer::isLoaded() const{
	return state == LOADED;
}

//--------------------------------------------------------------
float ofxSingleSoundPlayer::getVolume() const{
	float vol;
	{
		std::lock_guard<std::mutex> lock(volumeMutex);
		vol = volume.get();
	}
	return vol;
}

//--------------------------------------------------------------
unsigned long ofxSingleSoundPlayer::getDurationMS(){
	return sourceDuration;
}

//--------------------------------------------------------------
const ofSoundBuffer & ofxSingleSoundPlayer::getBuffer() const{
	if(preprocessedBuffer != nullptr && !isState(RESAMPLING)){
		return *preprocessedBuffer;
	}else if(buffer != nullptr){
		return *buffer;
	}else if (soundFile != nullptr){
		return soundFile->getBuffer();
	}
	ofLogError("ofxSimpleSoundPlayer::getBuffer()") << "Can not return buffer as there is none loaded. returning an empty buffer";
	return _dummyBuffer;
}

//--------------------------------------------------------------
const ofxSoundFile& ofxSingleSoundPlayer::getSoundFile() const{
	if(soundFile != nullptr){
		return *soundFile;
	}
	return _getDummySoundFile();
}

//--------------------------------------------------------------
ofxSoundFile& ofxSingleSoundPlayer::getSoundFile(){
	if(soundFile != nullptr){
		return *soundFile;
	}
	return _getDummySoundFile();
}

//--------------------------------------------------------------
ofxSoundFile& ofxSingleSoundPlayer::_getDummySoundFile()
{
	ofLogError("ofxSimpleSoundPlayer::getSoundFile()") << "Can not return sound file as there is none loaded. returning an empty one";
	return _dummySoundFile;
}
//--------------------------------------------------------------
const shared_ptr <ofxSoundFile>& ofxSingleSoundPlayer::getSharedSoundFile() const{
	return soundFile;
}
//--------------------------------------------------------------
shared_ptr <ofxSoundFile>& ofxSingleSoundPlayer::getSharedSoundFile(){
	return soundFile;
}
//--------------------------------------------------------------
size_t ofxSingleSoundPlayer::getNumChannels() {
	return sourceNumChannels;
}

//--------------------------------------------------------------
ofEvent<void>& ofxSingleSoundPlayer::getAsyncLoadEndEvent(){
	_makeSoundFile();
	return soundFile->loadAsyncEndEvent;
}

//--------------------------------------------------------------
std::string ofxSingleSoundPlayer::getFilePath() const{
	if(soundFile == nullptr) return "";
	return soundFile->getPath();
}

//--------------------------------------------------------------
size_t ofxSingleSoundPlayer::getNumFrames() const{
	return sourceNumFrames;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::updateVolumes(){
	float vl, vr;
	ofStereoVolumes(volume.get(), pan.load(), vl, vr);
	volumeLeft = vl;
	volumeRight = vr;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::enableUpdateListener(){
	if(!bListeningUpdate){
		updateListener = ofEvents().update.newListener(this, &ofxSingleSoundPlayer::update);
		bListeningUpdate = true;
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::disableUpdateListener(bool forceDisable){
	if(bListeningUpdate && ((!bNeedsPreprocessBuffer)  || forceDisable)){
		updateListener.unsubscribe();
		bListeningUpdate = false;
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::_makeSoundFile(){
	if(soundFile == nullptr){
		soundFile = make_shared<ofxSoundFile>();
	}
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::_makeSoundBuffer(){
	if(buffer == nullptr){
		buffer = make_unique<ofSoundBuffer>();
	}
}

//--------------------------------------------------------------
std::string  ofxSingleSoundPlayer::getSourceInfo() const{
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
std::string  ofxSingleSoundPlayer::getPlaybackInfo() const {
	stringstream ss;
	ss << "Position: " << getPosition() << endl;
	ss << "PositionMS: " << getPositionMS() << endl;
	ss << "Playing: " << boolalpha << isPlaying() << endl;
	ss << "Speed: " << getSpeed() << endl;
	ss << "Rel. Speed: " << getRelativeSpeed() << endl;
	ss << "Pan: " << getPan() << endl;
	ss << "Volume: " << getVolume() << endl;
	ss << "IsLooping: " << boolalpha << isLooping() << endl;

	return ss.str();
	
}

//--------------------------------------------------------------
size_t ofxSingleSoundPlayer::getId() const{
	return _id;
}

//--------------------------------------------------------------
void ofxSingleSoundPlayer::setId(size_t id){
	_id = id;
}
