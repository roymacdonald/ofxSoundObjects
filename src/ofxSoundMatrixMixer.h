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

class ofxSoundMatrixMixerRenderer;
class ofxSoundMatrixMixer: public ofxSoundObject{
public:
	friend class ofxSoundMatrixMixerRenderer;

	ofxSoundMatrixMixer();
	virtual ~ofxSoundMatrixMixer();
	
	ofxSoundObject* getInputChannelSource(size_t channelNumber);
	ofxSoundObject* getInputObject(size_t objectNumber);
	size_t getNumInputChannels();
	size_t getNumInputObjects();
	
	size_t getNumOutputChannels();
	
	
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
	
	void load(const std::string& path);
	void save(const std::string& path);
	
protected:
	struct MatrixInputObject{//this is just an auxiliary struct to keep things tidier 
		void updateChanVolsSize(const size_t& numOutChanns, const size_t& chanCount ){
			if(obj != nullptr){
				auto src  = obj->getSignalSourceObject();
				if(src != nullptr){
					if(channelsVolumes.size() != src->getNumChannels()){
						channelsVolumes.resize(src->getNumChannels());//, std::vector<ofParameter<float> >(numOutChanns));
					}
					for(size_t i = 0; i < channelsVolumes.size();i++){
						if(channelsVolumes[i].size() != numOutChanns){
							channelsVolumes[i].resize(numOutChanns);
							for(size_t o = 0; o < numOutChanns; o++){
								channelsVolumes[i][o].set("chan "+ofToString(chanCount + i) + " : " + ofToString(o), 0,0,1);
							}
						}
					}
				}
			}
		}
		MatrixInputObject(ofxSoundObject* _obj, const size_t& numOutChanns, const size_t& chanCount):obj(_obj){
			updateChanVolsSize(numOutChanns, chanCount);
		}
		
		ofxSoundObject* obj;
		
		std::vector< std::vector<ofParameter<float> > > channelsVolumes; //[in channel index] [ output channel index] 
		std::vector<float> rmsVolume;
		std::vector<float> peakVolume, prevPeak;
		
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
	
	void pullChannel(ofSoundBuffer& buffer, const size_t& n, const size_t &numFrames, const unsigned int & sampleRate);
	
	
	void mixChannelBufferIntoOutput(const size_t& idx, ofSoundBuffer& input, ofSoundBuffer& output);

	bool bComputeRMSandPeak = true;
	
	void putMatrixVolumesIntoParamGroup(ofParameterGroup & group);
	

	
};

