//
//  ofxSoundObjectMatrixMixerRenderer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#include "ofxSoundObjectMatrixMixerRenderer.h"

#include "ofMain.h"
//#include "ofxGui.h"
#include "ofxSoundRendererUtils.h"
#include "ofxSingleSoundPlayer.h"

using namespace ofxSoundRendererUtils;

//----------------------------------------------------

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
			
			
			float h =0;

			bDisableTextRendering = (cellHeight < 16.0f);
			textMeshes.clear();
			string txt;
			ofBitmapFont bf;
			float y =0;
			for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
				auto & v = obj->inObjects[idx]->channelsVolumes;
				
				h = cellHeight * v.size();
				
				obj->inObjects[idx]->vuMeter.setup({p1.x - vuWidth, p1.y, vuWidth, h} , VUMeter::VU_DRAW_VERTICAL, VUMeter::VU_STACK_VERTICAL);
				
				p0.y += h;
				p1.y += h;
				
				if(idx < obj->inObjects.size()-1){
					addLineToMesh(lineGridMesh, p0, p1, ofColor(100));
				}
				y = p0.y;
				
				auto sgnlSrc = obj->inObjects[idx]->obj->getSignalSourceObject();
				auto player = dynamic_cast<ofxBaseSoundPlayer*>(sgnlSrc);
				
				if(player){
					playheadsIndices[idx] = player;
					y -= playheadHeight;
					addRectToMesh(playheadMesh,{p0.x, y,
						ofMap(player->getPositionMS(), 0, player->getDurationMS(), 0, leftR.width - vuWidth),
						playheadHeight
					},ofColor(0),false);
					if(!bDisableTextRendering)	txt = "Player: " + ofFilePath::getFileName(player->getFilePath());
				}else if(sgnlSrc){
					if(!bDisableTextRendering) txt = sgnlSrc->getName();
				}else{
					txt = "";
				}
				if(!bDisableTextRendering && !txt.empty()){
					textMeshes.push_back(ofVboMesh());
					textMeshes.back() = bf.getMesh(txt, p0.x+4, y);
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
	float mx = leftR.getMaxX() - vuWidth;
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
void ofxSoundMatrixMixerRenderer::drawStatus(float x, float y){
	std::stringstream ss;
	ss << "Num Output Channels : " << numChansOut << std::endl;
	ss << "Num Input Channels  : " << numChansIn << std::endl;
	ss << "Num Input Objects   : " << obj->inObjects.size() << std::endl;
	
	ofDrawBitmapStringHighlight(ss.str(), x, y);
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::draw(const ofRectangle& mixerRect){
	
	if(obj != nullptr){
		

		if(numChansIn != obj->getNumInputChannels() ||
		   numChansOut != obj->getNumOutputChannels() ||
		   drawRect != mixerRect){
			
			
			numChansIn = obj->getNumInputChannels();
			numChansOut = obj->getNumOutputChannels();
			
			drawRect = mixerRect;
			buildMeshes();
		}
		
		mainMesh.draw();
		lineGridMesh.draw();
		slidersGrid.draw(gridR);
		outputSliders.draw(outputSlidersRect);
		if(!bDisableTextRendering){
			ofBitmapFont bf;
			bf.getTexture().bind();
			for(auto& tm: textMeshes){
				tm.draw();
			}
			bf.getTexture().unbind();
		}
		
		updatePlayheads();

		playheadMesh.draw();
		
		
		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
			obj->inObjects[idx]->vuMeter.draw();
		}
		obj->outVuMeter.draw();
		
		
	}
}
