//
//  ofxSoundMatrixMixer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 4/28/19.
//
//

#include "ofxSoundMatrixMixer.h"
#include "ofxSoundUtils.h"
#include "ofxSoundPlayerObject.h"
//----------------------------------------------------
ofxSoundMatrixMixer::ofxSoundMatrixMixer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
//	masterVolume = 1.0f;	
	masterVol.set("Master Vol", 1, 0, 1);
//	masterVol.addListener(this, &ofxSoundMatrixMixer::masterVolChanged);
	outVuMeter.drawMode = VUMeter::VU_DRAW_HORIZONTAL;

}
////----------------------------------------------------
//void ofxSoundMatrixMixer::masterVolChanged(float& f) {
//	mutex.lock();
//	masterVolume = masterVol;
//	mutex.unlock();
//}
//----------------------------------------------------
ofxSoundMatrixMixer::~ofxSoundMatrixMixer(){
//	masterVol.removeListener(this, &ofxSoundMatrixMixer::masterVolChanged);
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
		bNeedsInputNumUpdate = true;
//		updateNumInputChannels();
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
		size_t i = outputVolumes.size(); 
		outputVolumes.resize(numOutputChannels);
		for( ; i < numOutputChannels; i++){
			outputVolumes[i].set("out " + ofToString(i), 0, 0, 1);
		}
		
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setVolumeForConnectionChannel(const float& volValue, const size_t& connectionIndex, const size_t& inputChannel, const size_t& outputChannel){
	if(connectionIndex < inObjects.size()){
		if(inputChannel < inObjects[connectionIndex].channelsVolumes.size()){
			if(outputChannel < inObjects[connectionIndex].channelsVolumes[inputChannel].size()){
				inObjects[connectionIndex].channelsVolumes[inputChannel][outputChannel] = volValue;
			}else{
				ofLogWarning("ofxSoundMatrixMixer::setVolumeForConnectionChannel") << "outputChannel index out of bounds";
			}
		}else{
			ofLogWarning("ofxSoundMatrixMixer::setVolumeForConnectionChannel") << "inputChannel " << inputChannel << " index out of bounds";
		}
	}else{
		ofLogWarning("ofxSoundMatrixMixer::setVolumeForConnectionChannel") << "connection index out of bounds";
	}
}
//----------------------------------------------------
const float& ofxSoundMatrixMixer::getVolumeForConnectionChannel(const size_t& connectionIndex, const size_t& inputChannel, const size_t& outputChannel) const{
	if(connectionIndex < inObjects.size()){
		if(inputChannel < inObjects[connectionIndex].channelsVolumes.size()){
			if(outputChannel < inObjects[connectionIndex].channelsVolumes[inputChannel].size()){
				return inObjects[connectionIndex].channelsVolumes[inputChannel][outputChannel];
			}else{
				ofLogWarning("ofxSoundMatrixMixer::getVolumeForConnectionChannel") << "outputChannel index out of bounds";
			}
		}else{
			ofLogWarning("ofxSoundMatrixMixer::getVolumeForConnectionChannel") << "inputChannel index out of bounds";
		}
	}else{
		ofLogWarning("ofxSoundMatrixMixer::getVolumeForConnectionChannel") << "connection index out of bounds";
	}
	
	return dummyFloat;
}


void ofxSoundMatrixMixer::setOutputVolumeForChannel (const float & volValue, const size_t& outputChannel){
	if(outputChannel < outputVolumes.size()){
		outputVolumes[outputChannel] = volValue;
	}else{
		ofLogWarning("ofxSoundMatrixMixer::setOutputVolumeForChannel") << " outputChannel out of range";
	}
}
const float & ofxSoundMatrixMixer::getOutputVolumeForChannel ( const size_t& outputChannel) const{
	if(outputChannel < outputVolumes.size()){
		return outputVolumes[outputChannel].get();
	}
	ofLogWarning("ofxSoundMatrixMixer::setOutputVolumeForChannel") << " outputChannel out of range";
}
//----------------------------------------------------
//----------------------------------------------------
void ofxSoundMatrixMixer::setVolumeForChannel(const float& volValue, const size_t& inputChannel, const size_t& outputChannel){
	size_t c = getConnectionIndexAtInputChannel(inputChannel);
	size_t i = getFirstInputChannelForConnection(c);
	setVolumeForConnectionChannel(volValue, c, inputChannel - i, outputChannel);
	
}
//----------------------------------------------------
const float& ofxSoundMatrixMixer::getVolumeForChannel(const size_t& inputChannel, const size_t& outputChannel) const{
	if(inputChannel < matrixInputChannelMap.size()){
		auto& connectionIndex = matrixInputChannelMap[inputChannel];
		if(connectionIndex < inObjects.size()){
			if(inputChannel < inObjects[connectionIndex].channelsVolumes.size()){
				if(outputChannel < inObjects[connectionIndex].channelsVolumes[inputChannel].size()){
					return inObjects[connectionIndex].channelsVolumes[inputChannel][outputChannel].get();
				}else{
					ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "outputChannel index out of bounds";
				}
			}else{
				ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "inputChannel index out of bounds";
			}
		}else{
			ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "connection index out of bounds";
		}
	}else{
		ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "input channel index out of bounds";
	}
	return dummyFloat;	
}
size_t ofxSoundMatrixMixer::getConnectionIndexAtInputChannel(const size_t& chan){
	if(chan < matrixInputChannelMap.size()){
		return matrixInputChannelMap[chan];
	}
	ofLogWarning("ofxSoundMatrixMixer::getConnectionIndexAtInputChannel") << "channel index out of bounds";
	return 0;
}

 //----------------------------------------------------
size_t ofxSoundMatrixMixer::getFirstInputChannelForConnection(const size_t& connectionIndex){
	if(connectionIndex < connectionFirstChannel.size()){
		return connectionFirstChannel[connectionIndex];
	}
	ofLogWarning("ofxSoundMatrixMixer::getFirstInputChannelForConnection") << "connection index out of range";
	return 0; 
}


//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumInputChannels(){
	if(bNeedsInputNumUpdate){
		bNeedsInputNumUpdate = false;
		numInputChannels = 0;
		
		ofxSoundObject * src = nullptr;
		
		ofxSoundUtils::resize_vec(numConnectionInputChannels, inObjects.size()); 
		ofxSoundUtils::resize_vec(connectionFirstChannel, inObjects.size());
		
		for(size_t i = 0; i < inObjects.size(); i++){
			src = inObjects[i].obj->getSignalSourceObject();
			if(src != nullptr){
				numConnectionInputChannels [i] = src->getNumChannels();
				connectionFirstChannel[i] = numInputChannels; 
				numInputChannels += src->getNumChannels();
			}
		}
		ofxSoundUtils::resize_vec(matrixInputChannelMap, numInputChannels);
		
		size_t count = 0;
		for(size_t i = 0; i < inObjects.size(); i++){
			for(size_t j = 0; j < numConnectionInputChannels[i]; j++){
				matrixInputChannelMap[count + j] = i;
			}
			count += numConnectionInputChannels[i];
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setMasterVolume(float vol){
	mutex.lock();
	masterVol = vol;
	mutex.unlock();
}
//----------------------------------------------------
float ofxSoundMatrixMixer::getMasterVolume(){
	return masterVol.get();
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
			auto player = dynamic_cast<ofxSoundPlayerObject*>(source);
			if((player && player->isPlaying()) || !player ){// this is to avoid pulling audio when the player is not playing
				size_t nc = source->getNumChannels();
				buffer.resize(nc * numFrames);
				buffer.setNumChannels(nc);
				buffer.setSampleRate(sampleRate);
				inObjects[chanIndex].obj->audioOut(buffer);
				inObjects[chanIndex].bBufferProcessed = true;
				if(bComputeRMSandPeak){
					inObjects[chanIndex].vuMeter.calculate(buffer);
				}
			}else{
				inObjects[chanIndex].bBufferProcessed = false;
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
	auto in_nc = input.getNumChannels();
	
	if(input.getNumFrames() != output.getNumFrames()){
		ofLogWarning("ofxSoundMatrixMixer::mixChannelBufferIntoOutput",  "input and output buffers have different number of frames. these should be equal");
	}
	if(inObjects[idx].bBufferProcessed){
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
		
		updateNumInputChannels();
		
		auto nf = output.getNumFrames();
		auto nov = outputVolumes.size();
		for(size_t i =0; i < nov; i++){
			for(size_t f = 0; f < nf; f++){
				output[f * nov + i] *= outputVolumes[i].get();
			}
		}
		
		if(bComputeRMSandPeak){
			outVuMeter.calculate(output);
		}
		
		output*=masterVol;
		
		
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
	for(auto& o : outputVolumes){
		group.add(o);
	}
}
