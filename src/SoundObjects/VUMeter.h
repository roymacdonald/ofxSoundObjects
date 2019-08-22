//
//  ofxSoundObjectVURenderer.h
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/5/19.
//
//

#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"
#include "ofxSoundUtils.h"
#include "ofxSoundRendererUtils.h"

using namespace ofxSoundRendererUtils;
class VUMeter:  public ofxSoundObject{
public:
	VUMeter():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){}
	virtual  std::string getName() override{ return "VUMeter";}
	
	//--------------------------------------------------------------
	VUMeter(const VUMeter& a): ofxSoundObject(a){
		this->drawMode = a.drawMode;
		this->stackMode = a.stackMode;
		setDrawRectangle(a.drawRect);
	}
	//--------------------------------------------------------------
	VUMeter& operator=(const VUMeter& mom) {
		this->processData = mom.processData;
		this->drawData = mom.drawData;
		this->setDrawRectangle(mom.drawRect);
		this->drawMode = mom.drawMode;
		this->stackMode = mom.stackMode;
		return *this;
	}
	
	enum DrawMode{
		VU_DRAW_VERTICAL =0,
		VU_DRAW_HORIZONTAL
	} drawMode = VU_DRAW_VERTICAL;
	enum StackMode{
		VU_STACK_VERTICAL =0,
		VU_STACK_HORIZONTAL
	} stackMode = VU_STACK_HORIZONTAL;
	
	//--------------------------------------------------------------
	void setup(const ofRectangle& r, DrawMode drawMode = VU_DRAW_VERTICAL, StackMode stackMode = VU_STACK_HORIZONTAL){
		this->drawMode = drawMode;
		this->stackMode = stackMode;
		setDrawRectangle(r);
		
	}
	void setup(float x, float y, float w, float h, DrawMode drawMode = VU_DRAW_VERTICAL, StackMode stackMode = VU_STACK_HORIZONTAL){
		setup(ofRectangle(x,y,w, h), drawMode, stackMode);
	}
	//--------------------------------------------------------------
	void setDrawRectangle(const ofRectangle& r){
		if(drawRect != r){
//			std::cout << "VUMeter drawRect " << drawRect << std::endl;
			drawRect.set(r);
			buildMeshes();
		}
	}
	//--------------------------------------------------------------
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override{
		
		calculate(input);
		input.copyTo(output);
	}
	//--------------------------------------------------------------
	void calculate(ofSoundBuffer &input){
		
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
		}
		
		//		mutex.lock();
		std::lock_guard<std::mutex> mtx(mutex);
		drawData = processData;
		//		mutex.unlock();
		bNeedsUpdate = true;
	}
	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	//	void drawChannel(size_t chan, ofRectangle r){
	//		size_t n = drawData.rms.size();
	//		if(chan < n){
	//			float rms_v = ofMap(drawData.rms[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
	//			float peak_v = ofMap(drawData.peak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
	//			float peakHold_v = ofMap(drawData.holdPeak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
	//			if(drawMode == VU_DRAW_VERTICAL){
	//				r.y = r.getMaxY() - rms_v;
	//				r.height = rms_v;
	//			}else{
	//				r.width = rms_v;
	//			}
	//			ofSetColor(getRmsColor());
	//			ofDrawRectangle(r);
	//			drawPeakLine(peak_v, r);
	//			ofSetColor(getPeakColor());
	//			drawPeakLine(peakHold_v, r);
	//		}
	//	}
	//--------------------------------------------------------------
	void draw(){
		
		
		buildMeshes();
		
		updateMeshes();
		
		
		fillMesh.draw();
		lineMesh.draw();
		
//		std::stringstream ss;
//		
//		switch(drawMode){
//			case VU_DRAW_VERTICAL: ss <<  "VU_DRAW_VERTICAL\n";break; 
//			case VU_DRAW_HORIZONTAL: ss <<  "VU_DRAW_HORIZONTAL\n";break; 
//		}
//		
//		switch(stackMode){
//			case VU_STACK_VERTICAL: ss <<  "VU_STACK_VERTICAL";break; 
//			case VU_STACK_HORIZONTAL: ss <<  "VU_STACK_HORIZONTAL";break; 
//		}
//		
//		ofDrawBitmapStringHighlight(ss.str(), drawRect.x+10, drawRect.y+20);
		
	}
	//--------------------------------------------------------------
	void buildMeshes(){
		size_t n = drawData.rms.size();
		if(prevNumChans != n){
//			std::cout << "VUMeter buildMeshes " << std::endl;
			
			prevNumChans = n;
			
			lineMesh.clear();
			fillMesh.clear();
			
			
			lineMesh.setMode(OF_PRIMITIVE_LINES);
			lineMesh.setUsage(GL_STATIC_DRAW);
			
			fillMesh.setMode(OF_PRIMITIVE_TRIANGLES);
			fillMesh.setUsage(GL_STATIC_DRAW);
			
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
				//			ofRectangle r = singleMeterRect;
				for(size_t i =0; i < n; i++){
					
					vuRects[i] = r;
					
					addRectToMesh(fillMesh, r, getRmsColor(), false);
					addLineToMesh(lineMesh, r.getTopLeft(), bDrawVertical ? r.getTopRight() :  r.getBottomLeft() , getPeakColor());
					addLineToMesh(lineMesh, r.getTopLeft(), bDrawVertical ? r.getTopRight() :  r.getBottomLeft() , getPeakHoldColor());
					if(bStackVertical){
						r.y += r.height;
					}else{
						r.x += r.width;
					}
				}
			}
			addRectToMesh(lineMesh, drawRect,  getBorderColor(), true);
			bNeedsUpdate = true;
			
		}
	}
	//--------------------------------------------------------------
	void updateMeshes(){
		if(bNeedsUpdate){
			bNeedsUpdate = false;
			size_t n = drawData.rms.size();
			if(n != vuRects.size()){
				n = std::min(n, vuRects.size());
				ofLogWarning("VUMeter::updateMeshes") << "vuRects size not equal to data size";
			}
			auto& v = fillMesh.getVertices();
			auto& vl = lineMesh.getVertices();
			size_t vi = 0;
			float mn ;
			float mx;
			for(size_t i =0; i < n; i++){
				mn = ((drawMode == VU_DRAW_VERTICAL)?vuRects[i].getMaxY():vuRects[i].getMinX());
				mx = ((drawMode == VU_DRAW_VERTICAL)?vuRects[i].getMinY():vuRects[i].getMaxX());
				
				float rms_v =		ofMap(drawData.rms[i],		0, 1, mn, mx);
				float peak_v =		ofMap(drawData.peak[i],		0, 1, mn, mx);
				float peakHold_v =	ofMap(drawData.holdPeak[i], 0, 1, mn, mx);
				
				vi = i*4;
				size_t d = (drawMode == VU_DRAW_VERTICAL)?1:0;
				
				v[vi+1 -d][d] = rms_v;
				v[vi+2 -d][d] = rms_v;
				
				vl[vi  ][d] = peak_v;
				vl[vi+1][d] = peak_v;
				
				vl[vi+2][d] = peakHold_v;
				vl[vi+3][d] = peakHold_v;
			}
		}
	}
	//--------------------------------------------------------------
	float getRmsForChannel(size_t channel) const{
		if(channel < drawData.rms.size()){
			return drawData.rms[channel];
		}
		return 0;
	}
	//--------------------------------------------------------------
	float getPeakForChannel(size_t channel) const{
		if(channel < drawData.peak.size()){
			return drawData.peak[channel];
		}
		return 0;
	}
	//--------------------------------------------------------------
	// You can use the following static functions as setters or getters.
	// if you want to set the color of the RMS value you can do, for example
	//	VUMeter::getRmsColor() = ofColor::blue;
	// or to set the peak release time
	//	VUMeter::getPeakReleaseTime() = 100;
	//--------------------------------------------------------------
	static ofColor& getRmsColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(150);
		return *i;
	}
	//--------------------------------------------------------------
	static ofColor& getPeakColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::red);
		return *i;
	}
	//--------------------------------------------------------------
	static ofColor& getPeakHoldColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::white);
		return *i;
	}
	//--------------------------------------------------------------
	static ofColor& getBorderColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(70);
		return *i;
	}
	//--------------------------------------------------------------
	static uint64_t& getPeakHoldTime(){
		static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(5000);
		return *i;
	}
	//--------------------------------------------------------------
	static uint64_t& getPeakReleaseTime(){
		static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(1000);
		return *i;
	}
	//--------------------------------------------------------------
	void resetPeak(){
		mutex.lock();
		processData.peak.assign(processData.peak.size(), 0.0f);
		processData.lastPeak.assign(processData.lastPeak.size(), 0.0f);
		mutex.unlock();
	}
	
protected:
	//--------------------------------------------------------------
	//	void drawPeakLine(float p, ofRectangle& r){
	//		if(drawMode == VU_DRAW_VERTICAL){
	//			p = r.getMaxY() - p;
	//			ofDrawLine(r.x, p, r.getMaxX(), p);
	//		}else{
	//			p = r.x + p;
	//			ofDrawLine(p, r.y, p, r.getMaxY());
	//		}
	//	}
	ofRectangle drawRect;
	std::vector <ofRectangle> vuRects;
	
private:
	
	bool bNeedsUpdate = false;
	
	struct VuData{
		std::vector<float>rms;
		std::vector<float>peak;
		std::vector<float>lastPeak;
		std::vector<float>holdPeak;
		
		void resize(const size_t& newSize){
			ofxSoundUtils::resize_vec(rms, newSize);
			ofxSoundUtils::resize_vec(peak, newSize);
			ofxSoundUtils::resize_vec(lastPeak, newSize);
			ofxSoundUtils::resize_vec(holdPeak, newSize);
		}
		
	}processData, drawData;
	
	uint64_t lastPeakTime;
	mutable ofMutex mutex;
	
	
	ofVboMesh lineMesh, fillMesh;
	size_t prevNumChans = 0;
	
	
	
};
