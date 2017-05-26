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
bool ofxLoadSound(ofSoundBuffer &buff, string path){
    ofxSoundFile sf(path);
    if(sf.isLoaded()){
        sf.readTo(buff);
        return true;
    }else{
        return false;
    }
}
bool ofxSaveSound(const ofSoundBuffer &buff,  string path){
	ofxSoundFile soundFile;
	return soundFile.save(path, buff);
}

//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile() {
	close();
}

//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile(string path) {
    ofxSoundFile();
    load(path);
}

//--------------------------------------------------------------
ofxSoundFile::~ofxSoundFile() {
	close();
}

//--------------------------------------------------------------
bool ofxSoundFile::load(string _path){
 	path = ofToDataPath(_path);
	sndFileHandle = SndfileHandle(path);
	if (sndFileHandle) {
//		cout << sndFileHandle.getString() << endl;

		//SF_INFO sfInfo;
		//ofLogNotice() << "opening descriptor " << path;
		//sndFile = sf_open(path.c_str(), SFM_READ, &sfInfo);
		//if (!sndFile) {
		//	ofLogError() << "ofxSoundFile: couldnt read " + path;
		//	bLoaded = false;
		//	return false;
		//}
		//ofLogNotice() << "odescriptor opened ";

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
			cout << "ofxSoundFile loaded! " << ofFilePath::getBaseName(path);
		//	cout << " samples: " << samples;
		//	cout << " channels: " << channels;
		//	cout << " samplerate: " << samplerate << endl;
		}
		//bLoaded = true;
		return true;
	}
	ofLogError() << "ofxSoundFile: couldnt read " + path;
	return false;
}

//--------------------------------------------------------------
bool ofxSoundFile::save(string path, const ofSoundBuffer &buff){
	// check that we're writing a wav and complain if the file extension is wrong.
	//ofFile f(path);
	//if(ofToLower(f.getExtension())!="wav") {
	//	path += ".wav";
	//	ofLogWarning() << "Can only write wav files - will save file as " << path;
	//}
	//
	//fstream file(ofToDataPath(path).c_str(), ios::out | ios::binary);
	//if(!file.is_open()) {
	//	ofLogError() << "Error opening sound file '" << path << "' for writing";
	//	return false;
	//}
	//
	//// write a wav header
	//short myFormat = 1; // for pcm
	//int mySubChunk1Size = 16;
	//int bitsPerSample = 16; // assume 16 bit pcm
	//int myByteRate = buff.getSampleRate() * buff.getNumChannels() * bitsPerSample/8;
	//short myBlockAlign = buff.getNumChannels() * bitsPerSample/8;
	//int myChunkSize = 36 + buff.size()*bitsPerSample/8;
	//int myDataSize = buff.size()*bitsPerSample/8;
	//int channels = buff.getNumChannels();
	//int samplerate = buff.getSampleRate();
	//
	//file.seekp (0, ios::beg);
	//file.write ("RIFF", 4);
	//file.write ((char*) &myChunkSize, 4);
	//file.write ("WAVE", 4);
	//file.write ("fmt ", 4);
	//file.write ((char*) &mySubChunk1Size, 4);
	//file.write ((char*) &myFormat, 2); // should be 1 for PCM
	//file.write ((char*) &channels, 2); // # channels (1 or 2)
	//file.write ((char*) &samplerate, 4); // 44100
	//file.write ((char*) &myByteRate, 4); //
	//file.write ((char*) &myBlockAlign, 2);
	//file.write ((char*) &bitsPerSample, 2); //16
	//file.write ("data", 4);
	//file.write ((char*) &myDataSize, 4);
	//
	//// write the wav file per the wav file format, 4096 bytes of data at a time.
	//#define WRITE_BUFF_SIZE 4096
	//
	//short writeBuff[WRITE_BUFF_SIZE];
	//int pos = 0;
	//while(pos<buff.size()) {
	//	int len = MIN(WRITE_BUFF_SIZE, buff.size()-pos);
	//	for(int i = 0; i < len; i++) {
	//		writeBuff[i] = (int)(buff[pos]*32767.f);
	//		pos++;
	//	}
	//	file.write((char*)writeBuff, len*bitsPerSample/8);
	//}
	//
	//file.close();
	//return true;
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
const string ofxSoundFile::getPath() const{
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
	if (sndFileHandle) sndFileHandle.seek(sample, SEEK_SET);//sf_seek(sndFile,sample,SEEK_SET);
	else return false;
	
	return true; //TODO: check errors
}
////--------------------------------------------------------------
//bool ofxSoundFile::sfReadFile(ofSoundBuffer & buffer){
//	//samples_read = sf_read_float (sndFile, &buffer[0], buffer.size());
//	//if(samples_read<(int)buffer.size()){
//	//	ofLogError() <<  "ofxSoundFile: couldnt read " << path;
//	//	return false;
//	//}
//	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE){
//		for (int i = 0 ; i < int(buffer.size()) ; i++){
//			buffer[i] *= scale ;
//		}
//	}
//	return true;
//}
//--------------------------------------------------------------