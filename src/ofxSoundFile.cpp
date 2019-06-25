/*
 * ofxSoundFile.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxSoundFile.h"
#include "ofLog.h"
#include "ofUtils.h"



using namespace std;
//--------------------------------------------------------------
bool ofxLoadSound(ofSoundBuffer &buff, string path){
	ofxSoundFile sf(path);
	if(sf.isLoaded()){
		buff = sf.getBuffer();
		//        sf.readTo(buff);
		return true;
	}else{
		return false;
	}
}

//--------------------------------------------------------------
// for now this only write 16 bit PCM WAV files.
// It can't really live in ofxSoundFile yet because
// ofxSoundFile doesn't hold a complete representation
// the sound file that can be written to disk. You'd
// need something that would let you stream the data to
// it via writeTo() or similar. Doesn't really fit wtih
// the current model.
bool ofxSaveSound(const ofSoundBuffer &buff,  string path){
	ofxSoundFile soundFile;
	return soundFile.save(path, buff,SF_FORMAT_PCM_16);
}


//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile() {	
	reset();
}
//--------------------------------------------------------------
ofxSoundFile::~ofxSoundFile() {}
//--------------------------------------------------------------
void ofxSoundFile::ThreadHelper::threadedFunction(){
	if(isThreadRunning()){
		ofAddListener(ofEvents().update, &soundFile, &ofxSoundFile::removeThreadHelperCB);
		soundFile.loadFile(true);
	}
}
//--------------------------------------------------------------
bool ofxSoundFile::removeThreadHelper(){
	if(threadHelper && !threadHelper->isThreadRunning()){
		ofRemoveListener(ofEvents().update, this, &ofxSoundFile::removeThreadHelperCB);
		threadHelper.reset();
		threadHelper = nullptr;
		return true;
	}
	return false;
}
//--------------------------------------------------------------
void ofxSoundFile::removeThreadHelperCB(ofEventArgs&){
	if(removeThreadHelper()){
		ofNotifyEvent(loadAsyncEndEvent);
	}
}
//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile(string path) {
	ofxSoundFile();
	load(path);
}
//--------------------------------------------------------------
void ofxSoundFile::setFromAudioFile(ofxAudioFile& audiofile){
	if(!audiofile.loaded()){
		return;
	}
	auto tempPath =  path;
	reset();
	numChannels = audiofile.channels();
	sampleRate =  audiofile.samplerate();
	numSamples = audiofile.length();
	
	buffer.resize(numSamples*numChannels);
	buffer.copyFrom(audiofile.data(), numSamples, numChannels, sampleRate);
	
	audiofile.free();
	
	path = tempPath;

	duration = 1000* float(numSamples) / float(sampleRate);
	
	bCompressed = (ofFilePath::getFileExt(ofToLower(path)) == "mp3");
	
	ofLogVerbose("ofxSoundFile::load") << "file loaded. is mp3 : " << (bCompressed?"YES":"NO");
	bLoaded = true;
}
//--------------------------------------------------------------
bool ofxSoundFile::loadFile( bool bAsync){
	if(!bAsync && threadHelper != nullptr){
		ofLogError("ofxSoundFile::load") << "Can not load a file while another is loading";
		return false;
	}
	if(isLoaded())reset();
	
	if( ofFile::doesFileExist( path ) ){
		ofxAudioFile audiofile; 
		audiofile.setVerbose(true);
		audiofile.load( path );
		bool bL = audiofile.loaded();
		if (!bL){
			ofLogError("ofxSoundFile::load")<<"error loading file, double check the file path";
			return false;
		}
		
		if(bAsync){
			static ofMutex mutex;
			std::lock_guard<std::mutex> lck (mutex);
			setFromAudioFile(audiofile);
		}else{
			setFromAudioFile(audiofile);
		}
		
	}else{
		ofLogError()<<"input file does not exists";
	}
	
	return bLoaded;   
	
}
//--------------------------------------------------------------
bool ofxSoundFile::loadAsync(std::string filepath){
	if(threadHelper == nullptr){
		path = filepath;

		threadHelper = make_shared<ThreadHelper>(*this);
		
		return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundFile::load(string filepath){
	path = filepath;
	return loadFile(false);
}

//--------------------------------------------------------------
bool ofxSoundFile::save(string path, const ofSoundBuffer &buff, int format){
	// check that we're writing a wav and complain if the file extension is wrong.
	ofFile f(path);
	if(ofToLower(f.getExtension())!="wav") {
		path += ".wav";
		ofLogWarning() << "Can only write wav files - will save file as " << path;
	}
	{
		fstream file(ofToDataPath(path).c_str(), ios::out | ios::binary);
		if(!file.is_open()) {
			ofLogError() << "Error opening sound file '" << path << "' for writing";
			return false;
		}
	}
	
	SndfileHandle sfile ;
	
	sfile = SndfileHandle (ofToDataPath(path, true), SFM_WRITE, SF_FORMAT_WAV | format, buff.getNumChannels(), buff.getSampleRate()) ;
	
	sfile.write (&buff.getBuffer()[0], buff.getBuffer().size());
	
	
	return true;
}


//--------------------------------------------------------------                  
//--------------------------------------------------------------
void ofxSoundFile::reset(){
	removeThreadHelper();	
	buffer.clear();
	bCompressed = false;
	bLoaded = false;
	duration = 0;
	numChannels = 0;
	sampleRate = 0;
	numSamples = 0;
	path = "";
}
//--------------------------------------------------------------
//void ofxSoundFile::close(){
//	reset();
//}
//--------------------------------------------------------------
const bool ofxSoundFile::isLoaded() const{
	return bLoaded;
}
//--------------------------------------------------------------
const unsigned int ofxSoundFile::getNumChannels() const{
	return numChannels; 
}
//--------------------------------------------------------------
const uint64_t ofxSoundFile::getDuration() const{
	return duration;
}
//--------------------------------------------------------------
const unsigned int ofxSoundFile::getSampleRate() const{
	return sampleRate;
}
//--------------------------------------------------------------
const uint64_t ofxSoundFile::getNumSamples() const{
	return numSamples;
}
//--------------------------------------------------------------
const bool ofxSoundFile::isCompressed() const{
	return bCompressed;
}
//--------------------------------------------------------------
const string ofxSoundFile::getPath() const{
	return path;
}
//--------------------------------------------------------------
ofSoundBuffer&  ofxSoundFile::getBuffer(){
	return buffer;
}
//--------------------------------------------------------------
const ofSoundBuffer&  ofxSoundFile::getBuffer() const{
	return buffer;
}
////--------------------------------------------------------------
//void ofxSoundFile::readTo(ofSoundBuffer & buf, uint64_t _samples){
//	if(_samples!=0){
//		// will read the requested number of samples
//		// clamp to the number of samples we actually have
//		_samples = min(_samples, getNumSamples());
//	}else{
//		_samples = getNumSamples();
//	}
//	buf.resize(_samples*getNumChannels());
//	buf.copyFrom(buffer.getBuffer(), _samples, getNumChannels(), getSampleRate());
//}
