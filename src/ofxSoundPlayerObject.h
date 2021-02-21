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
	void setSpeed(float spd, int index =-1 );
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
	bool  getIsLooping(size_t index =-1) const;
	unsigned long getDurationMS();

	const ofSoundBuffer & getCurrentBuffer() const;
	const ofSoundBuffer & getBuffer() const;
    ofEvent<size_t> endEvent;
	ofEvent<void>& getAsyncLoadEndEvent();

    const ofxSoundFile& getSoundFile() const {return soundFile;}
	ofxSoundFile& getSoundFile() { return soundFile;}

	class soundPlayInstance{
	public:
        soundPlayInstance(){
            updateVolumes();
        }
		float volume = 1;
		bool bIsPlaying =false;
		bool loop = false;
		float speed;
		float pan = 0;
		float relativeSpeed =1;
		unsigned int position=0;
		float volumeLeft, volumeRight;
		size_t id = 0;
        void updateVolumes(){
            ofStereoVolumes(volume, pan, volumeLeft, volumeRight);
        }
	};
	ofParameter<float>volume;
	bool canPlayInstance();
	void drawDebug(float x, float y);
	size_t getNumInstances() { return instances.size(); }
	
	
	
	const std::string getFilePath() const;
	
private:
	enum State{
		UNLOADED = 0,
		LOADING_ASYNC,
		LOADING_ASYNC_AUTOPLAY,
		LOADED
	};
	std::atomic<State> state;
	void setState(State newState);
	bool isState(State compState);
	
	void initFromSoundFile();
	
	
	void audioOutBuffersChanged( int nFrames, int nChannels, int sampleRate );
	virtual void audioOut(ofSoundBuffer& outputBuffer) override;
	void updatePositions(int numFrames);
	
	std::atomic<size_t> playerSampleRate;
	size_t playerNumFrames;
	size_t playerNumChannels;
	size_t sourceSampleRate;
	size_t sourceNumFrames;
	size_t sourceNumChannels;
	size_t sourceDuration;

	ofSoundBuffer buffer; 
	ofSoundBuffer resampledBuffer;
	ofxSoundFile soundFile;
	
	std::atomic<bool> bStreaming;
	std::atomic<bool> bMultiplay;
	std::atomic<bool> bIsPlayingAny;
	
	bool bDefaultlLooping = false;

	void setNumInstances(const size_t & num);
	
	vector<soundPlayInstance> instances;

	void checkPaused();
    
    void updateInstance(std::function<void(soundPlayInstance& inst)> func, int index, string methodName);
	ofMutex mutex;
	mutable ofMutex instacesMutex, volumeMutex;
	
	
	void update(ofEventArgs&);
	ofEventListener updateListener;
	std::atomic<bool> bListeningUpdate;
	std::vector<size_t> endedInstancesToNotify;
	void addInstanceEndNotification(const size_t & id);
	void clearInstanceEndNotificationQueue();
	ofMutex instanceEndQueueMutex;

	
	void processBuffers(ofSoundBuffer& buf, soundPlayInstance& i, const float& vol, const std::size_t& nFrames, const std::size_t& nChannels);
	
};


