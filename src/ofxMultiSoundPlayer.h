/*
 * ofxMultiPlayerObject.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofBaseTypes.h"
#include "ofConstants.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"
#include "ofSoundUtils.h"
#include "ofSoundStream.h"
#include "ofEvents.h"
#include "ofxSoundObjects.h"


#include "ofxSoundMixer.h"
#include "ofxSingleSoundPlayer.h"



class ofxMultiSoundPlayer:  public ofxBaseSoundPlayer {
public:
	ofxMultiSoundPlayer();
	
	virtual bool load(const ofSoundBuffer& loadBuffer, const std::string& name) override;
	
	virtual bool load(std::filesystem::path filePath, bool stream = false) override;
	virtual bool loadAsync(std::filesystem::path filePath, bool bAutoplay) override;
	virtual void unload() override;
	int play();// when play is called and multiplay enabled a new playing instance is setup and it's index returned. If it is not possible to play -1 is returned;
	void stop(size_t index =0);
	
	virtual size_t getNumChannels() override;
	
	void setVolume(float vol, int index =-1 );
	void setPan(float vol, int index =-1 ); // -1 = left, 1 = right
	
	void setSpeed(float spd, int index =-1);
	void setPaused(bool bP, int index =-1 );
	void setLoop(bool bLp, int index =-1 );
	void setMultiPlay(bool bMp);
	void setPosition(float pct, size_t index =0 ); // pct: 0 = start, 1 = end;
	void setPositionMS(int ms, size_t index =0 );

	float getPosition(size_t index ) const;
	int	  getPositionMS(size_t index ) const;
	bool  isPlaying(int index ) const;
	float getSpeed(size_t index ) const;
	float getRelativeSpeed(size_t index ) const;
	float getPan(size_t index ) const;
	virtual bool  isLoaded() const override;
	float getVolume(int index ) const;
	
	virtual size_t getNumFrames() const override;
	
	bool  isLooping(size_t index ) const;
	OF_DEPRECATED_MSG("Use isLooping() instead", bool  getIsLooping(size_t index =-1) const);
	virtual unsigned long getDurationMS() override;

	const ofSoundBuffer & getCurrentBuffer() const;
	const ofSoundBuffer & getBuffer() const;
    ofEvent<size_t> endEvent;
	ofEvent<void>& getAsyncLoadEndEvent();

	const ofxSoundFile& getSoundFile() const;
	ofxSoundFile& getSoundFile();


	ofParameter<float>volume;
	bool canPlayInstance();
	void drawDebug(float x, float y);
	size_t getNumInstances() { return instances.size(); }
	
	virtual std::string getFilePath() const override;
	
	
	
	///overrides of base class to make it easier to check this values 
	virtual float getPosition() const override { return getPosition(0);}
	virtual int	  getPositionMS() const override { return getPositionMS(0);}
	virtual bool  isPlaying() const override { return isPlaying(-1);}
	virtual float getSpeed() const override { return getSpeed(0);}
	virtual float getRelativeSpeed() const override { return getRelativeSpeed(0);}
	virtual float getPan() const override { return getPan(0);}
	virtual float getVolume() const override { return getVolume(-1);}
	virtual bool  isLooping() const override { return isLooping(-1);}
	virtual bool isReplaying() const override;
	virtual int getSourceSampleRate() const override;
	
	ofxSingleSoundPlayer& getPlayInstance(size_t index);
	const ofxSingleSoundPlayer& getPlayInstance(size_t index) const;
	size_t getNumPlayInstances() const;
	
    
    ///\brief overriden function. This is what gets called in order to process new audio data
    ///you shouldn't need to use it unless you really know what it does.
    virtual void audioOut(ofSoundBuffer& outputBuffer) override;
    
private:
	
	vector<unique_ptr<ofxSingleSoundPlayer>> instances;
	
	
	
//	std::atomic<bool> bStreaming;
	std::atomic<bool> bMultiplay;
	std::atomic<bool> bIsPlayingAny;
	
	bool bDefaultlLooping = false;

	void setNumInstances(const size_t & num);
	
	void checkPaused();
    
    void updateInstance(std::function<void(ofxSingleSoundPlayer* inst)> func, int index, string methodName);
	
	mutable ofMutex instacesMutex;

	ofxSoundMixer _mixer;

	ofEventListeners endEventListeners;
	
	void onInstanceEnd(size_t&);
};

typedef ofxMultiSoundPlayer ofxSoundPlayerObject;


