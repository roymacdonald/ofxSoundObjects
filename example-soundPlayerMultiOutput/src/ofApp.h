#pragma once

#include "ofMain.h"
#include "ofxSoundObjects.h"

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
        ofSoundStream stream;
	
        ofxSoundOutputMultiplexer output;

	//when puting ofxSoundPlayerObjects into a vector you need to do so using shared_ptr or unique_ptr
	vector<shared_ptr<ofxSoundPlayerObject>> players;

};
