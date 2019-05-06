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
	VUMeter(const VUMeter& a):ofRectangle(a), ofxSoundObject(a){}
	
	VUMeter& operator=(const VUMeter& mom) {
		this->rms = mom.rms;
		this->peak = mom.peak;
		this->lastPeak = mom.lastPeak;
		this->holdPeak = mom.holdPeak;
		return *this;
	}
	
	//--------------------------------------------------------------
	void setup(float x, float y, float w, float h){
		this->set(x, y, w, h);
	}
	//--------------------------------------------------------------
	void process(ofSoundBuffer &input, ofSoundBuffer &output) {
		
		calculate(input);
		input.copyTo(output);
	}
	void calculate(ofSoundBuffer &input){
		
		size_t nc = input.getNumChannels();
		
		if(rms.size() != nc) rms.resize(nc);
		if(peak.size() != nc) peak.resize(nc);
		if(lastPeak.size() != nc) lastPeak.resize(nc);
		if(holdPeak.size() != nc) holdPeak.resize(nc);
		
		auto t = ofGetElapsedTimeMillis();
		if(ofxSoundUtils::getBufferPeaks(input, peak, holdPeak)){
			lastPeakTime = t;
			lastPeak = peak;
			holdPeak = peak;
		}else{
			auto releaseStart = lastPeakTime + getPeakHoldTime();
			auto releaseEnd = releaseStart + getPeakReleaseTime();
			if( releaseStart < t && t <= releaseEnd){
				float pct = 1.0 - ofMap(t, releaseStart, releaseEnd,  0, 1, true);
				for(size_t i = 0; i < peak.size(); i++){
					holdPeak[i] = lastPeak[i]*pct;
				}
			}
		}
		
		for(size_t i =0; i < nc; i++){
			rms[i] = input.getRMSAmplitudeChannel(i);
		}
	}
	//--------------------------------------------------------------
	enum DrawMode{
		VU_DRAW_VERTICAL =0,
		VU_DRAW_HORIZONTAL
	} drawMode = VU_DRAW_VERTICAL;
	//--------------------------------------------------------------
	void drawChannel(size_t chan, ofRectangle r){
		mutex.lock();
		size_t n = rms.size(); 
		mutex.unlock();
		if(chan < n){
			mutex.lock();			
			float rms_v = ofMap(rms[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			float peak_v = ofMap(peak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			float peakHold_v = ofMap(holdPeak[chan], 0, 1, 0, ((drawMode == VU_DRAW_VERTICAL)?r.height:r.width));
			mutex.unlock();
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
	void draw(){
		mutex.lock();
		size_t n = rms.size(); 
		mutex.unlock();
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
	static ofColor& getRmsColor(){ 
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(100);
		return *i;
	};
	static ofColor& getPeakColor(){
		static std::unique_ptr<ofColor> i =  make_unique<ofColor>(ofColor::red);
		return *i;
	};
	
	static uint64_t& getPeakHoldTime(){
		static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(5000);
		return *i;
	}
	static uint64_t& getPeakReleaseTime(){
		static std::unique_ptr<uint64_t> i =  make_unique<uint64_t>(1000);
		return *i;
	}
	
	void resetPeak(){
		peak.assign(peak.size(), 0.0f);
		lastPeak.assign(peak.size(), 0.0f);
	}
	
	std::vector<float>rms;
	std::vector<float>peak;
	std::vector<float>lastPeak;
	std::vector<float>holdPeak;
	
private:
	
	void drawPeakLine(float p, ofRectangle& r){
		if(drawMode == VU_DRAW_VERTICAL){
			p = r.getMaxY() - p; 
			ofDrawLine(r.x, p, r.getMaxX(), p);
		}else{
			p = r.x + p; 
			ofDrawLine(p, r.y, p, r.getMaxY());
		}
	}
	
	uint64_t lastPeakTime;
	mutable ofMutex mutex;
};
