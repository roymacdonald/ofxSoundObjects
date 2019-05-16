//
//  ofxSoundRecorderObject.cpp
//  example-soundRecorder
//
//  Created by Roy Macdonald on 5/16/19.
//
//

#include "ofxSoundRecorderObject.h"



ofxSoundRecorderObject::ofxSoundRecorderObject():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION){
}

void ofxSoundRecorderObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
	
	input.copyTo(output);

	//TO DO: implement all this as part of ofxSoundFile, and implement the write process on a different thread.
	
	if(recState == INIT_REC){
		recState = REC_ON;
		
		drwav_data_format format;
		format.container = drwav_container_riff;
		format.format = DR_WAVE_FORMAT_IEEE_FLOAT;   
		format.channels = input.getNumChannels();
		format.sampleRate = input.getSampleRate();
		format.bitsPerSample = 32;//sizeof(float) * 8;
		
		ofLogVerbose("ofxSoundRecorderObject::process")
		<< "                        new audiofile: " << filename << endl
		<< "                        channels:      " << format.channels << endl
		<< "                        sampleRate:    " << format.sampleRate << endl
		<< "                        bitsPerSample: " << format.bitsPerSample;
		
		wav_handle = drwav_open_file_write( filename.c_str(), &format);
	}else if(recState == DEINIT_REC){
		ofLogVerbose("ofxSoundRecorderObject::process") << "finished recording file " << filename;
		recState = IDLE;
		drwav_uninit(wav_handle);
		wav_handle  = NULL;
	}
	
	if( recState == REC_ON && wav_handle != NULL){
		drwav_uint64 samplesWritten = drwav_write_pcm_frames(wav_handle, input.getNumFrames(), input.getBuffer().data());
		if(samplesWritten != input.getNumFrames()){
			ofLogWarning("ofxSoundRecorderObject::process") << "samplesWritten " << samplesWritten << " != input.getNumFrames() " <<  input.getNumFrames() << endl;
		}
	}	
}


bool ofxSoundRecorderObject::isRecording(){
	std::lock_guard<std::mutex> lck (mutex);
	return recState == REC_ON;
}
void ofxSoundRecorderObject::startRecording(const std::string & filename){
	mutex.lock();
	if(recState == IDLE){
		recState = INIT_REC;
		if(filename.empty()){
			this->filename = ofToDataPath(ofGetTimestampString()+".wav", true);
		}else{
			this->filename = filename;
		}
	}
	mutex.unlock();
}
void ofxSoundRecorderObject::stopRecording(){
	mutex.lock();
	if(recState == REC_ON){
		recState = DEINIT_REC;
	}
	mutex.unlock();
}
const std::string& ofxSoundRecorderObject::getFileName(){
	std::lock_guard<std::mutex> lck (mutex);
	return filename;
}
