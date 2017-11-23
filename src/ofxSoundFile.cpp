 /*
 * ofxSoundFile.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxSoundFile.h"
#include "ofLog.h"
#include "ofUtils.h"
#include <algorithm>

//--------------------------------------------------------------
bool ofxLoadSound(ofSoundBuffer &buff, std::string path){
    ofxSoundFile sf(path);
    if(sf.isLoaded()){
        sf.readTo(buff);
        return true;
    }else{
        return false;
    }
}
bool ofxSaveSound(const ofSoundBuffer &buff,  std::string path){
	ofxSoundFile soundFile;
	return soundFile.save(path, buff);
}

//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile() {
	close();
}

//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile(std::string path) {
    ofxSoundFile();
    load(path);
}

//--------------------------------------------------------------
ofxSoundFile::~ofxSoundFile() {
	close();
}

//--------------------------------------------------------------
bool ofxSoundFile::load(std::string _path){
 	path = ofToDataPath(_path);
	sndFileHandle = SndfileHandle(path);
	if (sndFileHandle) {

		if (sndFileHandle.format() == SF_FORMAT_FLOAT || sndFileHandle.format() == SF_FORMAT_DOUBLE) {
			ofLogNotice() << "calculating scale ";
			sf_command(sndFileHandle.rawHandle(), SFC_CALC_SIGNAL_MAX, &scale, sizeof(scale));
			if (scale < 1e-10) {
				scale = 1.0;
			}else {
				scale = 32700.0 / scale;
			}
		}

		int samples = sndFileHandle.frames();
		int samplerate = sndFileHandle.samplerate();
		duration = 0;
		if(samplerate != 0) { // prevent div by zero if file doesn't open.
			duration = float(samples) / float(samplerate);
			std::cout << "ofxSoundFile loaded! " << ofFilePath::getBaseName(path) << std::endl;
		}
		return true;
	}
	ofLogError() << "ofxSoundFile: couldnt read " + path;
	return false;
}

//--------------------------------------------------------------
bool ofxSoundFile::save(std::string path, const ofSoundBuffer &buff){
	
    //TO DO !!!!!
    
    
	return false;
}
//--------------------------------------------------------------
void ofxSoundFile::close(){
	samples_read = 0;
	duration = 0; //in secs
}

//--------------------------------------------------------------
const bool ofxSoundFile::isLoaded() const{
    return (bool)sndFileHandle;
}

//--------------------------------------------------------------
const int ofxSoundFile::getNumChannels() const{
	if (sndFileHandle) {
		return sndFileHandle.channels();
	}
	return 0;
}

//--------------------------------------------------------------
const unsigned long ofxSoundFile::getDuration() const{
	return duration*1000;
}

//--------------------------------------------------------------
const int ofxSoundFile::getSampleRate() const{
	if (sndFileHandle) {
		return sndFileHandle.samplerate();
	}
	return 0;
}

//--------------------------------------------------------------
const unsigned long ofxSoundFile::getNumSamples() const{
	if (sndFileHandle) {
		return sndFileHandle.frames();
	}
	return 0;
}
//--------------------------------------------------------------
const std::string ofxSoundFile::getPath() const{
    return path;
}
//--------------------------------------------------------------
bool ofxSoundFile::readTo(ofSoundBuffer & buffer, long long _samples){
	if (sndFileHandle) {
		if (sndFileHandle.channels() > 0 && sndFileHandle.frames() > 0) {
			buffer.setNumChannels(sndFileHandle.channels());
			buffer.setSampleRate(sndFileHandle.samplerate());
			if (_samples != 0) {
				// will read the requested number of samples
				// clamp to the number of samples we actually have
				_samples = std::min(_samples, sndFileHandle.frames());
			}
			else {
				_samples = sndFileHandle.frames();
			}
			_samples *= sndFileHandle.channels();
			if (buffer.size() != _samples) {
				buffer.resize(_samples);
			}
			samples_read = sndFileHandle.read(&buffer[0], buffer.size());
			return 	samples_read;
		}
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundFile::seekTo(long long sample){
	sample = std::min(sndFileHandle.frames(),sample);
	if (sndFileHandle) sndFileHandle.seek(sample, SEEK_SET);
	else return false;
	
	return true; //TODO: check errors
}
//--------------------------------------------------------------