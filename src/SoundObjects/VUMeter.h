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

class VUMeter: public ofRectangle, public ofxSoundObject{
public:
	VUMeter():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){}
	virtual  std::string getName() override{ return "VUMeter";}
//--------------------------------------------------------------
	VUMeter(const VUMeter& a):ofRectangle(a), ofxSoundObject(a){}
//--------------------------------------------------------------
	VUMeter& operator=(const VUMeter& mom) {
		this->processData = mom.processData;
		this->drawData = mom.drawData;
		return *this;
	}
//--------------------------------------------------------------
	void setup(float x, float y, float w, float h){
		this->set(x, y, w, h);
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
		
		mutex.lock();
		drawData = processData;
		mutex.unlock();
		
	}
//--------------------------------------------------------------
	enum DrawMode{
		VU_DRAW_VERTICAL =0,
		VU_DRAW_HORIZONTAL
	} drawMode = VU_DRAW_VERTICAL;
//--------------------------------------------------------------
	void drawChannel(size_t chan, ofRectangle r){		
		size_t n = drawData.rms.size(); 
		if(chan < n){			
			float rms_v = ofMap(drawData.rms[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			float peak_v = ofMap(drawData.peak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			float peakHold_v = ofMap(drawData.holdPeak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			if(drawMode == VU_DRAW_VERTICAL){
				r.y = r.getMaxY() - rms_v;
				r.height = rms_v;
			}else{
				r.width = rms_v;
			}
			ofSetColor(getRmsColor());
			ofDrawRectangle(r);
			drawPeakLine(peak_v, r);
			ofSetColor(getPeakColor());
			drawPeakLine(peakHold_v, r);
		}
	}
//--------------------------------------------------------------
	void draw(){
		size_t n = drawData.rms.size(); 
		if(n){
			for(size_t i =0; i < n; i++){
				ofRectangle r = *this;
				if(drawMode == VU_DRAW_VERTICAL){
					r.width /= n;
					r.x += r.width * i;
				}else{
					r.height /= n;
					r.y += r.height * i;
				}
				drawChannel(i, r);
			}
		}
		
	}
	
//--------------------------------------------------------------
	float getRMSforChannel(size_t channel) const{
		if(channel < drawData.rms.size()){
			return drawData.rms[channel];
		}
		return 0;
	}
//--------------------------------------------------------------
	float getPeakforChannel(size_t channel) const{
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
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(100);
		return *i;
	};
//--------------------------------------------------------------
	static ofColor& getPeakColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::red);
		return *i;
	};
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
	void drawPeakLine(float p, ofRectangle& r){
		if(drawMode == VU_DRAW_VERTICAL){
			p = r.getMaxY() - p; 
			ofDrawLine(r.x, p, r.getMaxX(), p);
		}else{
			p = r.x + p; 
			ofDrawLine(p, r.y, p, r.getMaxY());
		}
	}
	
private:

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
};
