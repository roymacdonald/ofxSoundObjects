/*
 * ofxSoundPlayerObject.h
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
#include "ofxSimpleSoundPlayer.h"



class ofxSoundPlayerObject:  public ofxSoundObject {
public:
	ofxSoundPlayerObject();
	
	bool load(const ofSoundBuffer& loadBuffer, const std::string& name);
	
	bool load(std::filesystem::path filePath, bool stream = false);
	bool loadAsync(std::filesystem::path filePath, bool bAutoplay);
	void unload();
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

	float getPosition(size_t index =0) const;
	int	  getPositionMS(size_t index =0) const;
	bool  isPlaying(int index = -1) const;
	float getSpeed(size_t index =0) const;
	float getRelativeSpeed(size_t index =0) const;
	float getPan(size_t index =0) const;
	bool  isLoaded() const;
	float getVolume(int index =-1) const;
	
	bool  isLooping(size_t index =-1) const;
	OF_DEPRECATED_MSG("Use isLooping() instead", bool  getIsLooping(size_t index =-1) const);
	unsigned long getDurationMS();

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
	
	const std::string getFilePath() const;
	
	vector<unique_ptr<ofxSimpleSoundPlayer>> instances;
	
private:
	
	virtual void audioOut(ofSoundBuffer& outputBuffer) override;
	
//	std::atomic<bool> bStreaming;
	std::atomic<bool> bMultiplay;
	std::atomic<bool> bIsPlayingAny;
	
	bool bDefaultlLooping = false;

	void setNumInstances(const size_t & num);
	
	void checkPaused();
    
    void updateInstance(std::function<void(ofxSimpleSoundPlayer* inst)> func, int index, string methodName);
	
	mutable ofMutex instacesMutex;

	ofxSoundMixer _mixer;
	
};


