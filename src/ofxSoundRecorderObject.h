//
//  ofxSoundRecorderObject.h
//  example-soundRecorder
//
//  Created by Roy Macdonald on 5/16/19.
//
//

#pragma once
//#include "ofBaseTypes.h"
//#include "ofConstants.h"
#include "ofSoundBuffer.h"
//#include "ofThread.h"
#include "ofSoundUtils.h"
//#include "ofSoundStream.h"

#include "ofxSoundObjects.h"
#include "dr_wav.h"
class ofxSoundRecorderObject:  public ofxSoundObject {
public:
	ofxSoundRecorderObject();
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output);
	
	
	bool isRecording();
	void startRecording(const std::string & filename);
	void stopRecording();
	
	const std::string& getFileName();
	
private:
	
	
	drwav* wav_handle = NULL;
	
	enum RecState{
		IDLE = 0,
		INIT_REC,
		REC_ON,
		DEINIT_REC
	}recState = IDLE;

	std::string filename;
	ofMutex mutex;
};
