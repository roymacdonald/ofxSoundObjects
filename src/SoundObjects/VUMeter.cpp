//
//  VUMeter.cpp
//
//  Created by Roy Macdonald on 9/12/19.
//
//

#include "VUMeter.h"


VUMeter::VUMeter():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR), objectName("VUMeter"){}
//--------------------------------------------------------------
VUMeter::VUMeter(const VUMeter& a): ofxSoundObject(a){
	this->drawMode = a.drawMode;
	this->stackMode = a.stackMode;
	setDrawRectangle(a.drawRect);
}
//--------------------------------------------------------------
VUMeter& VUMeter::operator=(const VUMeter& mom) {
	this->processData = mom.processData;
	this->drawData = mom.drawData;
	this->setDrawRectangle(mom.drawRect);
	this->drawMode = mom.drawMode;
	this->stackMode = mom.stackMode;
	return *this;
}
//--------------------------------------------------------------
void VUMeter::setup(const ofRectangle& r, DrawMode drawMode, StackMode stackMode){
	this->drawMode = drawMode;
	this->stackMode = stackMode;
	setDrawRectangle(r);
	bNeedsBuildMeshes = true;
}
//--------------------------------------------------------------
void VUMeter::setup(float x, float y, float w, float h, DrawMode drawMode, StackMode stackMode){
	setup(ofRectangle(x,y,w, h), drawMode, stackMode);
}
//--------------------------------------------------------------
void VUMeter::setDrawRectangle(const ofRectangle& r){
	if(drawRect != r){
		drawRect.set(r);
		bNeedsBuildMeshes = true;
		buildMeshes();
	}
}
//--------------------------------------------------------------
void VUMeter::resetPeak(){
	std::lock_guard<std::mutex> lck(mutex);
	processData.peak.assign(processData.peak.size(), 0.0f);
	processData.lastPeak.assign(processData.lastPeak.size(), 0.0f);
}
//--------------------------------------------------------------
void VUMeter::process(ofSoundBuffer &input, ofSoundBuffer &output){
	calculate(input);
	output = input;
}
//--------------------------------------------------------------
void VUMeter::calculate(ofSoundBuffer &input){
	
	size_t nc = input.getNumChannels();
	
	processData.resize(nc);
	
	
	auto t = ofGetElapsedTimeMillis();
	
	if(ofxSoundUtils::getBufferPeaks(input, processData.peak, processData.holdPeak)){
		lastPeakTime = t;
		processData.lastPeak = processData.peak;
		processData.holdPeak = processData.peak;
	}else{
		auto releaseStart = lastPeakTime + getPeakHoldTime();
		auto releaseEnd = releaseStart + getPeakReleaseTime();
		if( releaseStart < t && t <= releaseEnd){
			float pct = 1.0 - ofMap(t, releaseStart, releaseEnd,  0, 1, true);
			for(size_t i = 0; i < processData.peak.size(); i++){
				processData.holdPeak[i] = processData.lastPeak[i]*pct;
			}
		}
	}
	
	for(size_t i =0; i < nc; i++){
		processData.rms[i] = input.getRMSAmplitudeChannel(i);
		processData.bClippingPeak[i] = processData.peak[i] >= 1.0;
		processData.bClippingRms[i] =  processData.rms[i] >= 1.0;
	}
	
	std::lock_guard<std::mutex> mtx(mutex);
	drawData = processData;
	bNeedsUpdate = true;
}
//--------------------------------------------------------------
void VUMeter::draw(){
	
	buildMeshes();
	
	updateMeshes();
	borderMesh.draw();
	fillMesh.draw();
	lineMesh.draw();
	
}
//--------------------------------------------------------------
void VUMeter::buildMeshes(){
	size_t n = drawData.rms.size();
	if(prevNumChans != n || getForceRebuild() || bNeedsBuildMeshes ){
		
		bNeedsBuildMeshes = false;
		prevNumChans = n;
		
		lineMesh.clear();
		fillMesh.clear();
		borderMesh.clear();
		
		lineMesh.setMode(OF_PRIMITIVE_LINES);
		lineMesh.setUsage(GL_DYNAMIC_DRAW);
		
		fillMesh.setMode(OF_PRIMITIVE_TRIANGLES);
		fillMesh.setUsage(GL_DYNAMIC_DRAW);
		
		borderMesh.setMode(OF_PRIMITIVE_LINES);
		borderMesh.setUsage(GL_DYNAMIC_DRAW);
		
		
		
		
		vuRects.resize(n);
		
		bool bDrawVertical = drawMode == VU_DRAW_VERTICAL;
		bool bStackVertical = stackMode == VU_STACK_VERTICAL;
		if(n > 0){
			ofRectangle r = drawRect;
			
			if(bStackVertical){
				r.height /= n;
			}else{
				r.width /= n;
			}
			
			for(size_t i =0; i < n; i++){
				
				vuRects[i] = r;
				
				addRectToMesh(fillMesh, r, getRmsColor(), false);
				addRectToMesh(borderMesh, r, getBorderColor(),true);
				addLineToMesh(lineMesh, r.getTopLeft(), bDrawVertical ? r.getTopRight() :  r.getBottomLeft() , getPeakColor());
				addLineToMesh(lineMesh, r.getTopLeft(), bDrawVertical ? r.getTopRight() :  r.getBottomLeft() , getPeakHoldColor());
				if(bStackVertical){
					r.y += r.height;
				}else{
					r.x += r.width;
				}
			}
		}
		bNeedsUpdate = true;
	}
}
//--------------------------------------------------------------
void VUMeter::updateMeshes(){
	if(bNeedsUpdate){
		bNeedsUpdate = false;
		size_t n = drawData.rms.size();
		if(n != vuRects.size()){
			n = std::min(n, vuRects.size());
			ofLogWarning("VUMeter::updateMeshes") << "vuRects size not equal to data size";
		}
		auto& v = fillMesh.getVertices();
		auto& vl = lineMesh.getVertices();
		auto& c  = fillMesh.getColors();
		auto& cl  = lineMesh.getColors();
		size_t vi = 0;
		float mn ;
		float mx;
		
		ofFloatColor fillPeakColor, linePeakColor;
		
		for(size_t i =0; i < n; i++){
			mn = ((drawMode == VU_DRAW_VERTICAL)?vuRects[i].getMaxY():vuRects[i].getMinX());
			mx = ((drawMode == VU_DRAW_VERTICAL)?vuRects[i].getMinY():vuRects[i].getMaxX());
			
			float rms_v =		ofMap(drawData.rms[i],		0, 1, mn, mx, true);
			float peak_v =		ofMap(drawData.peak[i],		0, 1, mn, mx, true);
			float peakHold_v =	ofMap(drawData.holdPeak[i], 0, 1, mn, mx, true);
			
			vi = i*4;
			size_t d = (drawMode == VU_DRAW_VERTICAL)?1:0;
			
			v[vi+1 -d][d] = rms_v;
			v[vi+2 -d][d] = rms_v;
			
			vl[vi  ][d] = peak_v;
			vl[vi+1][d] = peak_v;
			
			vl[vi+2][d] = peakHold_v;
			vl[vi+3][d] = peakHold_v;
			
			
			fillPeakColor = (drawData.bClippingRms[i]?getClippingColor():getRmsColor());
			linePeakColor = (drawData.bClippingPeak[i]?getClippingColor():getPeakHoldColor());
			
			for(int j = 0; j < 4; j++){
				c[vi+j] = fillPeakColor;
			}
			
			cl[vi+2] = linePeakColor;
			cl[vi+3] = linePeakColor;
			
		}
	}
}
//--------------------------------------------------------------
VUMeter::DrawMode VUMeter::getDrawMode(){return drawMode;}
//--------------------------------------------------------------
VUMeter::StackMode VUMeter::getStackMode(){return stackMode;}
//--------------------------------------------------------------
void VUMeter::setDrawMode(VUMeter::DrawMode newMode){
	if(drawMode != newMode){
		drawMode = newMode;
		bNeedsBuildMeshes = true;
	}
}
//--------------------------------------------------------------
void VUMeter::setStackMode(VUMeter::StackMode newMode){
	if(stackMode != newMode){
		stackMode = newMode;
		bNeedsBuildMeshes = true;
	}
}
//--------------------------------------------------------------
float VUMeter::getRmsForChannel(size_t channel) const{
	if(channel < drawData.rms.size()){
		return drawData.rms[channel];
	}
	return 0;
}
//--------------------------------------------------------------
float VUMeter::getPeakForChannel(size_t channel) const{
	if(channel < drawData.peak.size()){
		return drawData.peak[channel];
	}
	return 0;
}
//--------------------------------------------------------------
ofColor& VUMeter::getRmsColor(){
	static std::unique_ptr<ofColor> i =  make_unique<ofColor>(150);
	return *i;
}
//--------------------------------------------------------------
ofColor& VUMeter::getPeakColor(){
	static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::yellow);
	return *i;
}
//--------------------------------------------------------------
ofColor& VUMeter::getPeakHoldColor(){
	static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::white);
	return *i;
}
//--------------------------------------------------------------
ofColor& VUMeter::getBorderColor(){
	static std::unique_ptr<ofColor> i =  make_unique<ofColor>(70);
	return *i;
}
//--------------------------------------------------------------
ofColor& VUMeter::getClippingColor(){
	static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::red);
	return *i;
}
//--------------------------------------------------------------
uint64_t& VUMeter::getPeakHoldTime(){
	static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(5000);
	return *i;
}
//--------------------------------------------------------------
uint64_t& VUMeter::getPeakReleaseTime(){
	static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(1000);
	return *i;
}
//--------------------------------------------------------------
bool& VUMeter::getForceRebuild(){
	static std::unique_ptr<bool> i =  make_unique<bool>(false);
	return *i;
}
