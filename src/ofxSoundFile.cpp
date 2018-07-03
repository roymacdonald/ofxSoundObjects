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
		audiofile.load( filepath );
		result = audiofile.loaded();
		if (!result){
			ofLogError()<<"error loading file, double check the file path";
		}else{
			bCompressed = (ofFilePath::getFileExt(ofToLower(filepath)) == "mp3");
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
//	channels = 1;
	audiofile.free();
	duration = 0; //in secs
//	samplerate = 0;
//	samples = 0;
}

//--------------------------------------------------------------
const bool ofxSoundFile::isLoaded() const{
	return audiofile.loaded();
//    return bLoaded;
}

//--------------------------------------------------------------
const unsigned int ofxSoundFile::getNumChannels() const{
	return audiofile.channels();
	//	return channels;
}

//--------------------------------------------------------------
const uint64_t ofxSoundFile::getDuration() const{
	return duration*1000;
}

//--------------------------------------------------------------
const unsigned int ofxSoundFile::getSampleRate() const{
	return audiofile.samplerate();
//	return samplerate;
}

//--------------------------------------------------------------
const uint64_t ofxSoundFile::getNumSamples() const{
	return audiofile.length();
//    return samples;
}

//--------------------------------------------------------------
const bool ofxSoundFile::isCompressed() const{
    return bCompressed;
}

//--------------------------------------------------------------
const string ofxSoundFile::getPath() const{
	return audiofile.path();
//    return path;
}

//--------------------------------------------------------------
bool ofxSoundFile::readTo(ofSoundBuffer & buffer, uint64_t _samples){
//	buffer.setNumChannels(getNumChannels());
//	buffer.setSampleRate(getSampleRate());
	if(_samples!=0){
		// will read the requested number of samples
		// clamp to the number of samples we actually have
		_samples = min(_samples, getNumSamples());
	}else{
		_samples = getNumSamples();
	}
	buffer.resize(_samples*getNumChannels());
	buffer.copyFrom(audiofile.data(), _samples, getNumChannels(), getSampleRate());
//#ifdef OF_USING_SNDFILE
//	else if (sndFile){
//		// will read entire file
//		buffer.resize(samples);
//	}
//#endif
//#ifdef OF_USING_LAD
//	else if (audioDecoder) {
//		// will read entire file
//		buffer.resize(samples);
//	}
//#endif
//#ifdef OF_USING_MPG123
//	else if(mp3File){
//		buffer.clear();
//	}
//#endif
//	
//#ifdef OF_USING_SNDFILE
//	if(sndFile) return sfReadFile(buffer);
//#elif defined( OF_USING_MPG123 )
//	if(mp3File) return mpg123ReadFile(buffer);
//#elif defined( OF_USING_LAD )
//	if(audioDecoder) return ladReadFile(buffer);
//#endif
//	return false;
}
//--------------------------------------------------------------
//bool ofxSoundFile::seekTo(unsigned int sample){
//	sample = min(samples,sample);
//#ifdef OF_USING_SNDFILE
//	if(sndFile) sf_seek(sndFile,sample,SEEK_SET);
//#endif
//#ifdef OF_USING_LAD
//	if(audioDecoder) audioDecoder->seek(sample);
//#endif
//	
//#ifdef OF_USING_MPG123
//	else if(mp3File) mpg123_seek(mp3File,sample,SEEK_SET);
//#endif
//	else return false;
//	
//	return true; //TODO: check errors
//}
//--------------------------------------------------------------
//#ifdef OF_USING_SNDFILE
//bool ofxSoundFile::sfReadFile(ofSoundBuffer & buffer){
//	samples_read = sf_read_float (sndFile, &buffer[0], buffer.size());
//	/*if(samples_read<(int)buffer.size()){
//		ofLogError() <<  "ofxSoundFile: couldnt read " << path;
//		return false;
//	}*/
//	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE){
//		for (int i = 0 ; i < int(buffer.size()) ; i++){
//			buffer[i] *= scale ;
//		}
//	}
//	return true;
//}
//#endif
////--------------------------------------------------------------
//#ifdef OF_USING_LAD
//bool ofxSoundFile::ladReadFile(ofSoundBuffer &buffer){
//	
//	int samplesRead = audioDecoder->read( buffer.size(), &buffer[0] );
//	return samplesRead;
//}
//#endif
//
//

