/*
 * ofxBasicSoundPlayer.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofBaseTypes.h"
#include "ofConstants.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"

#include "ofSoundStream.h"
#include "ofEvents.h"
#include "ofxSoundObjects.h"

class ofxBasicSoundPlayer:  public ofxSoundObject {//public ofBaseSoundOutput {
public:
	ofxBasicSoundPlayer();
	virtual ~ofxBasicSoundPlayer();
	bool load(std::filesystem::path filePath, bool stream = false);
	void unload();
	size_t play();// when play is called and multiplay enabled a new playing instance is setup and it's index returned;
	void stop(size_t index =0);
	
	void setVolume(float vol, size_t index =0 );
	void setPan(float vol, size_t index =0 ); // -1 = left, 1 = right
	void setSpeed(float spd, size_t index =0 );
	void setPaused(bool bP, size_t index =0 );
	void setLoop(bool bLp, size_t index =0 );
	void setMultiPlay(bool bMp);
	void setPosition(float pct, size_t index =0 ); // 0 = start, 1 = end;
	void setPositionMS(int ms, size_t index =0 );

	float getPosition(size_t index =0) const;
	int getPositionMS(size_t index =0) const;
	bool isPlaying() const;
	bool isPlaying(size_t index) const;
	float getSpeed(size_t index =0) const;
	float getPan(size_t index =0) const;
	bool isLoaded() const;
	float getVolume(size_t index =0) const;
	bool getIsLooping(size_t index =0) const;
	unsigned long getDurationMS();

	ofSoundBuffer & getCurrentBuffer();

	static void setMaxSoundsTotal(int max);
	static void setMaxSoundsPerPlayer(int max);
	void setMaxSounds(int max);

	ofEvent<ofSoundBuffer> newBufferE;
    ofEvent<size_t> endEvent;

    const ofxSoundFile& getSoundFile() const {return soundFile;}
	ofxSoundFile& getSoundFile() { return soundFile;}

	virtual void print(string prefix = "") {
		string name = typeid(*this).name();
		cout << prefix << " " << name << endl;
		cout << prefix << "    " << ofFilePath::getFileName(soundFile.getPath()) << endl;
		if (inputObject) {
			inputObject->print(prefix + "    ");
		}
	}
	//enum soundPlayProp {
	//	VOLUME,
	//	IS_PLAYING,
	//	LOOP,
	//	SPEED,
	//	PAN,
	//	RELATIVE_SPEED,
	//	POSITION,
	//	VOL_LEFT,
	//	VOL_RIGHT
	//};
	class soundPlayInstance{
	public:
		float volume;
		bool bIsPlaying =false;
		bool loop = false;
		float speed;
		float pan;
		float relativeSpeed =1;
		unsigned int position=0;
		float volumeLeft, volumeRight;
		size_t id = 0;
	};
	ofParameter<float>volume;
	bool canPlayInstance();
	void drawDebug(float x, float y);
	size_t getNumInstances() { return instances.size(); }
private:
	void audioOutBuffersChanged( int nFrames, int nChannels, int sampleRate );
	void audioOut(ofSoundBuffer& outputBuffer);
	void updatePositions(int numFrames);
	
	size_t playerSampleRate;
	size_t playerNumFrames;
	size_t playerNumChannels;
	//size_t durationMS;
//	float duration;

	static int maxSoundsTotal;
	static int maxSoundsPerPlayer;
	int maxSounds;
	ofSoundBuffer buffer, resampledBuffer;
	ofxSoundFile soundFile;
	bool bStreaming = false;
	bool bMultiplay = false;
	bool bIsLoaded = false;
	bool bIsPlayingAny = false;
	vector<soundPlayInstance> instances;
	//vector<float> volume;
	//vector<bool> bIsPlaying;
	//vector<bool> loop;
	//vector<float> speed;
	//vector<float> pan;
	//vector<float> relativeSpeed;
	//vector<unsigned int> positions;
	//vector<float> volumesLeft, volumesRight;

	void checkPaused();

};


