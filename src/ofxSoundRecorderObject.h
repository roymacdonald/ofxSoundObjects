//
//  ofxSoundRecorderObject.h
//  example-soundRecorder
//
//  Created by Roy Macdonald on 5/16/19.
//
//

#pragma once

#include "ofxSoundObjects.h"
#include "ofxSoundObjectsConstants.h"
#include "dr_wav.h"

#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
class ofxSoundRecorderObject:   public ofThread, public ofxSoundObject  {
#else
class ofxSoundRecorderObject:  public ofxSoundObject{
#endif
public:
	ofxSoundRecorderObject();
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
	virtual ~ofxSoundRecorderObject();
#endif
    virtual void audioOut(ofSoundBuffer &output) override;
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	/// This will return true if it is recording. It is thread safe. When recording if you call stopRecording(), isRecording() will return false only when the recorded file has been closed. As the recording task happens on a thread different to the main thread, if you call stopRecording() and then immediatelly check isRecording() it is possible that you might get true, as the recorded file might not be closed yet.
	bool isRecording();
	/// \brief start recording a file to disk.
	/// \param filepath the file path where the recording wil get stored.
	/// \param recordToRam if set to true it will also keep a buffer of the recording in memory. It is useful for sampling, when you want to playback what you've recorded immediately after finishing recording.
	void startRecording(const std::string & filepath, bool recordToRam = false);
	void stopRecording();
	
	
	
	const std::string& getFileName();
	
	
	std::string getRecStateString();
	
	
	/// \brief returns the sound buffer with the recently recorded data.
	/// It will only return a valid buffer if not currently recording.
	const ofSoundBuffer& getRecordedBuffer();
	
	
	// This event gets triggered when the recording has finished and the file has been closed. 
	// It will get triggered from either the audio thread or the recorder's own thread (if OFX_SOUND_ENABLE_THREADED_RECORDER has been defined in ofxSoundObjectsConstants.h), which in any case are not the main thread so you should be careful about the callback function. 
	ofEvent<std::string> recordingEndEvent;
	
	float getRecordingElapsedTime();
	
protected:
	
#ifdef OFX_SOUND_ENABLE_THREADED_RECORDER
	virtual void threadedFunction() override;
	ofThreadChannel<ofSoundBuffer> writeChannel;
#endif
	
	void write(ofSoundBuffer& input);

private:
	
	
	drwav* wav_handle = NULL;
	
	enum RecState{
		IDLE = 0,
		INIT_REC,
		REC_ON,
		DEINIT_REC
	};
	std::atomic<RecState> recState;

	std::string filename, filenameBuffer;
	ofMutex mutex;
	float recStartTime = 0.0f;
	
	ofSoundBuffer recordingBuffer, internalRecordingBuffer;
	
	bool bRecordToRam = false;
	
};
