#pragma once

#include "ofMain.h"
#include "ofxSoundObject.h"
/**
 * This class represents a simple mixer which adds together the output
 * of multiple chains of ofxSoundObjects
 */
class ofxSoundMixer: public ofxSoundObject {
public:
	ofxSoundMixer();
    virtual ~ofxSoundMixer();

	virtual  std::string getName() override{ return "Sound Mixer";}
	OF_DEPRECATED_MSG("Use getConnectionSource instead",ofxSoundObject* getChannelSource(int channelIndex));
	
	/// returns the connected object at the specified index
	ofxSoundObject* getConnectionSource(int connectionIndex);
	/// get the number of connected objects.
	OF_DEPRECATED_MSG("Use getNumConnections() instead", size_t getNumChannels() override;)
	size_t getNumConnections();
	
	
	
	/// sets output volume multiplier.
	/// a volume of 1 means "full volume", 0 is muted.
	void  setMasterVolume(float vol);
	float getMasterVolume();

	/// sets output stereo panning.
	/// 0.5 is center panned, 0 is full left and 1 is full right.
	/// panning is disabled for non-stereo sound.
	void  setMasterPan(float pan);
	float getMasterPan();

	void audioOut(ofSoundBuffer &output) override;
	bool isConnectedTo(ofxSoundObject& obj);

    OF_DEPRECATED_MSG("Use setConnectionVolume", void  setChannelVolume(int channelNumber, float vol));
    OF_DEPRECATED_MSG("Use getConnectionVolume", float getChannelVolume(int channelNumber));
	
	void  setConnectionVolume(int channelNumber, float vol);
	float getConnectionVolume(int channelNumber);
	
	ofParameter<float> masterVol;
protected:
	void masterVolChanged(float& f);
	void disconnectInput(ofxSoundObject * input) override;
	vector<ofxSoundObject*> connections;
    vector<float> connectionVolume;
    float masterPan;
	float masterVolume;
	void setInput(ofxSoundObject *obj) override;
	ofMutex mutex;
};

