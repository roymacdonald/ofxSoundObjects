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
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::disableSliders(){
	slidersGrid.disable();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::toggleSliders(){
	slidersGrid.toggle();
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
namespace{
void addLineToMesh(ofVboMesh& mesh, const glm::vec3& p0, const glm::vec3& p1, const ofFloatColor& color){
	auto i = mesh.getVertices().size();
	mesh.addVertex(p0);
	mesh.addVertex(p1);
	mesh.addIndex(i);
	mesh.addIndex(i+1);
	mesh.addColors({color,color});
}
void addRectToMesh(ofVboMesh& mesh, const ofRectangle& r, const ofFloatColor& color, bool bAddLinesIndices){
	
	auto i = mesh.getVertices().size();
	
	mesh.addVertex(r.getTopLeft());
	mesh.addVertex(r.getTopRight());
	mesh.addVertex(r.getBottomRight());
	mesh.addVertex(r.getBottomLeft());
	mesh.addColors({color,color,color,color});
	
	if(bAddLinesIndices){
		mesh.addIndex(i);
		mesh.addIndex(i+1);
		
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
		
		mesh.addIndex(i+2);
		mesh.addIndex(i+3);
		
		mesh.addIndex(i+3);
		mesh.addIndex(i);
	}else{
		mesh.addIndex(i);
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
		
		mesh.addIndex(i);
		mesh.addIndex(i+2);
		mesh.addIndex(i+3);
		
	}
}
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::buildMeshes(){

//	std::cout << "ofxSoundMatrixMixerRenderer::buildMeshes" << std::endl;
//	std::cout << numChansIn << std::endl;
//	std::cout << numChansOut << std::endl;
//	
	
	mainMesh.clear();
	lineGridMesh.clear();

	
	mainMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	mainMesh.setUsage(GL_STATIC_DRAW);
	
	lineGridMesh.setMode(OF_PRIMITIVE_LINES);
	lineGridMesh.setUsage(GL_STATIC_DRAW);
	
	
	addRectToMesh(mainMesh, drawRect, ofColor(55), false);
	
	leftR.set (drawRect.x, drawRect.y, leftW, drawRect.height - bottomH);
	bottomR.set(leftR.getMaxX(), leftR.getMaxY(), drawRect.width - leftR.width,  bottomH);
	gridR.set (leftR.getMaxX(), leftR.getMinY(), bottomR.width, leftR.height);
	bottomLeftR.set(leftR.x, bottomR.y, leftR.width, bottomR.height);
	
	
	addRectToMesh(lineGridMesh, leftR, ofColor(100), true);
	addRectToMesh(lineGridMesh, bottomR, ofColor(100), true);
	addRectToMesh(lineGridMesh, gridR, ofColor(100), true);
	addRectToMesh(lineGridMesh, bottomLeftR, ofColor(100), true);
	
	slidersGrid.setGridSize(numChansOut, numChansIn);
	
	
	if(obj != nullptr){
	
		for(size_t y = 0; y < numChansIn; y++){
			for(size_t x = 0; x < numChansOut; x++){
				slidersGrid.linkParameter(x, y,
				obj->getVolumeParamForChannel(y, x));
			}
		}
	
	
	
	if(obj->inObjects.size()){
		
		
		if(numChansIn > 0 && numChansOut > 0){
		
			
			
			float outputSliderHeight = 16;
			//	vector<ofRectangle> outChanR (obj->getNumOutputChannels());
			float outRectMargin  = 1;
			
			//	auto nc_out = obj->getNumOutputChannels();
			
			auto brbl =  bottomR.getBottomLeft();
			float w = bottomR.width/numChansOut;
			for(size_t i = 0; i < numChansOut; i++){
				
				addRectToMesh(mainMesh,
							  {brbl.x+ (w*i) + outRectMargin,
								  brbl.y - outputSliderHeight,
								  w - (outRectMargin*2),
								  outputSliderHeight
							  },
							  ofColor(70),
							  false);
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
			std::cout << "ofxSoundMatrixMixerRenderer::draw" << std::endl;
			std::cout << numChansIn << "  " << obj->getNumInputChannels() << std::endl;
			std::cout << numChansOut << "  " << obj->getNumOutputChannels() << std::endl;
			
			
			
			numChansIn = obj->getNumInputChannels();
			numChansOut = obj->getNumOutputChannels();
			
			drawRect = mixerRect;
			
			buildMeshes();
		}
		
		mainMesh.draw();
		lineGridMesh.draw();
		slidersGrid.draw(gridR);
//		slidersGridMesh.draw();
//
//			ofRectangle cell;
//			cell.x = gridR.getMaxX()  - chanW;
//			cell.y = gridR.y;
//			cell.width = gridR.width/ obj->getNumOutputChannels();
////		cell.height = gridR.height/ obj->getNumInputChannels();
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
		
//		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
//			auto & v = obj->inObjects[idx]->channelsVolumes;
//			
//		}
//			objR.height = cell.height * v.size();
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
			
		
		std::stringstream ss;
		
		ss << "Num Output Channels : " << obj->getNumOutputChannels() << std::endl;
		ss << "Num Input Channels  : " << obj->getNumInputChannels() << std::endl;
		ss << "Num Input Objects   : " << obj->inObjects.size() << std::endl;
		
		//		vecToString(ss, obj->matrixInputChannelMap);
		//		vecToString(ss, obj->numConnectionInputChannels);
		//		vecToString(ss, obj->connectionFirstChannel);
		ofDrawBitmapStringHighlight(ss.str(), 20, 20);
	}
}
