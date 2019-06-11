#pragma once

#include "ofMain.h"
#include "ofxSoundObjects.h"
#include "waveformDraw.h"

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
	
	ofSoundStream stream;
	
	ofxSoundInputMultiplexer input;
	ofxSoundOutputMultiplexer output;
	
	
	vector< unique_ptr<ofxSoundRecorderObject>> recorders;
	vector< waveformDraw > waves; 
	
	void toggleChannelRecording(int chan);
	
};
