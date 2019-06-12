#pragma once

#include "ofMain.h"
#include "ofxSoundObjects.h"
#include "ofxSoundObjectMatrixMixerRenderer.h"

#define USE_WAVES
#ifdef USE_WAVES
#include "waveformDraw.h"
#endif
class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	
	ofxSoundMatrixMixer mixer; 
	ofxSoundMatrixMixerRenderer mixerRenderer;
	

	ofSoundStream stream;
	
	ofxSoundInputMultiplexer input;
	ofxSoundOutputMultiplexer output;
//	ofxSoundMixer mixer;
	
	
	vector< unique_ptr<ofxSoundRecorderObject>> recorders;
 
	
	bool toggleChannelRecording(int chan);
#ifdef USE_WAVES
	vector< waveformDraw > waves;
#endif
};

