#pragma once

#include "ofMain.h"
#include "ofxSoundPlayerObject.h"

#include "waveformDraw.h"
#include "SineWaveGenerator.h"

#include "ofxNDISoundObject.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
	
	// the sender object
	ofxNDISenderSoundObject sender;
	
	// even when you might want to send audio through NDI and not output to your speakers you need a soundstream as it will be used to clock the NDI stream. The sender needs to be connected to a signal chain which has an output stream at its end. Without it it will not work.
	ofSoundStream stream;
    ofxSoundOutput output;
	
	/// these are here just to have different possible sources of audio to send via NDI.
    ofxSoundInput input;
    ofxSoundPlayerObject player;
	SineWaveGenerator sinGen;

	void loadDialogPlayer();
	
	
	// these are for drawing waves and sound levels
	waveformDraw wave;
	VUMeter vuMeter;
	
	// Utility stuff to switch inputs
	enum InputIndex {
		INPUT_SINE_WAVE = 0,
		INPUT_PLAYER,
		INPUT_LIVE_MIC,
		NUM_INPUTS
	}inputIndex = NUM_INPUTS;
	
	void setInput(InputIndex newInput);
	
	// function that arranges the different elements to be drawn.
	void setViewports();

	// stuff for the help text
	ofRectangle helpTextRect;
	string helpText;
	bool bDrawHelp = true;
	void setHelpText();
	
};
