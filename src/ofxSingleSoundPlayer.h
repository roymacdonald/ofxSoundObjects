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


class ofxMultiSoundPlayer;

/// \ brief
/// This class is a fully featured sound file player, with the particularity that it does not have multi play.
/// This means that each time you call play() the sound will start playing again from the begining of the file,
/// instead of playing a new "instance" of the sound alongside the one that is currently sounding.
/// The purpose of having this class is that it demands less work and processes faster than ofxSoundPlayerObject
class ofxSingleSoundPlayer:  public ofxSoundObject {
public:
	ofxSingleSoundPlayer();
	
	friend class ofxSoundPlayerObject;
	
	bool load(const ofSoundBuffer& loadBuffer, const std::string& name);
	
	bool load(std::filesystem::path filePath, bool stream = false);
	bool loadAsync(std::filesystem::path filePath, bool bAutoplay);
	
	bool load(shared_ptr<ofxSoundFile>& sharedFile);
	
	void unload();
	void play();
	void stop();
	
	virtual size_t getNumChannels() override;
	
	void setVolume(float vol );
	void setPan(float pan ); // -1 = left, 1 = right
	
	///\brief set playback speed.
	///\param speed the speed to set the audio playback to.
	///\param preprocess when true it will make a copy of the audio buffer and resample it so there is less use
	/// 				 of processing power while playing back. otherwise, when false (default) the audio is resampled on real time.
	void setSpeed(float speed, bool preprocess = false);
	void setPaused(bool bP );
	void setLoop(bool bLp );
	
	void setPosition(float pct ); // pct: 0 = start, 1 = end;
	void setPositionMS(int ms );

	///\brief set the players id.
	///
	/// by default each player instance will get a different id, but you might want to set it to a specific value.
	/// The id is passed to the callback function of the endEvent listener. This way you can have a single callback
	/// function and based on the id you can tell which player was the one that ended playing.
	///\param id the id the player will get.
	void setId(size_t id);
	
	float getPosition() const;
	int	  getPositionMS() const;
	bool  isPlaying() const;
	float getSpeed() const;
	float getRelativeSpeed() const;
	float getPan() const;
	bool  isLoaded() const;
	float getVolume() const;
	bool  isLooping() const;
	unsigned long getDurationMS();
	
	size_t getNumFrames() const;
	
	size_t getId() const;


    ofEvent<size_t> endEvent;
	ofEvent<void>& getAsyncLoadEndEvent();

	const ofSoundBuffer & getBuffer() const;
	
	const ofxSoundFile& getSoundFile() const;
	ofxSoundFile& getSoundFile();
	
	///\returns a shared pointer of the sound file loaded in the player. This is useful when having multiple players playing the same sound, but you only want to load it once. Notice that the returned shared pointer can be null.
	const shared_ptr <ofxSoundFile>& getSharedSoundFile() const;
	shared_ptr <ofxSoundFile>& getSharedSoundFile();
	
	
	ofParameter<float>volume;
	
	void drawDebug(float x, float y);
	
	std::string getFilePath() const;
	
	std::string getSourceInfo() const;
	std::string getPlaybackInfo() const;
	
private:
	
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
	void initFromSoundBuffer();
	
	
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
	std::atomic<bool> loop ;
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
	
//	void processBuffers(const ofSoundBuffer& sourceBuffer, ofSoundBuffer& outputBuffer);
	

	void _makeSoundFile();
	void _makeSoundBuffer();

	static ofSoundBuffer _dummyBuffer;
	static ofxSoundFile _dummySoundFile;
	
	static ofxSoundFile& _getDummySoundFile();
	
	size_t _id = 0;
#ifdef USE_OFX_SAMPLE_RATE
	unique_ptr<ofxSamplerate> sampleRateConverter = nullptr;
#endif
};


