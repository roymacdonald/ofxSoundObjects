//
//  ofxSimpleSoundPlayer.h
//  example-soundPlayerObject_multi
//
//  Created by Roy Macdonald on 4/15/21.
//
#pragma once

#include "ofBaseTypes.h"
#include "ofConstants.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"
#include "ofSoundUtils.h"
#include "ofSoundStream.h"
#include "ofEvents.h"
//#include "ofxSoundObjects.h"
#include "ofxSoundObject.h"
//#include "ofxSoundMixer.h"
#include "ofxSoundFile.h"
//#include "ofxSoundPlayerObject.h"
#include "ofxSoundUtils.h"

//#define USE_OFX_SAMPLE_RATE
#ifdef USE_OFX_SAMPLE_RATE
#include "ofxSamplerate.h"
#endif



class ofxBaseSoundPlayer:  public ofxSoundObject{
public:
	ofxBaseSoundPlayer():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
	
	virtual bool load(const ofSoundBuffer& loadBuffer, const std::string& name) = 0;
	
	virtual bool load(std::filesystem::path filePath, bool stream = false) = 0;
	virtual bool loadAsync(std::filesystem::path filePath, bool bAutoplay) = 0;
	
	virtual bool load(shared_ptr<ofxSoundFile>& sharedFile) = 0;
	virtual void unload() = 0;
	
	virtual float getPosition() const = 0;
	virtual int	  getPositionMS() const = 0;
	virtual bool  isPlaying() const = 0;
	virtual float getSpeed() const = 0;
	virtual float getRelativeSpeed() const = 0;
	virtual float getPan() const = 0;
	virtual bool  isLoaded() const = 0;
	virtual float getVolume() const = 0;
	virtual bool  isLooping() const = 0;
	virtual unsigned long getDurationMS() = 0;
	virtual bool isReplaying() const = 0;
	virtual int getSourceSampleRate() const = 0;
	virtual size_t getNumFrames() const = 0;
	
	virtual std::string getFilePath() const = 0;
};


class ofxMultiSoundPlayer;

/// \ brief
/// This class is a fully featured sound file player, with the particularity that it does not have multi play.
/// This means that each time you call play() the sound will start playing again from the begining of the file,
/// instead of playing a new "instance" of the sound alongside the one that is currently sounding.
/// The purpose of having this class is that it demands less work and processes faster than ofxSoundPlayerObject
class ofxSingleSoundPlayer:  public ofxBaseSoundPlayer {
public:
	ofxSingleSoundPlayer();
	
	friend class ofxMultiSoundPlayer;
	
	virtual bool load(const ofSoundBuffer& loadBuffer, const std::string& name) override;
	
	virtual bool load(std::filesystem::path filePath, bool stream = false) override;
	virtual bool loadAsync(std::filesystem::path filePath, bool bAutoplay) override;
	
	virtual bool load(shared_ptr<ofxSoundFile>& sharedFile) override;
	
	virtual void unload() override;
	void play();
	void stop();

	
	void setVolume(float vol);
	void setPan(float pan ); // -1 = left, 1 = right
	
	///\brief set playback speed.
	///\param speed the speed to set the audio playback to.
	///\param preprocess when true it will make a copy of the audio buffer and resample it so there is less use
	/// 				 of processing power while playing back. otherwise, when false (default) the audio is resampled on real time.
	void setSpeed(float speed, bool preprocess = false);
	void setPaused(bool bP);
	void setLoop(bool bLp);
	
	void setPosition(float pct ); // pct: 0 = start, 1 = end;
	void setPositionMS(int ms);

	///\brief set the players id.
	///
	/// by default each player instance will get a different id, but you might want to set it to a specific value.
	/// The id is passed to the callback function of the endEvent listener. This way you can have a single callback
	/// function and based on the id you can tell which player was the one that ended playing.
	///\param id the id the player will get.
	void setId(size_t id);
	
	
	virtual size_t getNumChannels() override;
	
	virtual float getPosition() const override;
	virtual int	  getPositionMS() const override;
	virtual bool  isPlaying() const override;
	virtual float getSpeed() const override;
	virtual float getRelativeSpeed() const override;
	virtual float getPan() const override;
	virtual bool  isLoaded() const override;
	virtual float getVolume() const override;
	virtual bool  isLooping() const override;
	virtual unsigned long getDurationMS() override;
	virtual int getSourceSampleRate() const override;
	virtual size_t getNumFrames() const override;
	
	size_t getId() const;

