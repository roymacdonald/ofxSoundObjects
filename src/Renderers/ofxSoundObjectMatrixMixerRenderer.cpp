//
//  ofxSoundObjectMatrixMixerRenderer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#include "ofxSoundObjectMatrixMixerRenderer.h"

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSoundRendererUtils.h"

using namespace ofxSoundRendererUtils;

//----------------------------------------------------

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
void ofxSoundMatrixMixerRenderer::enableSliders(){
	slidersGrid.enable();
	outputSliders.enable();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::disableSliders(){
	slidersGrid.disable();
	outputSliders.disable();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::toggleSliders(){
	slidersGrid.toggle();
	outputSliders.toggle();
}
//----------------------------------------------------
bool ofxSoundMatrixMixerRenderer::isSlidersEnabled(){
	return slidersGrid.isEnabled();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::setNonSliderMode(bool b){
	bNonSliderMode = b;
	if(bNonSliderMode) disableSliders();
}
//----------------------------------------------------
bool ofxSoundMatrixMixerRenderer::isNonSliderMode(){
	return bNonSliderMode;
}
//----------------------------------------------------
void vecToString(std::stringstream & ss, std::vector<size_t>& vec){
	
	for(auto& v:vec){
		ss << v << ", ";
	}
	ss << std::endl;
}
void ofxSoundMatrixMixerRenderer::setMinChannelHeight(const float& minHeight){
	this->minHeight = minHeight;
}

//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::buildPlayheads(){

	if(obj->inObjects.size()){
		if(numChansIn > 0 && numChansOut > 0){
			
			float cellHeight = gridR.height/ numChansIn;
			
			auto p0 = leftR.getTopLeft();
			auto p1 = leftR.getTopRight();
			playheadsIndices.clear();
			
			float playheadHeight = std::min(cellHeight, 5.0f);
			float vuWidth = 20;
			
			float h =0;
			for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
				auto & v = obj->inObjects[idx]->channelsVolumes;
				
				h = cellHeight * v.size();
				obj->inObjects[idx]->vuMeter.setup(p1.x - vuWidth, p1.y, vuWidth, h , VUMeter::VU_DRAW_VERTICAL, VUMeter::VU_STACK_VERTICAL);
				
				p0.y += h;
				p1.y += h;
				
				if(idx < obj->inObjects.size()-1){
					addLineToMesh(lineGridMesh, p0, p1, ofColor(100));
				}
				
				auto sgnlSrc = obj->inObjects[idx]->obj->getSignalSourceObject();
				auto player = dynamic_cast<ofxSoundPlayerObject*>(sgnlSrc);
				
				if(player){
					playheadsIndices[idx] = player;
					
					addRectToMesh(playheadMesh,{p0.x, p0.y - playheadHeight,
						ofMap(player->getPositionMS(), 0, player->getDurationMS(), 0, leftR.width - vuWidth),
						playheadHeight
					},ofColor(0),false);
				}
			}
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::buildOutputSliders(){
	if(obj){
		if(numChansIn > 0 && numChansOut > 0){
			
			float outputSliderHeight = 16;
			float outRectMargin  = 1;
			
//			auto brbl =  bottomR.getBottomLeft();
//			float w = bottomR.width/numChansOut;
//			for(size_t i = 0; i < numChansOut; i++){
//				
//				addRectToMesh(mainMesh,
//							  {brbl.x+ (w*i) + outRectMargin,
//								  brbl.y - outputSliderHeight,
//								  w - (outRectMargin*2),
//								  outputSliderHeight
//							  },
//							  ofColor(70),
//							  false);
//			}
			
			outputSlidersRect = {
				bottomR.x + outRectMargin,
				bottomR.getMaxY() - outputSliderHeight,
				bottomR.width  - (outRectMargin*2),
				outputSliderHeight
			};
			
			
			
			outputSliders.setGridSize(obj->outputChannels.size(), 1);
			
			for(size_t i = 0; i< obj->outputChannels.size(); i++){
				outputSliders.linkParameter(i, 0,obj->outputChannels[i].volume);
			}
			
//			ofRectangle r (bottomR.x + outRectMargin, bottomR.getMaxY() - (outputSliderHeight*2), bottomR.width  - (outRectMargin*2), outputSliderHeight);
////			cout << "outVuMeter rect " << r << endl;
			auto r = outputSlidersRect;
			r.y -= outputSliderHeight;
			obj->outVuMeter.setup(r,VUMeter::VU_DRAW_HORIZONTAL, VUMeter::VU_STACK_HORIZONTAL);
		}
	}
}

//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::buildSlidersGrid(){
	slidersGrid.setGridSize(numChansOut, numChansIn);
	if(obj != nullptr){

		for(size_t y = 0; y < numChansIn; y++){
			for(size_t x = 0; x < numChansOut; x++){
				slidersGrid.linkParameter(x, y,
										  obj->getVolumeParamForChannel(y, x));
			}
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::buildMeshes(){


	mainMesh.clear();
	lineGridMesh.clear();
	playheadMesh.clear();
	
	mainMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	mainMesh.setUsage(GL_STATIC_DRAW);
	
	lineGridMesh.setMode(OF_PRIMITIVE_LINES);
	lineGridMesh.setUsage(GL_STATIC_DRAW);
	
	playheadMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	playheadMesh.setUsage(GL_STATIC_DRAW);
	
	
	addRectToMesh(mainMesh, drawRect, ofColor(55), false);
	
	leftR.set (drawRect.x, drawRect.y, leftW, drawRect.height - bottomH);
	bottomR.set(leftR.getMaxX(), leftR.getMaxY(), drawRect.width - leftR.width,  bottomH);
	gridR.set (leftR.getMaxX(), leftR.getMinY(), bottomR.width, leftR.height);
	bottomLeftR.set(leftR.x, bottomR.y, leftR.width, bottomR.height);
	
	
	addRectToMesh(lineGridMesh, leftR, ofColor(100), true);
	addRectToMesh(lineGridMesh, bottomR, ofColor(100), true);
	addRectToMesh(lineGridMesh, gridR, ofColor(100), true);
	addRectToMesh(lineGridMesh, bottomLeftR, ofColor(100), true);
	
	buildSlidersGrid();
	
	
	buildOutputSliders();
	buildPlayheads();
		
	
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::updatePlayheads(){
	
	auto & v = playheadMesh.getVertices();
	
	float mn = leftR.getMinX();
	float mx = leftR.getMaxX();
	for(auto& p: playheadsIndices){
		if(p.second){
			if(p.second->isPlaying()){
			auto i = p.first *4;
			if(i < v.size() -3){
				
				float pos = ofMap(p.second->getPositionMS(), 0, p.second->getDurationMS(), mn, mx);
				v[i+1].x = pos;
				v[i+2].x = pos;
			}
			}
		}
	}
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::draw(const ofRectangle& mixerRect){
	
	if(obj != nullptr){
		

		if(numChansIn != obj->getNumInputChannels() ||
		   numChansOut != obj->getNumOutputChannels() ||
		   drawRect != mixerRect){
//			std::cout << "ofxSoundMatrixMixerRenderer::draw" << std::endl;
//			std::cout << numChansIn << "  " << obj->getNumInputChannels() << std::endl;
//			std::cout << numChansOut << "  " << obj->getNumOutputChannels() << std::endl;
			
			
			
			numChansIn = obj->getNumInputChannels();
			numChansOut = obj->getNumOutputChannels();
			
			drawRect = mixerRect;
			buildMeshes();
		}
		
		mainMesh.draw();
		lineGridMesh.draw();
		slidersGrid.draw(gridR);
		outputSliders.draw(outputSlidersRect);
		
//		ofPushStyle();
//		ofNoFill();
//		ofSetColor(ofColor::yellow);
//		ofDrawRectangle(slidersGrid.gridRect);
//		ofSetColor(ofColor::blue);
//		ofDrawRectangle(outputSliders.gridRect);
//		ofPopStyle();
		
		
		
		updatePlayheads();

		playheadMesh.draw();
		
//
//			ofRectangle cell;
//			cell.x = gridR.getMaxX()  - chanW;
//			cell.y = gridR.y;
//			cell.width = gridR.width/ obj->getNumOutputChannels();
//			float cellHeight = gridR.height/ obj->getNumInputChannels();
//			cell.height = std::max(minHeight, gridR.height/ obj->getNumInputChannels());
//
//			bool bDisableTextRendering = (cell.height < 16.0f);



		//TO-DO this logic needs some love 
//		if(bDisableTextRendering){
//			if(bSlidersEnabled){
//				bSlidersWereEnabled = true;
//				setNonSliderMode(true);
//			}
//		}else if(bSlidersWereEnabled && !bSlidersEnabled){
//			enableSliders();
//		}
		
		// end draw output channels
		
		// start draw grid
		
//		if(obj->inObjects.size()){
//		auto p0 = gridR.getTopLeft();
//		auto p1 = gridR.getTopRight();
//		for(size_t idx =0 ; idx < obj->inObjects.size()-1; idx++ ){
//			auto & v = obj->inObjects[idx]->channelsVolumes;
////
////		}
//			
//			p0.y += cellHeight * v.size();
//			p1.y += cellHeight * v.size();
//			
//			chanR.y = objR.y;
//			
//			drawRect(objR);			
//			ofSetColor(255);
			
			
			
//			auto sgnlSrc = obj->inObjects[idx]->obj->getSignalSourceObject();
//			auto player = dynamic_cast<ofxSoundPlayerObject*>(sgnlSrc);
//			
//			// input object info header rect draw start
//			if(player){
//				if(!bDisableTextRendering){
//					auto info = getSoundFileInfo(player->getSoundFile());
//					auto bb = bf.getBoundingBox(info, 0,0);
//					ofDrawBitmapString(info, objR.x - bb.x, objR.y - bb.y);
//				}
//				ofRectangle posR = objR;
//				posR.height = std::min(cell.height, 10.0f);
//				posR.y = objR.getMaxY() - posR.height;
//				posR.width = ofMap(player->getPositionMS(), 0, player->getDurationMS(), 0, objR.width);
//				
//				ofSetColor(100);
//				ofDrawRectangle(posR);
//				
//				
//			}else{
//				if(!bDisableTextRendering){
//					auto liveScr = dynamic_cast<ofxSoundInput*>(sgnlSrc);
//					if(liveScr){
//						ofDrawBitmapString(ofxSoundUtils::getSoundDeviceString(liveScr->getDeviceInfo(), true, false), objR.x, objR.getMinY()+20);
//					}else{
//						ofDrawBitmapString(ofToString(v.size()), objR.x, objR.getMaxY() - 20);
//					}
//				}
//			}
			// input object info header rect draw end
//		}
//		}
		
		
		
		
		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
			obj->inObjects[idx]->vuMeter.draw();
		}
		obj->outVuMeter.draw();
		
		std::stringstream ss;
//
		
		
		
		ss << "Num Output Channels : " << numChansOut << std::endl;
		ss << "Num Input Channels  : " << numChansIn << std::endl;
//		ss << "Num Input Objects   : " << obj->inObjects.size() << std::endl;
		
		//		vecToString(ss, obj->matrixInputChannelMap);
		//		vecToString(ss, obj->numConnectionInputChannels);
		//		vecToString(ss, obj->connectionFirstChannel);
		ofDrawBitmapStringHighlight(ss.str(), 20, 20);
	}
}
