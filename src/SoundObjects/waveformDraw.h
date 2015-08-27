#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"
class waveformDraw: public ofRectangle, public ofxSoundObject{
public:
    waveformDraw(){}
    ~waveformDraw(){}
//--------------------------------------------------------------
    void setup(float x, float y, float w, float h){
        this->set(x, y, w, h);
    }
//--------------------------------------------------------------
    void process(ofSoundBuffer &input, ofSoundBuffer &output) {
       	if(input.size()!=buffer.size()) {
            ofLogVerbose("ofSoundObject") << "working buffer size != output buffer size.";
            buffer.resize(input.size());
            buffer.setNumChannels(input.getNumChannels());
            buffer.setSampleRate(input.getSampleRate());
		}
		
		mutex1.lock();
		input.copyTo(buffer);
        mutex1.unlock();
        input.copyTo(output);
	}
//--------------------------------------------------------------
    void draw(){
        if (buffer.size() >0) {
            vector<ofMesh>meshes;
			int chans = buffer.getNumChannels();
			for (int i = 0; i < chans && i < buffer.size(); i++) {
				meshes.push_back(ofMesh());
				meshes.back().setMode(OF_PRIMITIVE_LINE_STRIP);
			}
			float h = this->height / float(chans);
			float h2 = h * 0.5f;
            float factor= chans * float(this->width) / buffer.size();
			
            for(int i=0; i<buffer.size(); i++){
				int s  = i%chans;
				ofVec3f v(factor*(i-(s)) + x, (buffer[i]+1)*h2 + (h * (s)) + y, 0);
				meshes[s].addColor(ofFloatColor::pink);
				meshes[s].addVertex(v);
			}
			for (int i = 0; i < chans; i++) {
				meshes[i].draw();
			}
        }
    }
    ofSoundBuffer buffer;
    ofMutex mutex1;
};