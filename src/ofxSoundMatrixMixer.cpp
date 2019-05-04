//
//  ofxSoundMatrixMixer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 4/28/19.
//
//

#include "ofxSoundMatrixMixer.h"
#include "ofxSoundPlayerObject.h"
#include "ofMain.h"
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
	inObjects.push_back(MatrixInputObject(obj, numOutputChannels));
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
		for(auto& i : inObjects){
			i.updateChanVolsSize(numOutputChannels);
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
			if( !ofIsFloatEqual(v, 0.0f)){
				for(size_t i= 0; i < nf; i++){
					output[i * out_nc + oc] += v* input[i * in_nc +ic];
				}
			}
		}
	}
}
//----------------------------------------------------
// this pulls the audio through from earlier links in the chain and sums up the total output
void ofxSoundMatrixMixer::audioOut(ofSoundBuffer &output) {
	updateNumOutputChannels(output.getNumChannels());
	if(inObjects.size()>0) {
		size_t numFrames = output.getNumFrames();
		unsigned int samplerate = output.getSampleRate();
		for(size_t i = 0; i < inObjects.size(); i++){
			ofSoundBuffer tempBuffer;
			pullChannel(tempBuffer, i, numFrames, samplerate);
			
		}
		output*=masterVolume;
	}
}
//----------------------------------------------------

void drawRect(const ofRectangle& r){
	ofSetColor(55);
	ofNoFill();
	ofDrawRectangle(r);
	ofFill();
	ofSetColor(0);
	ofDrawRectangle(r);
}
std::string getSoundFileInfo(const ofxSoundFile& f){
	std:stringstream ss;
	ss << ofFilePath::getBaseName(f.getPath())  << std::endl;
	ss << "  Duration " << f.getDuration() << std::endl;
	ss << "  NumChannels " << f.getNumChannels() << std::endl;
	ss << "  SampleRate " << f.getSampleRate() << std::endl;
	ss << "  NumSamples " << f.getNumSamples();
	return ss.str();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::draw(){

	if(obj != nullptr){
		
		glm::vec2 margin = {20, 80};
		float leftW = 200;
		float bottomH = 100;
		float chanW = 50;
		ofRectangle leftR (margin.x, margin.y, leftW, ofGetHeight() - margin.y - margin.x - bottomH);
		ofRectangle bottomR(leftR.getMaxX(), leftR.getMaxY(), ofGetWidth() - margin.x - leftR.getMaxX(),  bottomH);
		
		ofRectangle gridR (leftR.getMaxX(), leftR.getMinY(), bottomR.width, leftR.height);
		
		
		ofRectangle cell;
		cell.x = gridR.getMaxX()  - chanW;
		cell.y = gridR.y;
		cell.width = gridR.width/ obj->getNumOutputChannels();
		cell.height = gridR.height/ obj->getNumInputChannels();  

		drawRect(leftR);
		drawRect(bottomR);
		drawRect(gridR);
		
		size_t cellCount = 0;
		ofBitmapFont bf;
		ofRectangle objR(leftR.x, leftR.y, leftW - chanW, leftR.height / obj->inObjects.size());
		ofRectangle chanR(objR.getMaxX(), objR.y, chanW, cell.height);
		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
			auto & v = obj->inObjects[idx].channelsVolumes;
			
			objR.height = cell.height * v.size();
			chanR.y = objR.y;
			
			drawRect(objR);			
			ofSetColor(255);
			
			
			
			
			auto src = dynamic_cast<ofxSoundPlayerObject*>(obj->inObjects[idx].obj->getSignalSourceObject());
			if(src){
				auto info = getSoundFileInfo(src->getSoundFile());
				auto bb = bf.getBoundingBox(info, 0,0);
				ofDrawBitmapString(info, objR.x - bb.x, objR.y - bb.y);
				
				ofRectangle posR = objR;
				posR.height = 10;
				posR.y = objR.getMaxY() - posR.height;
				posR.width = ofMap(src->getPositionMS(), 0, src->getDurationMS(), 0, objR.width);
				
				ofSetColor(100);
				ofDrawRectangle(posR);
				
			}else{
				ofDrawBitmapString(ofToString(v.size()), objR.x, objR.getMaxY());
			}
			objR.y += objR.height;
			
			for(size_t i = 0; i < v.size(); i++){
				cell.y = gridR.y + (cell.height * (i + cellCount));
				chanR.y = cell.y;
				drawRect(chanR);
				for(size_t j = 0; j < v[i].size(); j++){
					cell.x = gridR.x + cell.width*j; 	 
					drawRect(cell);
					ofSetColor(255);
					
					ofDrawBitmapString(ofToString(v[i][j]), cell.x, cell.getMaxY());
				}
			}
			cellCount += v.size();
			
		}
		std::stringstream ss;
		
		ss << "Num Output Channels : " << obj->getNumOutputChannels() << std::endl;
		ss << "Num Input Channels  : " << obj->getNumInputChannels() << std::endl;
		ss << "Num Input Objects   : " << obj->inObjects.size() << std::endl;
		ofDrawBitmapStringHighlight(ss.str(), margin.x, margin.x);
	}
	
	
}
