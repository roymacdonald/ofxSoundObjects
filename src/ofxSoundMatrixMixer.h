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
#include "ofxSoundObjectsConstants.h"
 
#ifdef OFX_SOUND_ENABLE_MULTITHREADING
#include "tbb/parallel_for.h"
#endif

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
	
	void setOutputVolumeForAllChannels(const float & volValue);
	
	
	
	void load(const std::string& path);
	void save(const std::string& path);
	
	static ofParameter<bool>& getComputeRMSandPeak();
	
protected:

	class MatrixInputObject{ 
	public:
		friend class ofxSoundMatrixMixer;

		//	
		//		MatrixInputObject();
		MatrixInputObject(ofxSoundObject* _obj, const size_t& numOutChanns, const size_t& chanCount);
		~MatrixInputObject();
		
		
		bool updateChanVolsSize(const size_t& numOutChanns, const size_t& chanCount );
		
		ofxSoundObject* obj;
		
		std::vector< std::vector<ofParameter<float> > > channelsVolumes; //[in channel index] [ output channel index] 
		VUMeter vuMeter;
		bool bBufferProcessed = false;
		
		
		void pullChannel();
		
		
		ofSoundBuffer & getBuffer();
		
		size_t numFramesToProcess;
		unsigned int sampleRate;

	private:

		ofSoundBuffer buffer;
	};

	std::vector< size_t > matrixInputChannelMap;
	std::vector< size_t > numConnectionInputChannels;
	std::vector< size_t > connectionFirstChannel;
	
	
	VUMeter outVuMeter;

	std::vector<ofParameter<float>> outputVolumes;
	ofParameter<float> masterVol;
	void masterVolChanged(float& f);
	
	void disconnectInput(ofxSoundObject * input) override;
	std::vector<std::shared_ptr<MatrixInputObject>>inObjects;
	size_t numInputChannels = 0;
	size_t numOutputChannels = 0;
	void updateNumInputChannels();
	void updateNumOutputChannels(const size_t & nc);
	
	float masterVolume;
	
	void setInput(ofxSoundObject *obj) override;
	

	void mixChannelBufferIntoOutput(const size_t& idx, ofSoundBuffer& input, ofSoundBuffer& output);
		
	void putMatrixVolumesIntoParamGroup(ofParameterGroup & group);
	
	
#ifdef OFX_SOUND_ENABLE_MULTITHREADING
	struct MatrixInputsCollection{
		std::vector<std::shared_ptr<MatrixInputObject>> * inObjects;
		void operator()( const tbb::blocked_range<size_t>& range ) const {
			for( size_t i = range.begin(); i!=range.end(); ++i ){
				inObjects->at(i)->pullChannel();		
			}
		}
	};
#endif

	
private:
	ofMutex mutex;
	bool bNeedsInputNumUpdate = true; 
	
	float dummyFloat;//this is bad practice
	
};

