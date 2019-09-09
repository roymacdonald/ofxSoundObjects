#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"
class waveformDraw: public ofRectangle, public ofxSoundObject{
public:
	virtual  std::string getName() override{ return "waveForm";}
	waveformDraw():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
		bRenderWaveforms = false; 
	}
	waveformDraw(const waveformDraw& a): ofRectangle(a), ofxSoundObject(a){}
//--------------------------------------------------------------
    void setup(float x, float y, float w, float h){
        this->set(x, y, w, h);
    }
//--------------------------------------------------------------
    void process(ofSoundBuffer &input, ofSoundBuffer &output) override{
		{
			std::lock_guard<std::mutex> lck(mutex1);
			if(input.size()!=buffer.size()) {
				ofLogVerbose("waveformDraw") << "working buffer size != output buffer size.";
				buffer.resize(input.size());
				buffer.setNumChannels(input.getNumChannels());
				buffer.setSampleRate(input.getSampleRate());
			}
			input.copyTo(this->buffer);
		}
		bRenderWaveforms = true;
        input.copyTo(output);
	}
	
//--------------------------------------------------------------
    void draw(const ofRectangle& viewport = ofRectangle()){
		if(!viewport.isZero() && viewport != (ofRectangle)*this){
			this->set(viewport);
			bRenderWaveforms = true;
		}
		if(bRenderWaveforms){
			bRenderWaveforms = false;
			makeWaveformMesh();
		}
		for (auto& w: waveforms) {
			w.draw();
		}
    }
//--------------------------------------------------------------
	// you can use this method to render and draw a static buffer.
	void makeMeshFromBuffer(const ofSoundBuffer& buffer){
		{
			std::lock_guard<std::mutex> lck(mutex1);
			this->buffer.resize(buffer.size());
			this->buffer.setNumChannels(buffer.getNumChannels());
			this->buffer.setSampleRate(buffer.getSampleRate());
			buffer.copyTo(this->buffer);
			bRenderWaveforms = true;
		}
	}
protected:
	void makeWaveformMesh(){
		if (buffer.size() >0) {
			
			auto chans = buffer.getNumChannels();
			waveforms.resize(chans);
			for (int i = 0; i < chans; i++) {
				waveforms[i].clear();
				waveforms[i].setMode(OF_PRIMITIVE_LINE_STRIP);
			}
			
			float h = this->height / float(chans);
			
			float xInc = this->getWidth()/(float)(buffer.getNumFrames() -1);
			glm::vec3 v;
			v.x = x;
			for(int i=0; i<buffer.getNumFrames(); i++){
				for (int j = 0; j < chans; j++) {
					v.y = ofMap(buffer[i*chans + j], -1, 1, h*(j+1) + y, h*j + y );
					waveforms[j].addVertex(v);
				}
				v.x += xInc;
			}
		}
	}
	vector<ofVboMesh>waveforms;
    ofSoundBuffer buffer;
	std::atomic<bool> bRenderWaveforms;
	
   mutable ofMutex mutex1;
};
