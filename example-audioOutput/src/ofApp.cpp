#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);

	// 2 output channels,
	// 0 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)

    ofxSoundStreamSetup(2, 0, this, 44100, 512, 4);

	ofSetFrameRate(60);
    wave.setup(0,0,ofGetWidth(),ofGetHeight());
    sine.setup(440);
    sine.connectTo(wave).connectTo(ofGetSystemSoundMixer());
}


//--------------------------------------------------------------
void ofApp::update(){

    sine.freq = ofMap(ofGetMouseX(), 0, ofGetWidth(), 100, 15000);
    sine.amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, 1);
}

//--------------------------------------------------------------
void ofApp::draw(){
    wave.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
