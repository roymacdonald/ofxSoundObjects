//
//  ofxSoundMatrixMixer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 4/28/19.
//
//

#include "ofxSoundMatrixMixer.h"
#include "ofxSoundUtils.h"
//----------------------------------------------------
ofxSoundMatrixMixer::ofxSoundMatrixMixer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	masterVolume = 1.0f;	
	masterVol.set("Master Vol", 1, 0, 1);
	masterVol.addListener(this, &ofxSoundMatrixMixer::masterVolChanged);
}
//----------------------------------------------------
void ofxSoundMatrixMixer::masterVolChanged(float& f) {
	mutex.lock();
	masterVolume = masterVol;
	mutex.unlock();
}
//----------------------------------------------------
ofxSoundMatrixMixer::~ofxSoundMatrixMixer(){
	masterVol.removeListener(this, &ofxSoundMatrixMixer::masterVolChanged);
}
//----------------------------------------------------
ofxSoundObject* ofxSoundMatrixMixer::getInputObject(size_t objectNumber){
	if (objectNumber < inObjects.size()) {
		return inObjects[objectNumber].obj;
	}else{
		return nullptr;
	}
}


//----------------------------------------------------
size_t ofxSoundMatrixMixer::getNumOutputChannels(){
	return numOutputChannels;
}

//----------------------------------------------------
size_t ofxSoundMatrixMixer::getNumInputChannels(){
	return numInputChannels;
}
//----------------------------------------------------
size_t ofxSoundMatrixMixer::getNumInputObjects(){
	return inObjects.size();
}


