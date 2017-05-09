/*
 * ofxBasicSoundPlayer.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofBaseSoundPlayer.h"
#include "ofBaseTypes.h"
#include "ofConstants.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"

#include "ofSoundStream.h"
#include "ofEvents.h"
#include "ofxSoundObjects.h"

class ofxBasicSoundPlayer: public ofBaseSoundPlayer, public ofxSoundObject {//public ofBaseSoundOutput {
public:
	ofxBasicSoundPlayer();
	virtual ~ofxBasicSoundPlayer();
    bool load(string filePath, bool stream = false);
	bool load(std::filesystem::path filePath, bool stream = false);
	void unload();
	void play();
	void stop();

	void setVolume(float vol);
	void setPan(float vol); // -1 = left, 1 = right
	void setSpeed(float spd);
	void setPaused(bool bP);
	void setLoop(bool bLp);
	void setMultiPlay(bool bMp);
	void setPosition(float pct); // 0 = start, 1 = end;
	void setPositionMS(int ms);
	
	float getPosition() const;
	int getPositionMS() const;
	bool isPlaying() const;
	float getSpeed() const;
	float getPan() const;
	bool isLoaded() const;
	float getVolume() const;
	bool getIsLooping() const;
	unsigned long getDurationMS();

	ofSoundBuffer & getCurrentBuffer();

	static void setMaxSoundsTotal(int max);
	static void setMaxSoundsPerPlayer(int max);
	void setMaxSounds(int max);

	ofEvent<ofSoundBuffer> newBufferE;

    const ofxSoundFile& getSoundFile() const {return soundFile;}
    
private:
	void audioOutBuffersChanged( int nFrames, int nChannels, int sampleRate );
	void audioOut(ofSoundBuffer& outputBuffer);
	void updatePositions(int numFrames);
	
	int playerSampleRate, playerNumFrames, playerNumChannels;
	
	static int maxSoundsTotal;
	static int maxSoundsPerPlayer;
	int maxSounds;
	float volume;
	ofSoundBuffer buffer, resampledBuffer;
	ofxSoundFile soundFile;
	bool streaming;
	bool bIsPlaying;
	bool multiplay;
	bool bIsLoaded;
	bool loop;
	float speed;
	float pan;
	vector<float> relativeSpeed;
	vector<unsigned int> positions;
	vector<float> volumesLeft;
	vector<float> volumesRight;	
};


