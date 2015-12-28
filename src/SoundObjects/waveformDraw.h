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
			for (int i = 0; i < chans; i++) {
				meshes.push_back(ofMesh());
				meshes.back().setMode(OF_PRIMITIVE_LINE_STRIP);
			}
            
			float h = this->height / float(chans);
			//float h2 = h * 0.5f;
//            float factor= this->width / buffer.getNumFrames();
			
            for(int i=0; i<buffer.getNumFrames(); i++){
                ofVec3f v;
                v.x = ofMap(i, 0, buffer.getNumFrames() -1 , x, getMaxX());
                for (int j = 0; j < chans; j++) {
                    v.y = ofMap(buffer[i*chans + j], -1, 1, h*(j+1), h*j );
                    meshes[j].addColor(ofFloatColor::pink);
                    meshes[j].addVertex(v);
                }
			}
			for (int i = 0; i < chans; i++) {
				meshes[i].draw();
			}
        }
    }
    ofSoundBuffer buffer;
    ofMutex mutex1;
};