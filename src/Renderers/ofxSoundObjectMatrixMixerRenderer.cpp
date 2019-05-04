//
//  ofxSoundObjectMatrixMixerRenderer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#include "ofxSoundObjectMatrixMixerRenderer.h"
#include "ofxSoundPlayerObject.h"
#include "ofMain.h"
#include "ofxGui.h"

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
		float chanW = 10;
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
		
		if(sliders.size() != obj->inObjects.size())sliders.resize(obj->inObjects.size());
		
		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
			auto & v = obj->inObjects[idx].channelsVolumes;
			if(sliders[idx].size() != v.size()) sliders[idx].resize(v.size());
			for(size_t i = 0; i <sliders[idx].size(); i++){
				if(sliders[idx][i].size() != v[i].size()) sliders[idx][i].resize(v[i].size());
				for(size_t o = 0; o < sliders[idx][i].size(); o++){
					if(!sliders[idx][i][o]){
						sliders[idx][i][o] = make_unique<ofxFloatSlider>(v[i][o], cell.width);
					}					
				}
			}
			
			
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
			ofRectangle rmsR;
			for(size_t i = 0; i < v.size(); i++){
				cell.y = gridR.y + (cell.height * (i + cellCount));
				chanR.y = cell.y;
				drawRect(chanR);
				if(obj->bComputeRMSandPeak && i < obj->inObjects[idx].rmsVolume.size()){
					rmsR = chanR;
					rmsR.height = ofMap(obj->inObjects[idx].rmsVolume[i], 0, 1, 0, chanR.height);
					rmsR.y = chanR.getMaxY() - rmsR.height; 
					ofSetColor(100);
					ofDrawRectangle(rmsR);
				}
				
				for(size_t j = 0; j < v[i].size(); j++){
					cell.x = gridR.x + cell.width*j; 	 
					drawRect(cell);
					ofSetColor(255);
					
					auto &s = sliders[idx][i][j]; 
					if(s){
						auto p = cell.getBottomLeft();
						p.y -= s->getHeight();
						if(s->getPosition() != p){
							s->setPosition(p);
						}
						s->draw();
					}
//					ofDrawBitmapString(ofToString(v[i][j]), cell.x, cell.getMaxY());
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
