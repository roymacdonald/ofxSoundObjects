//
//  waveformDraw.h
//  example-audio-sender
//
//  Created by Roy Macdonald on 9/17/19.
//
//

#pragma once

#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"
#include "ofxSoundUtils.h"
#include "ofx2DCanvas.h"

template<typename BufferType>
class waveformDraw_: public ofRectangle, public ofxSoundObject{
public:
	
	waveformDraw_();
	waveformDraw_(const waveformDraw_& a): ofRectangle(a), ofxSoundObject(a){}

	void setup(const ofRectangle& r);
    void setup(float x, float y, float w, float h);
    virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	void draw(const ofRectangle& viewport = ofRectangle());
	
	// you can use this method to render and draw a static buffer.
    // bRenderToFbo if true it will render into an fbo. use this when rendering static buffers.
	void makeMeshFromBuffer(const ofSoundBuffer& buffer, bool bRenderToFbo = false);
	
	void setGridSpacingByNumSamples(size_t spacing);
	
	void setWaveColor(const ofColor& color);
//	void setBackgroundColor(const ofColor& color);
    void setGridColor(const ofColor& color);
	void setMarginColor(const ofColor& color);

	const ofColor&  getWaveColor();

	const ofColor&  getMarginColor();
    
    ofx2DCanvas& getCanvas(){return canvas;}
    
    

    void enableFbo();
    void disableFbo();
    bool isFboEnabled() {return bUseFbo;}
    
    
    ///\brief Setups up the cameras to start drawing.
    ///Call it for drawing an overlay over the waveform.
    ///Make sure that you have called draw() before calling begin(), and you must call end() once done drawing.
    ///While between the begin() and end() calls the coordinate space origin is at the top left corner of the viewport.
    ///No need to worry about zooming and panning to match the waveform. This is the purpose of this function.
    void begin();
    void end();
    
protected:
    void drawWave();
    void initFbo();
    void updateFbo();
    bool bUseFbo = false;
    bool bUpdateFbo = false;
    bool bIsCanvasTransforming = false;

    ofRectangle onTransformRect;
    
    
    ofEventListeners listeners;
    
    ofFbo fbo;
    
	virtual void updateWaveformMesh();
	
	void makeWaveformMesh();
 
	void makeGrid();
	
	ofx2DCanvas canvas;
	
	vector<ofVboMesh>waveforms;
	
	ofVboMesh gridMesh;
	
	size_t gridSpacing = 0;

	
	bool bCanvasIsSetup = false;
	
	
	std::atomic<bool> bRenderWaveforms;
	std::atomic<bool> bMakeGrid;
	mutable ofMutex mutex1;
	
	BufferType buffer;

	ofColor  waveColor;
    ofFloatColor  gridColor = {(80.0f/255.0f)};
	ofColor  marginColor;

};

typedef waveformDraw_<ofSoundBuffer> waveformDraw;



class circularBufferWaveformDraw : public waveformDraw_<ofxCircularSoundBuffer>{
public:
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
    void setNumBuffers(size_t numBuffers);

protected:
	virtual void updateWaveformMesh() override;

};



