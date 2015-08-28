#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
    stream.setup(2, 2, 44100, 256, 1);

    wave.setup(0, 0, ofGetWidth(), ofGetHeight());
    
    stream.setInput(input);
    stream.setOutput(output);
    
    input.connectTo(wave).connectTo(output);
    
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    wave.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ }

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
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

