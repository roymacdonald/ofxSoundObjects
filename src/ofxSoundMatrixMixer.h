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
#include "VUMeter.h"

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
	

	// this function will set the volume of the channel relative to its parent object (connection)
	// a connection can have any amount of channels. Connections are the input ofxSoundObject.
	// This means this means if we have two objects conected, to change the
	// volume for the first input channel of the second object that goes out to the first output channel 
	// you would call setVolumeForChannel( volValue, 1, 0, 0);
	// remember that channels and connections are zero based, meaning that the first is zero.
	// this same logic applies for the getter.
	void setVolumeForConnectionChannel(const float& volValue, const size_t& connectionIndex, const size_t& inputChannel, const size_t& outputChannel);
	
	const float& getVolumeForConnectionChannel(const size_t& connectionIndex, const size_t& inputChannel, const size_t& outputChannel) const;
	
	
	
	// this function will set the volume of the channel relative to the overall input channel count.
	// This means this means if we have two objects conected, each with 2 channels and we want to change the
	// volume for the first input channel of the second object that goes out to the first output channel 
	// you would call setVolumeForChannel( volValue, 2, 0);
	// remember that channels are zero based, meaning that the first is zero.
	// this same logic applies for the getter.
	void setVolumeForChannel(const float& volValue, const size_t& inputChannel, const size_t& outputChannel);
	const float& getVolumeForChannel(const size_t& inputChannel, const size_t& outputChannel) const;

	// returns the connection index of a certain overall channel
	size_t getConnectionIndexAtInputChannel(const size_t& chan);
	
	
	
	/// Will return the overall first channel for a connection 
	size_t getFirstInputChannelForConnection(const size_t& connectionIndex);
	
	
	void setOutputVolumeForChannel (const float & volValue, const size_t& outputChannel);
	const float & getOutputVolumeForChannel ( const size_t& outputChannel)const;
	
	
	void load(const std::string& path);
	void save(const std::string& path);
	
protected:
	struct MatrixInputObject{//this is just an auxiliary struct to keep things tidier 
		bool updateChanVolsSize(const size_t& numOutChanns, const size_t& chanCount ){
			bool bUpdated = false;
			if(obj != nullptr){
				auto src  = obj->getSignalSourceObject();
				if(src != nullptr){
					if(channelsVolumes.size() != src->getNumChannels()){
						channelsVolumes.resize(src->getNumChannels());//, std::vector<ofParameter<float> >(numOutChanns));
						bUpdated = true;
					}
					for(size_t i = 0; i < channelsVolumes.size();i++){
						if(channelsVolumes[i].size() != numOutChanns){
							channelsVolumes[i].resize(numOutChanns);
							bUpdated = true;
							for(size_t o = 0; o < numOutChanns; o++){
								channelsVolumes[i][o].set("chan "+ofToString(chanCount + i) + " : " + ofToString(o), 0,0,1);
							}
						}
					}
				}
			}
			return bUpdated;
		}
		MatrixInputObject(ofxSoundObject* _obj, const size_t& numOutChanns, const size_t& chanCount):obj(_obj){
			updateChanVolsSize(numOutChanns, chanCount);
		}
		
		ofxSoundObject* obj;
		
		std::vector< std::vector<ofParameter<float> > > channelsVolumes; //[in channel index] [ output channel index] 
		VUMeter vuMeter;
		bool bBufferProcessed = false;
		
	};

	std::vector< size_t > matrixInputChannelMap;
	std::vector< size_t > numConnectionInputChannels;
	std::vector< size_t > connectionFirstChannel;
	
	
	VUMeter outVuMeter;

	std::vector<ofParameter<float>> outputVolumes;
	ofParameter<float> masterVol;
	void masterVolChanged(float& f);
	
	void disconnectInput(ofxSoundObject * input) override;
	std::vector<MatrixInputObject>inObjects;
	size_t numInputChannels = 0;
	size_t numOutputChannels = 0;
	void updateNumInputChannels();
	void updateNumOutputChannels(const size_t & nc);
	
	float masterVolume;
	
	void setInput(ofxSoundObject *obj) override;
	
	void pullChannel(ofSoundBuffer& buffer, const size_t& n, const size_t &numFrames, const unsigned int & sampleRate);
	
	
	void mixChannelBufferIntoOutput(const size_t& idx, ofSoundBuffer& input, ofSoundBuffer& output);

	bool bComputeRMSandPeak = true;
	
	void putMatrixVolumesIntoParamGroup(ofParameterGroup & group);
private:
	ofMutex mutex;
	bool bNeedsInputNumUpdate = true; 
	
	float dummyFloat;//this is bad practice
	
};

