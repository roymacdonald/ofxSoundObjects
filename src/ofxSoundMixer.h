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

	ofxSoundObject* getChannelSource(int channelNumber);
	int getNumChannels();

	/// sets output volume multiplier.
	/// a volume of 1 means "full volume", 0 is muted.
	void setMasterVolume(float vol);
	float getMasterVolume();

	/// sets output stereo panning.
	/// 0.5 is center panned, 0 is full left and 1 is full right.
	/// panning is disabled for non-stereo sound.
	void  setMasterPan(float pan);
	float getMasterPan();

	void audioOut(ofSoundBuffer &output);
	bool isConnectedTo(ofxSoundObject& obj);

    void setChannelVolume(int channelNumber, float vol);
    float getChannelVolume(int channelNumber);
	
//	virtual void print(string prefix = "") {
//		cout << prefix << " " << "MIXER: " << channels.size() << endl;
//		prefix += "----";
//		for (int i = 0; i < channels.size(); i ++) {
//			if (channels[i] != nullptr) {
//				channels[i]->print(prefix);
//			}
//		}
//	}

	ofParameter<float> masterVol;
protected:
	void masterVolChanged(float& f);
	void disconnectInput(ofxSoundObject * input);
	vector<ofxSoundObject*>channels;
    vector<float> channelVolume;
    float masterPan;
	float masterVolume;
	void setInput(ofxSoundObject *obj);
	ofMutex mutex;
};

