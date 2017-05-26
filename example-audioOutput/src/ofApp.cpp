#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);

	ofBackground(34, 34, 34);

	int samplerate = 48000;
    stream.setup( 2, 0, samplerate, 256, 1);

	ofSetFrameRate(60);
    wave.setup(0,0,ofGetWidth(),ofGetHeight());
    sine.setup(440,1,0,samplerate);
    sine.connectTo(wave);
    stream.setOutput(wave);
}

//--------------------------------------------------------------
void ofApp::exit() {
	stream.close();
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
