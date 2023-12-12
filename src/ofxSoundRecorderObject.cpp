//
//  ofxSoundRecorderObject.cpp
//  example-soundRecorder
//
//  Created by Roy Macdonald on 5/16/19.
//
//

#include "ofxSoundRecorderObject.h"


//--------------------------------------------------------------
ofxSoundRecorderObject::ofxSoundRecorderObject():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION), recState(IDLE){
	setName ("Recorder");
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
	startThread();
#endif
}
//--------------------------------------------------------------
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
ofxSoundRecorderObject::~ofxSoundRecorderObject(){
	stopRecording();
	writeChannel.close();
	if(isThreadRunning()) waitForThread(true);
}
#endif
//--------------------------------------------------------------
void ofxSoundRecorderObject::write(ofSoundBuffer& input){
	//TO DO: implement all this as part of ofxSoundFile, and implement the write process on a different thread.
	if(recState == INIT_REC){
		recState = REC_ON;
		
		drwav_data_format format;
		format.container = drwav_container_riff;
		format.format = DR_WAVE_FORMAT_IEEE_FLOAT;   
		format.channels = input.getNumChannels();
		format.sampleRate = input.getSampleRate();
		format.bitsPerSample = 32;
		
		ofLogVerbose("ofxSoundRecorderObject::process")
		<< "                        new audiofile: " << filenameBuffer << endl
		<< "                        channels:      " << format.channels << endl
		<< "                        sampleRate:    " << format.sampleRate << endl
		<< "                        bitsPerSample: " << format.bitsPerSample;
		
		wav_handle = drwav_open_file_write( filenameBuffer.c_str(), &format);
//		input.copyTo(internalRecordingBuffer);
		internalRecordingBuffer = input;
		cout << "input.getNumChannels : " << input.getNumChannels() <<endl;
		cout << "internalRecordingBuffer.getNumChannels : " << internalRecordingBuffer.getNumChannels() <<endl;
	}else if(recState == DEINIT_REC){
		ofLogVerbose("ofxSoundRecorderObject::process") << "finished recording file " << filenameBuffer;
		drwav_uninit(wav_handle);
		wav_handle  = NULL;
		recState = IDLE;
		if(bRecordToRam){
			std::lock_guard<std::mutex> lck (mutex);
			internalRecordingBuffer.swap(recordingBuffer);
			bRecordToRam = false;
		}
		
		ofNotifyEvent(recordingEndEvent, filenameBuffer);
	}
	
	if( recState == REC_ON && wav_handle != NULL){
		if(bRecordToRam){
//			recordingBuffer.clear()
			internalRecordingBuffer.append(input);
		}
		drwav_uint64 samplesWritten = drwav_write_pcm_frames(wav_handle, input.getNumFrames(), input.getBuffer().data());
		if(samplesWritten != input.getNumFrames()){
			ofLogWarning("ofxSoundRecorderObject::process") << "samplesWritten " << samplesWritten << " != input.getNumFrames() " <<  input.getNumFrames() << endl;
		}
	}	
}
//--------------------------------------------------------------
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
void ofxSoundRecorderObject::threadedFunction(){
	ofSoundBuffer buffer;
	while(writeChannel.receive(buffer)){
		write(buffer);
	}
}
#endif
//--------------------------------------------------------------
void ofxSoundRecorderObject::audioOut(ofSoundBuffer &output){
    if(ofxSoundUtils::checkBuffers(output, getBuffer(), false))
    {
        auto& buffer = getBuffer();
        ofxSoundInput* obj = (ofxSoundInput*)getSignalSourceObject();
        int buffer_size = obj->getInputStream()->getBufferSize() * obj->getInputStream()->getNumInputChannels();
        buffer.resize(buffer_size);
        buffer.setNumChannels(obj->getInputStream()->getNumInputChannels());
        buffer.setSampleRate(obj->getInputStream()->getSampleRate());
    }
    if(inputObject != NULL)
    {
        if(isBypassed())
        {
            inputObject->audioOut(output);
        }
        else
        {
            inputObject->audioOut(getBuffer());
        }
    }
    if(!isBypassed())
    {
        process(getBuffer(), output);
    }
}

//--------------------------------------------------------------
void ofxSoundRecorderObject::process(ofSoundBuffer &input, ofSoundBuffer &output){
    input.copyTo(output, output.getNumFrames(), output.getNumChannels(), 0);
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
    if(recState != IDLE){
        writeChannel.send(input);
    }
#else
    write(input);
#endif
}
//--------------------------------------------------------------
bool ofxSoundRecorderObject::isRecording(){
	return recState != IDLE;
}
//--------------------------------------------------------------
void ofxSoundRecorderObject::startRecording(const std::string & filename, bool recordToRam){

	
	if(recState == IDLE){
		if(filename.empty()){
			this->filename = ofToDataPath(ofGetTimestampString()+".wav", true);
		}else{
			this->filename = filename;
		}
		{
			std::lock_guard<std::mutex> lck (mutex);
			bRecordToRam = recordToRam;
			if(bRecordToRam){
				recordingBuffer.clear();
				internalRecordingBuffer.clear();
			}
			this->filenameBuffer = this->filename;
			recStartTime = ofGetElapsedTimef();
		}
		recState = INIT_REC;
	}else{
		ofLogWarning("ofxSoundRecorderObject::startRecording")<< "can not start recording when there is already another recording happening. Please stop this recording before beggining a new one";
	}
}
//--------------------------------------------------------------
void ofxSoundRecorderObject::stopRecording(){
	if(recState == REC_ON){
		recState = DEINIT_REC;
	}
}
//--------------------------------------------------------------
float ofxSoundRecorderObject::getRecordingElapsedTime(){
	if(isRecording()){
		return  ofGetElapsedTimef() - recStartTime;
	}
	return 0.0f;

}
//--------------------------------------------------------------
const std::string& ofxSoundRecorderObject::getFileName(){
	return filename;
}
//--------------------------------------------------------------
std::string ofxSoundRecorderObject::getRecStateString(){
	RecState state;
	state = recState;

	if(state == IDLE)return "IDLE";
	if(state == INIT_REC)return "INIT_REC";
	if(state == REC_ON)return "RECORDING";
	if(state == DEINIT_REC)return "DEINIT_REC";
	return "";
}
//--------------------------------------------------------------
const ofSoundBuffer& ofxSoundRecorderObject::getRecordedBuffer(){
	return recordingBuffer;
}
