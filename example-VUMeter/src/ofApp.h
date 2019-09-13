#pragma once

#include "ofMain.h"

#include "waveformDraw.h"

#include "ofxSoundPlayerObject.h"
#include "ofxGui.h"
#include "VUMeter.h"

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
	
	ofxSoundPlayerObject player;

	VUMeter vuMeter;
	
	bool bDrawVertical = true;
	bool bStackVertical = false;
	
	
	
	waveformDraw fullFileWaveform;
	
	void setVuMeterAndFileWaveformViews();
	
	
	
	
	
	// gui stuff.
	ofEventListeners listeners;
	ofxPanel gui;
	bool bDrawGui = true;
	bool bDrawHelp = true;
	
	// these ofParameters are for controlling the static (global) parameters of the VUMeter. Changing these will affect all instances of VUMeter
	
	ofParameter<ofColor> rmsColor  = {"Rms Color", VUMeter::getRmsColor(), ofColor(0,0,0,0), ofColor(255,255,255,255)};
	ofParameter<ofColor> peakColor  = {"Peak Color", VUMeter::getPeakColor(), ofColor(0,0,0,0), ofColor(255,255,255,255)};
	ofParameter<ofColor> peakHoldColor  = {"Peak Hold Color", VUMeter::getPeakHoldColor(), ofColor(0,0,0,0), ofColor(255,255,255,255)};
	ofParameter<ofColor> borderColor  = {"Border Color", VUMeter::getBorderColor(), ofColor(0,0,0,0), ofColor(255,255,255,255)};
	ofParameter<uint64_t> peakHoldTime  = {"Peak Hold Time", VUMeter::getPeakHoldTime(), 0, 10000};
	ofParameter<uint64_t> peakReleaseTime  = {"Peak Release Time", VUMeter::getPeakReleaseTime(), 0, 10000};

	
	
	
	ofParameter<float> vuMeterWH = {"VU Meter width or height", 40, 1, 300};
	
	
	ofParameter<void> resetPeak = {"Reset Peak"};


	
	void drawPlayhead();
	bool bPlayerWasPlaying;
	bool setPlayerPositionToMouse(bool bPause);

	
	ofRectangle helpTextRect;
	string helpText;
	
	void setHelpText();
	
};