//----------------------------------------------------
ofxSoundObject* ofxSoundMatrixMixer::getInputChannelSource(size_t channelNumber){
	if (channelNumber < inObjects.size()) {
		return inObjects[channelNumber].obj;
	}else{
		return nullptr;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::disconnectInput(ofxSoundObject * input){
	ofRemove(inObjects, [&](MatrixInputObject& o){return input == o.obj;});
//	for (int i =0; i<input.size(); i++) {
//		if (input == channels[i]) {
//			channels.erase(channels.begin() + i);
////			channelVolume.erase(channelVolume.begin() + i);
//			break;
//		}
//	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setInput(ofxSoundObject *obj){
	if(obj){
	for (int i =0; i<inObjects.size(); i++) {
		if (obj == inObjects[i].obj) {
			ofLogNotice("ofxSoundMatrixMixer::setInput") << " already connected" ;
			return;
		}
	}
	inObjects.push_back(MatrixInputObject(obj, numOutputChannels, numInputChannels));
	updateNumInputChannels();
//		for(auto & i : inObjects){
			
//			i.resize(numInputChannels);
//			inObjectsVolumes.push_back(std::vector<float>( numInputChannels, 0.0f));
//		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumOutputChannels(const size_t & nc){
	if(numOutputChannels != nc){
		numOutputChannels = nc;
		size_t count = 0;
		for(auto& i : inObjects){
			i.updateChanVolsSize(numOutputChannels, count);
			count += i.channelsVolumes.size();
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumInputChannels(){

	numInputChannels = 0;
	
	ofxSoundObject * src = nullptr;
	for(auto& i : inObjects){
		src = i.obj->getSignalSourceObject();
		if(src != nullptr){
			numInputChannels += src->getNumChannels();
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setMasterVolume(float vol){
	mutex.lock();
	masterVolume = vol;
	mutex.unlock();
}
//----------------------------------------------------
float ofxSoundMatrixMixer::getMasterVolume(){
	return masterVolume;
}
//----------------------------------------------------
bool ofxSoundMatrixMixer::isConnected(ofxSoundObject& obj){
	for (int i =0; i<inObjects.size(); i++) {
		if (&obj == inObjects[i].obj) {
			return true;
		}
	}
	return false;
}
//----------------------------------------------------
void ofxSoundMatrixMixer::pullChannel(ofSoundBuffer& buffer, const size_t& chanIndex, const size_t &numFrames, const unsigned int & sampleRate){
	if (inObjects[chanIndex].obj != nullptr ) {
		ofxSoundObject * source = inObjects[chanIndex].obj->getSignalSourceObject();
		if(source != nullptr){			
			size_t nc = source->getNumChannels();
			buffer.resize(nc * numFrames);
			buffer.setNumChannels(nc);
			buffer.setSampleRate(sampleRate);
			inObjects[chanIndex].obj->audioOut(buffer);
			
			if(bComputeRMSandPeak){
				inObjects[chanIndex].vuMeter.calculate(buffer);
			}
			
		}else{
			std::cout << "cant pullChannel. source is null" << std::endl; 
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::mixChannelBufferIntoOutput(const size_t& idx, ofSoundBuffer& input, ofSoundBuffer& output){
	auto nf = output.getNumFrames();
	auto out_nc = output.getNumChannels();
	auto  in_nc = input.getNumChannels();
	
	if(input.getNumFrames() != output.getNumFrames()){
		ofLogWarning("ofxSoundMatrixMixer::mixChannelBufferIntoOutput",  "input and output buffers have different number of frames. these should be equal");
	}
	
	for(size_t ic =0; ic < in_nc; ic++){
		for(size_t oc = 0; oc < out_nc; oc++){
			auto& v = inObjects[idx].channelsVolumes[ic][oc]; 
			if( !ofIsFloatEqual(v.get(), 0.0f)){
				for(size_t i= 0; i < nf; i++){
					output[i * out_nc + oc] += v* input[i * in_nc +ic];
				}
			}
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::audioOut(ofSoundBuffer &output) {
	updateNumOutputChannels(output.getNumChannels());
	if(inObjects.size()>0) {
		output.set(0);
		size_t numFrames = output.getNumFrames();
		unsigned int samplerate = output.getSampleRate();
		for(size_t i = 0; i < inObjects.size(); i++){
			ofSoundBuffer tempBuffer;
			pullChannel(tempBuffer, i, numFrames, samplerate);
			mixChannelBufferIntoOutput(i, tempBuffer, output);
		}
		output*=masterVolume;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::load(const std::string& filename){
	auto extension = ofToLower(ofFilePath::getFileExt(filename));
	ofParameterGroup group;
	putMatrixVolumesIntoParamGroup(group);
	if(extension == "xml"){
		ofXml xml;
		xml.load(filename);
		ofDeserialize(xml, group);
	}else
		if(extension == "json"){
			ofFile jsonFile(filename);
			ofJson json = ofLoadJson(jsonFile);
			ofDeserialize(json, group);
		}else{
			ofLogError("ofxGui") << extension << " not recognized, only .xml and .json supported by now";
		}

}
//----------------------------------------------------
void ofxSoundMatrixMixer::save(const std::string& filename){
	ofParameterGroup group;
	putMatrixVolumesIntoParamGroup(group);
	auto extension = ofToLower(ofFilePath::getFileExt(filename));
	if(extension == "xml"){
		ofXml xml;
		if(ofFile(filename, ofFile::Reference).exists()){
			xml.load(filename);
		}
		ofSerialize(xml, group);
		xml.save(filename);
	}else
		if(extension == "json"){
			ofJson json = ofLoadJson(filename);
			ofSerialize(json, group);
			ofSavePrettyJson(filename, json);
		}else{
			ofLogError("ofxGui") << extension << " not recognized, only .xml and .json supported by now";
		}

}
//----------------------------------------------------
void ofxSoundMatrixMixer::putMatrixVolumesIntoParamGroup(ofParameterGroup & group){
	group.setName("ofxSoundMatrixMixer");
	group.add(masterVol);
	for(size_t idx =0 ; idx < inObjects.size(); idx++ ){	
		for(size_t ic =0; ic < inObjects[idx].channelsVolumes.size(); ic++){
			for(size_t oc = 0; oc < inObjects[idx].channelsVolumes[ic].size(); oc++){
//				std::cout << inObjects[idx].channelsVolumes[ic][oc].getName() << std::endl;
				group.add(inObjects[idx].channelsVolumes[ic][oc]);
			}
		}
	}
}
