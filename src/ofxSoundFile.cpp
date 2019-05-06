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
        sf.readTo(buff);
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
    bCompressed = false;
//    bLoaded = false;	
	close();
}

//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile(string path) {
    ofxSoundFile();
    load(path);
}


//--------------------------------------------------------------
bool ofxSoundFile::load(string filepath){
// 	path = ofToDataPath(_path);
	bool result = false;

	if( ofFile::doesFileExist( filepath ) ){
		audiofile.setVerbose(true);
		audiofile.load( filepath );
		result = audiofile.loaded();
		if (!result){
			ofLogError("ofxSoundFile::load")<<"error loading file, double check the file path";
		}else{
			 
			bCompressed = (ofFilePath::getFileExt(ofToLower(filepath)) == "mp3");
			ofLogVerbose("ofxSoundFile::load") << "file loaded. is mp3 : "<< (bCompressed?"YES":"NO");  
		}
		
	}else{
		ofLogError()<<"input file does not exists";
	}

	
	if(result) { // prevent div by zero if file doesn't open.
		duration = float(audiofile.length()) / float(audiofile.samplerate());
	}
	return result;   
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
void ofxSoundFile::close(){
	audiofile.free();
	duration = 0; //in secs
}

//--------------------------------------------------------------
const bool ofxSoundFile::isLoaded() const{
	return audiofile.loaded();
}

//--------------------------------------------------------------
const unsigned int ofxSoundFile::getNumChannels() const{
	return audiofile.channels();
}

//--------------------------------------------------------------
const uint64_t ofxSoundFile::getDuration() const{
	return duration*1000;
}

//--------------------------------------------------------------
const unsigned int ofxSoundFile::getSampleRate() const{
	return audiofile.samplerate();
}

//--------------------------------------------------------------
const uint64_t ofxSoundFile::getNumSamples() const{
	return audiofile.length();
}

//--------------------------------------------------------------
const bool ofxSoundFile::isCompressed() const{
    return bCompressed;
}

//--------------------------------------------------------------
const string ofxSoundFile::getPath() const{
	return audiofile.path();
}
//--------------------------------------------------------------
void ofxSoundFile::readTo(ofSoundBuffer & buffer, uint64_t _samples){
	if(_samples!=0){
		// will read the requested number of samples
		// clamp to the number of samples we actually have
		_samples = min(_samples, getNumSamples());
	}else{
		_samples = getNumSamples();
	}
	buffer.resize(_samples*getNumChannels());
	buffer.copyFrom(audiofile.data(), _samples, getNumChannels(), getSampleRate());
}
