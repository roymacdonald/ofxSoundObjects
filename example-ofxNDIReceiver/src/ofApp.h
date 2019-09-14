#pragma once

#include "ofMain.h"

#include "waveformDraw.h"

#include "VUMeter.h"

#include "ofxNDISoundObject.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    void exit();
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

    ofSoundStream stream;
    ofxSoundOutput output;
	
	VUMeter vuMeter;
	
	waveformDraw wave;
	
	void setViewports();
	
	ofxNDIReceiverSoundObject receiver;
	
	ofRectangle helpTextRect;
	string helpText;
	bool bDrawHelp = true;
	
	
	void setHelpText();
	
};
