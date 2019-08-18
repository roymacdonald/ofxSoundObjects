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

//--------------------------------------------------------------------------------------------------------
//---------------------------------------   MatrixInputObject      --------------------------------------- 
//--------------------------------------------------------------------------------------------------------

//----------------------------------------------------
bool ofxSoundMatrixMixer::MatrixInputObject::pullChannel(){
	bBufferProcessed = false;
	if (obj != nullptr ) {
		ofxSoundObject * source = obj->getSignalSourceObject();
		if(source != nullptr){			
			auto player = dynamic_cast<ofxSoundPlayerObject*>(source);
			if((player && player->isPlaying()) || !player ){// this is to avoid pulling audio when the player is not playing
				size_t nc = source->getNumChannels();
				buffer.setSampleRate(sampleRate);
				buffer.allocate(this->numFramesToProcess, nc);
				obj->audioOut(buffer);
				bBufferProcessed = true;
				if(ofxSoundMatrixMixer::getComputeRMSandPeak()){
					vuMeter.calculate(buffer);
				}
				return true;
			}
		}else{
			std::cout << "cant pullChannel. source is null" << std::endl;
		}
	}
	return false;
}

//----------------------------------------------------
ofxSoundMatrixMixer::MatrixInputObject::MatrixInputObject(ofxSoundObject* _obj, const size_t& numOutChanns, const size_t& chanCount):obj(_obj){
	updateChanVolsSize(numOutChanns, chanCount);
}

//----------------------------------------------------
ofSoundBuffer & ofxSoundMatrixMixer::MatrixInputObject::getBuffer(){
	return buffer;
}
//----------------------------------------------------
bool ofxSoundMatrixMixer::MatrixInputObject::updateChanVolsSize(const size_t& numOutChanns, const size_t& chanCount ){
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
						channelsVolumes[i][o].set("chan "+ofToString(chanCount + i) + " : " + ofToString(o), 1,0,1);
					}
				}
			}
		}
	}
	return bUpdated;
}



