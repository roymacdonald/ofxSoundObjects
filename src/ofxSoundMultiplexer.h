//
//  ofxSoundMultiplexer.h
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#pragma once

#include "ofxSoundObject.h"
#include <map>
#include <set>
//--------------------------------------------------------------
//  ofxSoundInputMultiplexer
//--------------------------------------------------------------

typedef std::pair<size_t,size_t> linksIndices;// input index, output index

class ofxSoundBaseMultiplexer{

public:
    /// Create a new channel group. This is mostly useful when using a multi input audio interface.
    /// The vector that is passed as argument contains the indexes of the inputs that will make the group.
    /// This function outputs the index at which the new group will be stored. If trying to add a group already present, it will return the position of the group that is already present and will not add a new one.
    /// The groups are then used to create independent ofSoundBuffer instances that will only contain the audio data of its corresponding channels, which will be passed to the subsequent ofxSoundObject chain. This not only allows flexible routing but also reduces the amount of data being passed in the processing chain.
	ofxSoundObject& linkChannelsToObject(const std::vector<linksIndices>& links, ofxSoundObject& obj);
	ofxSoundObject& linkChannelsToObject(const linksIndices& links, ofxSoundObject& obj);
	ofxSoundObject& linkChannelToObject(size_t channel, ofxSoundObject& obj);
    
	
	bool deleteChannelsLinkedToObject(const std::vector<linksIndices>& links, ofxSoundObject& obj);
    bool deleteChannelsLinkedToObject(const linksIndices& links, ofxSoundObject& obj);
    bool deleteChannelLinkedToObject(size_t chan, ofxSoundObject& obj);
	
	
	std::set < std::pair<linksIndices, ofxSoundObject*>> & getLinkedChannelsToObjects();
    
    const std::set < std::pair<linksIndices, ofxSoundObject*>>& getLinkedChannelsToObjects() const ;
protected:
	virtual void sortLinks() = 0;
	
	std::set < std::pair<linksIndices, ofxSoundObject*>> linkedChannelsToObj;
	std::map < ofxSoundObject*, std::map<size_t, std::set<size_t>>> linkedObjToChannels;
//    std::map < int, ofxSoundObject*> channelsMap;
//	std::set<int> channelsSet;
private:
	
	
	
};

//////////////   MUX  

/// A multiplexer will take several inputs, in this case in the form of ofxSoundObject's ofSoundBuffers and create a single output ofSoundBuffer, where all the different inputs are rearranged and put into the different channels of the output buffer. 

//--------------------------------------------------------------
//  ofxSoundMultiplexer
//--------------------------------------------------------------
class ofxSoundMultiplexer: public ofxSoundBaseMultiplexer, public ofxSoundObject{
public:
	ofxSoundMultiplexer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){}
	virtual  std::string getName() override{ return "Sound Multiplexer";}
//	virtual size_t getNumChannels() override;	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
};

//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
class ofxSoundOutputMultiplexer: public ofxSoundMultiplexer{
public:
	ofxSoundOutputMultiplexer():ofxSoundMultiplexer(){
		type = OFX_SOUND_OBJECT_DESTINATION;
	}

	virtual  std::string getName() override{ return "Sound Output Multiplexer";}
	
};

//////////////   MUX  END



//////////////   DEMUX   
//--------------------------------------------------------------
//  ofxSoundDemultiplexer
//--------------------------------------------------------------
/// A demultiplexer, or demux, is the opposite of a multiplexer,  will take a single input, in this case in the form of ofxSoundObject's ofSoundBuffers. It hast serveral outputs, each mapped to a specific channel or channels of the input buffer. It will fill each outputs buffer with the input's channels that correspond to the channels that the output is mapped to.
class ofxSoundDemultiplexer:  public ofxSoundBaseMultiplexer, public ofxSoundObject{
public:
	ofxSoundDemultiplexer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
		chanMod = OFX_SOUND_OBJECT_CHAN_DEMUX;
	}
	virtual  std::string getName() override{ return "Sound Demultiplexer";}
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	/// this will connect a specific channel to the passes sound object.
	/// It will work as a kind of splitter.
//	ofxSoundObject &connectChannelTo(size_t sourceChannel, size_t destChannel, ofxSoundObject &soundObject);
//	ofxSoundObject &connectChannelsTo(const std::vector<int>& sourceGroup, const std::vector<int>& destGroup, ofxSoundObject &soundObject);
//	
//	
//	/// Disconnect a specific channel, one connected using connectChannelTo(...) 
//	void disconnectChannel(size_t sourceChannel, size_t destChannel);
//	/// Disconnect a specific channel group, one connected using connectChannelsTo(...)
//	void disconnectChannels(const std::vector<int>& sourceGroup, const std::vector<int>& destGroup);
	
	
protected:
//	std::map < ofxSoundObject* , int> channelsPtrMap;
	
};


//--------------------------------------------------------------
//  ofxSoundInputMultiplexer
//--------------------------------------------------------------
class ofxSoundInputDemultiplexer: public ofxSoundDemultiplexer{
public:
	ofxSoundInputDemultiplexer():ofxSoundDemultiplexer(){
		type = OFX_SOUND_OBJECT_SOURCE;
	}
	virtual  std::string getName() override{ return "Sound Input Demultiplexer";}
	
};

//////////////   DEMUX END
