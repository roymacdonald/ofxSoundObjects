//
//  ofxSoundMatrixMixer.h
//  example-matrixMixer
//
//  Created by Roy Macdonald on 4/28/19.
//
//

#pragma once
#include <vector>
#include "ofParameter.h"
#include "ofxSoundObject.h"
#include "ofTypes.h"

class ofxSoundMatrixMixer: public ofxSoundObject{
public:
	
	ofxSoundMatrixMixer();
	virtual ~ofxSoundMatrixMixer();
	
	ofxSoundObject* getInputChannelSource(size_t channelNumber);
	ofxSoundObject* getInputObject(size_t objectNumber);
	size_t getNumInputChannels();
	size_t getNumInputObjects();
	/// sets output volume multiplier.
	/// a volume of 1 means "full volume", 0 is muted.
	void setMasterVolume(float vol);
	float getMasterVolume();
	
	
	void audioOut(ofSoundBuffer &output) override;
	bool isConnected(ofxSoundObject& obj);
	
//	void setChannelVolume(int channelNumber, float vol);
//	float getChannelVolume(int channelNumber);
//	

	ofParameter<float> masterVol;
protected:
	struct MatrixInputObject{//this is just an auxiliary struct to keep things tidier 
		void updateChanVolsSize(const size_t& numOutChanns){
			if(obj){
				if(channelsVolumes.size() != obj->getBuffer().getNumChannels()){
					channelsVolumes.resize(obj->getBuffer().getNumChannels(), std::vector<float>(numOutChanns, 0.0f));
				}
				for(auto& c: channelsVolumes){
					if(c.size() != numOutChanns) 
						c.resize(numOutChanns);
				}
			}
		}
		MatrixInputObject(ofxSoundObject* _obj, const size_t& numOutChanns):obj(_obj){
			updateChanVolsSize(numOutChanns);
		}
		
		ofxSoundObject* obj;
		
		std::vector< std::vector<float> > channelsVolumes; //[in channel index] [ output channel index] 
	};
	
//	void setupMixers();
//	std::vector<ofxSoundMixer> mixers;
	void masterVolChanged(float& f);
	void disconnectInput(ofxSoundObject * input) override;
	std::vector<MatrixInputObject>inObjects;
//	vector<ofxSoundObject*>inObjects;
//	std::vector< std::vector<float> >inObjectsVolumes; //[ output channel index ] [ input channel index ] 
	size_t numInputChannels = 0;
	size_t numOutputChannels = 0;
	void updateNumInputChannels();
	void updateNumOutputChannels(const size_t & nc);
//	vector<float> channelVolume;
//	float masterPan;
	float masterVolume;
	void setInput(ofxSoundObject *obj) override;
	ofMutex mutex;

	
	
	
};