//--------------------------------------------------------------------------------------------------------
//----------------------------------------   ofxSoundMatrixMixer  ---------------------------------------- 
//--------------------------------------------------------------------------------------------------------
ofxSoundMatrixMixer::ofxSoundMatrixMixer():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	chanMod = OFX_SOUND_OBJECT_CHAN_MIXER;
	//	masterVolume = 1.0f;	
	masterVol.set("Master Vol", 1, 0, 1);
	//	masterVol.addListener(this, &ofxSoundMatrixMixer::masterVolChanged);
	outVuMeter.drawMode = VUMeter::VU_DRAW_HORIZONTAL;
	
}
//----------------------------------------------------
ofParameter<bool>& ofxSoundMatrixMixer::getComputeRMSandPeak(){
	static std::unique_ptr<ofParameter<bool>> i;
	if(!i){
		i = make_unique<ofParameter<bool>>();
		i->set("Compute RMS / Peak", true);
	}
	return *i;
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
		return inObjects[objectNumber]->obj;
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
		return inObjects[channelNumber]->obj;
	}else{
		return nullptr;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::disconnectInput(ofxSoundObject * input){
	if(input!=nullptr)
		ofRemove(inObjects, [&](shared_ptr<MatrixInputObject>& o){return input == o->obj;});
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setInput(ofxSoundObject *obj){
	if(obj){
		for (int i =0; i<inObjects.size(); i++) {
			if (obj == inObjects[i]->obj) {
				ofLogNotice("ofxSoundMatrixMixer::setInput") << " already connected" ;
				return;
			}
		}
		
		inObjects.push_back(std::make_shared<MatrixInputObject>(obj, numOutputChannels, numInputChannels));
		bNeedsInputNumUpdate = true;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::updateNumOutputChannels(const size_t & nc){
	if(numOutputChannels != nc){
//		cout << "numOutputChannels " << numOutputChannels << "   nc " << nc << endl;
		numOutputChannels = nc;
		size_t count = 0;
		for(auto& i : inObjects){
			i->updateChanVolsSize(numOutputChannels, count);
			count += i->channelsVolumes.size();
		}
		size_t i = outputChannels.size(); 
		outputChannels.resize(numOutputChannels);
		for( ; i < numOutputChannels; i++){
			outputChannels[i].setup("out " + ofToString(i));
		}
		
	}
}
//----------------------------------------------------
const VUMeter& ofxSoundMatrixMixer::getVUMeterForConnection(const size_t& connectionIndex){
	if(connectionIndex < inObjects.size()){
		return inObjects[connectionIndex]->vuMeter;
	}
	ofLogWarning("ofxSoundMatrixMixer::getVUMeterForConnection") << "Connection index is out of bounds. ";
	return outVuMeter;
}
//----------------------------------------------------
const VUMeter& ofxSoundMatrixMixer::getOutputVUMeter(){
	return outVuMeter;
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setVolumeForConnectionChannel(const float& volValue, const size_t& connectionIndex, const size_t& inputChannel, const size_t& outputChannel){
	if(connectionIndex < inObjects.size()){
		if(inputChannel < inObjects[connectionIndex]->channelsVolumes.size()){
			if(outputChannel < inObjects[connectionIndex]->channelsVolumes[inputChannel].size()){
				inObjects[connectionIndex]->channelsVolumes[inputChannel][outputChannel] = volValue;
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
		if(inputChannel < inObjects[connectionIndex]->channelsVolumes.size()){
			if(outputChannel < inObjects[connectionIndex]->channelsVolumes[inputChannel].size()){
				return inObjects[connectionIndex]->channelsVolumes[inputChannel][outputChannel];
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
//----------------------------------------------------
size_t ofxSoundMatrixMixer::getConnectionNumberOfChannels(const size_t& connectionIndex){
	if(connectionIndex < inObjects.size()){
		return inObjects[connectionIndex]->channelsVolumes.size();
	}
	ofLogWarning("ofxSoundMatrixMixer::getConnectionNumberOfChannels") << "connection index out of bounds";
	return 0;
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setOutputVolumeForAllChannels(const float & volValue){
	for(auto& o: outputChannels){
		o.volume = volValue;
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setOutputVolumeForChannel (const float & volValue, const size_t& outputChannel){
	if(outputChannel < outputChannels.size()){
		outputChannels[outputChannel].volume = volValue;
	}else{
		ofLogWarning("ofxSoundMatrixMixer::setOutputVolumeForChannel") << " outputChannel out of range";
	}
}
//----------------------------------------------------
const float & ofxSoundMatrixMixer::getOutputVolumeForChannel ( const size_t& outputChannel) const{
	if(outputChannel < outputChannels.size()){
		return outputChannels[outputChannel].volume.get();
	}
	ofLogWarning("ofxSoundMatrixMixer::setOutputVolumeForChannel") << " outputChannel out of range";
	return dummyFloat;
}
//----------------------------------------------------
void ofxSoundMatrixMixer::setVolumeForChannel(const float& volValue, const size_t& inputChannel, const size_t& outputChannel){
	size_t c = getConnectionIndexAtInputChannel(inputChannel);
	size_t i = getFirstInputChannelForConnection(c);
	setVolumeForConnectionChannel(volValue, c, inputChannel - i, outputChannel);
	
}
//----------------------------------------------------
const float& ofxSoundMatrixMixer::getVolumeForChannel(const size_t& inputChannel, const size_t& outputChannel){
	return getVolumeParamForChannel(inputChannel, outputChannel).get();
}
//----------------------------------------------------
ofParameter<float>& ofxSoundMatrixMixer::getVolumeParamForChannel(const size_t& inputChannel, const size_t& outputChannel) {
		if(inputChannel < matrixInputChannelMap.size()){
			auto& connectionIndex = matrixInputChannelMap[inputChannel];
			if(connectionIndex < inObjects.size()){
				auto i = inputChannel - getFirstInputChannelForConnection(connectionIndex);
				if(i < inObjects[connectionIndex]->channelsVolumes.size()){
					if(outputChannel < inObjects[connectionIndex]->channelsVolumes[i].size()){
						return inObjects[connectionIndex]->channelsVolumes[i][outputChannel];
					}else{
						ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "outputChannel index out of bounds";
					}
				}else{
					ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "inputChannel index out of bounds: " << i << " " << inObjects[connectionIndex]->channelsVolumes.size();
				}
			}else{
				ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "connection index out of bounds";
			}
		}else{
			ofLogWarning("ofxSoundMatrixMixer::getVolumeForChannel") << "input channel index out of bounds";
		}
		return dummyFloat;
}
//----------------------------------------------------
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
			src = inObjects[i]->obj->getSignalSourceObject();
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
		if (&obj == inObjects[i]->obj) {
			return true;
		}
	}
	return false;
}
bool ofxSoundMatrixMixer::MatrixInputObject::addIntoOutputChannel(const size_t& oc, ofSoundBuffer& output){
	if (obj != nullptr ) {
		auto nf = output.getNumFrames();
		auto out_nc = output.getNumChannels();
		auto in_nc = buffer.getNumChannels();
		
//		auto& cv = inObjects[idx]->channelsVolumes;
		for(size_t ic = 0; ic < in_nc; ic++){
			auto& v = channelsVolumes[ic][oc].get();
			if( !ofIsFloatEqual(v, 0.0f)){
				for(size_t i= 0; i < nf; i++){
					output[i * out_nc + oc] += v* buffer[i * in_nc +ic];
				}
			}
		}
		return true;
	}
	return false;
}
//----------------------------------------------------
void ofxSoundMatrixMixer::mixChannelBufferIntoOutput(const size_t& idx, ofSoundBuffer& output, bool bUseOldImplementation){
	// for optimizing, some objects, like the sound player will return an empty buffer when not playing
	auto & input = inObjects[idx]->getBuffer();
	if(input.getNumFrames() == 0 || !inObjects[idx]->bBufferProcessed) return;
	
	size_t out_nc = output.getNumChannels();
	
	
	
	if(input.getNumFrames() != output.getNumFrames()){
		ofLogWarning("ofxSoundMatrixMixer::mixChannelBufferIntoOutput") <<  "input and output buffers have different number of frames. these should be equal. in: " << input.getNumFrames() << " out: " << output.getNumFrames();
	}
	
	if(inObjects[idx]->bBufferProcessed){
	
#ifdef OFX_SOUND_ENABLE_MULTITHREADING
	ChannelAdder adder(inObjects[idx], output);
		// esto esta mal. no va a funcionar. hay que iterar por sobre los canales de salida
	tbb::parallel_for( tbb::blocked_range<size_t>( 0, out_nc),  adder);
#else
		auto nf = output.getNumFrames();
		size_t in_nc = std::min(input.getNumChannels(), inObjects[idx]->channelsVolumes.size());
		
		if(bUseOldImplementation){
			auto& cv = inObjects[idx]->channelsVolumes;
			for(size_t ic =0; ic < in_nc; ic++){
				for(size_t oc = 0; oc < out_nc && oc < cv[ic].size(); oc++){
					auto& v = cv[ic][oc];
					if( !ofIsFloatEqual(v.get(), 0.0f)){
						for(size_t i= 0; i < nf; i++){
							output[i * out_nc + oc] += v* input[i * in_nc +ic];
						}
					}
				}
			}
		}else{
			for(size_t oc = 0; oc < out_nc; oc++){
//			for(size_t ic =0; ic < in_nc; ic++){
				inObjects[idx]->addIntoOutputChannel(oc, output);
			}
		}
#endif
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixer::audioOut(ofSoundBuffer &output) {
	updateNumOutputChannels(output.getNumChannels());
	
	if(inObjects.size()>0) {
		output.set(0);
		size_t numFrames = output.getNumFrames();
		unsigned int samplerate = output.getSampleRate();
		updateNumInputChannels();
		for(size_t i = 0; i < inObjects.size(); i++){	
			inObjects[i]->sampleRate = samplerate;
			inObjects[i]->numFramesToProcess = numFrames;
#ifdef OFX_SOUND_ENABLE_MULTITHREADING
		}
		size_t n = inObjects.size();
		MatrixInputsCollection col;
		col._inObjects = &inObjects;
		tbb::parallel_for( tbb::blocked_range<size_t>( 0, n ),  col);

		for(size_t i = 0; i < inObjects.size(); i++){
			mixChannelBufferIntoOutput(i, output);
		}
#else
			if(inObjects[i]->pullChannel()){
				mixChannelBufferIntoOutput(i, output);
			}
		}
#endif
		
		
		
		auto nf = output.getNumFrames();
		auto nov = outputChannels.size();
		for(size_t i =0; i < nov; i++){
			for(size_t f = 0; f < nf; f++){
				output[f * nov + i] *= outputChannels[i].volume.get();
			}
		}
		
		if(getComputeRMSandPeak()){
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
		for(size_t ic =0; ic < inObjects[idx]->channelsVolumes.size(); ic++){
			for(size_t oc = 0; oc < inObjects[idx]->channelsVolumes[ic].size(); oc++){
				//				std::cout << inObjects[idx].channelsVolumes[ic][oc]->getName() << std::endl;
				group.add(inObjects[idx]->channelsVolumes[ic][oc]);
			}
		}
	}
	for(auto& o : outputChannels){
		group.add(o);
	}
}
