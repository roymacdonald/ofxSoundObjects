//
//  NDISenderObject.h
//  example-ofxNDI
//
//  Created by Roy Macdonald on 9/14/19.
//
//
#pragma once
#include "ofxSoundObjectsConstants.h"
#include "ofxSoundObject.h"
#include <atomic>
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
#include "ofxNDISender.h"
#include "ofxNDISendStream.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"
#endif


class ofxNDISenderSoundObject : public ofxSoundObject{

public:
	ofxNDISenderSoundObject();
	
	void setup(const std::string& name, const std::string &group="");
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	void setMuteOutput(bool bMute);
	bool isMuteOutput();
	
private:
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	ofxNDISender sender_;
	ofxNDISendAudio audio_;
#endif
	std::atomic<bool> bMute;
};


//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
	void setup(const std::string& name_or_url,
			   const std::string &group=""
#ifndef OFX_SOUND_OBJECTS_USE_OFX_NDI
	);
#else
			   ,uint32_t waittime_ms=1000,
			   ofxNDI::Location location= ofxNDI::Location::BOTH,
			   const std::vector<std::string> extra_ips={});
#endif
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	std::string getSourceName();
	std::string getSourceUrl();
	bool isConnected();
private:
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;
	ofxNDI::Source source;
	bool bAudioNeedsSetup = false;
#endif
};


















