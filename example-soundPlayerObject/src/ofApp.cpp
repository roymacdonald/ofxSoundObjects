#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
        ofSoundStreamSettings settings;
        settings.bufferSize = 256;
        settings.numBuffers = 1;
        settings.numInputChannels = 0;
        settings.numOutputChannels = 2;
        settings.sampleRate = player.getSoundFile().getSampleRate();
        stream.setup(settings);
        stream.setOutput(output);
        
        
		player.play();
		
		wave.setup(0, 0, ofGetWidth(), ofGetHeight());
        
        player.connectTo(wave).connectTo(output);
        

        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        pan.addListener(this, &ofApp::panChanged);
		ofBackground(0);
	}
}
//--------------------------------------------------------------
void ofApp::panChanged(float&f){

    player.setPan(pan);
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	wave.draw();
    
    gui.draw();
	
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