    ofEvent<size_t> endEvent;
	ofEvent<void>& getAsyncLoadEndEvent();

	const ofSoundBuffer & getBuffer() const;
	
	const ofxSoundFile& getSoundFile() const;
	ofxSoundFile& getSoundFile();
	
	///\returns a shared pointer of the sound file loaded in the player. This is useful when having multiple players playing the same sound, but you only want to load it once. Notice that the returned shared pointer can be null.
	const shared_ptr <ofxSoundFile>& getSharedSoundFile() const;
	shared_ptr <ofxSoundFile>& getSharedSoundFile();
	
	virtual std::string getFilePath() const override;
	
	std::string getSourceInfo() const;
	std::string getPlaybackInfo() const;
	
	ofParameter<float>volume;
	
	void drawDebug(float x, float y);
	

	
	///\brief will replay every certain amount of times
	///\param interval. The time interval between each replay. in milliseconds
	///\param times. How many times it should be repeated. Default is -1 which means an infinite number of times.
	void replayEvery(int interval, int times = -1);
	
	///\brief Get if the player is currently in replay mode (set by calling repeatEvery(...))
	///\returns true if in repeat mode false otherwise.
	virtual bool isReplaying() const override;
	
	///\brief get the interval set by repeatEvery function
	///\returns the interval in milliseconds
	int getReplayInterval() const;
	
	///\get how many more times it is going to be repeated
	///\returns remaining times to play sound
	int getReplayRemainigTimes() const;
	
protected:
	static ofSoundBuffer _dummyBuffer;
	static ofxSoundFile _dummySoundFile;
	
	static ofxSoundFile& _getDummySoundFile();
	
private:
	enum LoopMode{
		OFX_SOUND_OBJECTS_NONE=0,
		OFX_SOUND_OBJECTS_LOOP,
		OFX_SOUND_OBJECTS_REPEAT
	};
	std::atomic<LoopMode> loopMode;
	std::atomic<int> replayInterval;
	std::atomic<int> replayTimes;
	
	std::atomic<uint64_t> lastPlayTime;
	
	void checkReplay();

	void updateVolumes();

	size_t id = 0;
	
	atomic<bool> bNeedsFade = {false};
	atomic<bool> bFadeIn = {true};
	
	enum State{
		UNLOADED = 0,
		LOADING_ASYNC,
		LOADING_ASYNC_AUTOPLAY,
		LOADED,
		RESAMPLING
	};
	std::atomic<State> state;
	void setState(State newState);
	bool isState(State compState)const;
	
	void initFromSoundFile();
	void initFromSoundBuffer(const ofSoundBuffer& buffer);
	
	
	void checkBuffer(const ofSoundBuffer& outputBuffer);
	virtual void audioOut(ofSoundBuffer& outputBuffer) override;
	void updatePositions(int numFrames);
	

	
	void resetValues();

	
	
	atomic<size_t> sourceSampleRate;
	atomic<size_t> sourceNumFrames;
	atomic<size_t> sourceNumChannels;
	size_t sourceDuration;
	atomic<size_t> outputSampleRate;
	
	
	std::atomic<bool> bIsPlaying;

	std::atomic<float> speed;
	std::atomic<float> pan ;
	std::atomic<float> relativeSpeed;
	std::atomic<size_t> position;
	std::atomic<float> volumeLeft, volumeRight;
	
	void updateRelativeSpeed();
	atomic<bool> bNeedsRelativeSpeedUpdate;
	
	void volumeChanged(float&);
	
	
	unique_ptr<ofSoundBuffer> buffer = nullptr;
	unique_ptr<ofSoundBuffer> preprocessedBuffer = nullptr;
	shared_ptr<ofxSoundFile> soundFile = nullptr;
	
	std::atomic<bool> bStreaming;
	
	bool preprocessBuffer();
	std::atomic<bool> bNeedsPreprocessBuffer;

	ofEventListener volumeListener;
    
    
	ofMutex mutex;
	mutable ofMutex volumeMutex;
	
	
	void update(ofEventArgs&);
	ofEventListener updateListener;
	std::atomic<bool> bListeningUpdate ;
	std::atomic<bool> bNotifyEnd ;
	
	void enableUpdateListener();
	void disableUpdateListener(bool forceDisable = false);
	
	
	void _makeSoundFile();
	void _makeSoundBuffer();


	
	size_t _id = 0;
#ifdef USE_OFX_SAMPLE_RATE
	unique_ptr<ofxSamplerate> sampleRateConverter = nullptr;
#endif
};


