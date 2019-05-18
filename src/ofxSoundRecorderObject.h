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
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	bool isRecording();
	void startRecording(const std::string & filename);
	void stopRecording();
	
	
	const std::string& getFileName();
	
	
	std::string getRecStateString();
	
	
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
};
