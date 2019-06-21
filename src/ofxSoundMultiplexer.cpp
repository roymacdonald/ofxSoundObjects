//
//  ofxSoundBufferMultiplexer.cpp
//  example-soundPlayerObject
//
//  Created by Roy Macdonald on 25-11-17.
//
//

#include "ofxSoundMultiplexer.h"
#include "ofxSoundUtils.h"
#include "ofUtils.h"
#include "ofLog.h"

//--------------------------------------------------------------
//  ofxSoundBaseMultiplexer
//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::linkChannelsToObject(const std::vector<linksIndices>& links, ofxSoundObject& obj){
	for(auto&l : links){
		linkChannelsToObject(l, obj);
	}
	return obj;
}
//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::linkChannelsToObject(const linksIndices& links, ofxSoundObject& obj){
	linkedChannelsToObj.insert({links, &obj});
	linkedObjToChannels[&obj][links.first].insert(links.second);
	
	return obj;
}
//--------------------------------------------------------------
ofxSoundObject& ofxSoundBaseMultiplexer::linkChannelToObject(size_t channel, ofxSoundObject& obj){
	return linkChannelsToObject({channel, channel}, obj);
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelsLinkedToObject(const std::vector<linksIndices>& links, ofxSoundObject& obj){
	if(links.size() == 0)return false;
	bool bSuccess = true;
	for(auto&l : links){
		bSuccess &= deleteChannelsLinkedToObject(l, obj);
	}
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelsLinkedToObject(const linksIndices& links, ofxSoundObject& obj){

	if(linkedChannelsToObj.erase({links, &obj})){
		
		if(linkedObjToChannels.count(&obj)){
			if(linkedObjToChannels[&obj].count(links.first)){
				linkedObjToChannels[&obj][links.first].erase(links.second);
				if(linkedObjToChannels[&obj][links.first].size() == 0){
					linkedObjToChannels[&obj].erase(links.first);
					if(linkedObjToChannels[&obj].size()){
						linkedObjToChannels.erase(&obj);
					}
				}
			}
		}else{
			std::cout << "something went wrong. Code should not reach here" << std::endl; 
		}
		return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundBaseMultiplexer::deleteChannelLinkedToObject(size_t chan, ofxSoundObject& obj){
	return  deleteChannelsLinkedToObject({chan, chan}, obj);
}

//--------------------------------------------------------------
std::set < std::pair<linksIndices, ofxSoundObject*>> & ofxSoundBaseMultiplexer::getLinkedChannelsToObjects(){
	return linkedChannelsToObj;
}
//--------------------------------------------------------------
const std::set < std::pair<linksIndices, ofxSoundObject*>>& ofxSoundBaseMultiplexer::getLinkedChannelsToObjects() const {
	return linkedChannelsToObj;
}

//--------------------------------------------------------------
//  ofxSoundDemultiplexer
//--------------------------------------------------------------

void ofxSoundDemultiplexer::process(ofSoundBuffer &input, ofSoundBuffer &output){
//		for(auto & m: channelsMap){
//			ofxSoundUtils::getBufferFromChannelGroup(input, m.second.getBuffer(), m.first);
//		}
}


//--------------------------------------------------------------
//  ofxSoundOutputMultiplexer
//--------------------------------------------------------------
//size_t ofxSoundMultiplexer::getNumChannels(){
//	return channelsSet.size();
//}
//--------------------------------------------------------------
void ofxSoundMultiplexer::process(ofSoundBuffer &input, ofSoundBuffer &output) {
//	for(auto & m: linkedObjToChannels){
//		
//		ofSoundBuffer temp;
//		temp.allocate(output.getNumFrames(), m.first->size());
//		temp.setSampleRate(output.getSampleRate());
//		m..audioOut(temp);
//	}
//	
	//muxing all groups
	//	for(auto & m: channelsMap){
	//		ofxSoundUtils::setBufferFromChannelGroup(m.second.getBuffer(), output, m.first);
	//	}
}
