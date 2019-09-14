//
//  NDISenderObject.h
//  example-ofxNDI
//
//  Created by Roy Macdonald on 9/14/19.
//
//
#pragma once
#include "ofxNDISender.h"
#include "ofxNDISendStream.h"
#include "ofxSoundObject.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"

class ofxNDISenderSoundObject : public ofxSoundObject{

public:
	ofxNDISenderSoundObject();
	
	void setup(const std::string& name, const std::string &group="");
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	void setMuteOutput(bool bMute);
	bool isMuteOutput();
	
private:
	ofxNDISender sender_;
	ofxNDISendAudio audio_;
	std::atomic<bool> bMute;
};


//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
	void setup(const std::string& name_or_url,
			   const std::string &group=""
			   ,uint32_t waittime_ms=1000,
			   ofxNDI::Location location= ofxNDI::Location::BOTH,
			   const std::vector<std::string> extra_ips={});
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	std::string getSourceName();
	std::string getSourceUrl();
	bool isConnected();
private:
	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;
	bool bAudioNeedsSetup = false;
	ofxNDI::Source source;
};


















