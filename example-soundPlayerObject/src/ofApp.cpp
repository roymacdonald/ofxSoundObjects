#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
        stream.setup(2, 0, player.getSoundFile().getSampleRate(), 256, 1);
        stream.setOutput(output);
        
		player.play();
		
		wave.setup(0, 0, ofGetWidth(), ofGetHeight());
        
        player.connectTo(wave).connectTo(output);
        

        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        gui.add(player.volume);
        pan.addListener(this, &ofApp::panChanged);
        gui.add(speed.set("Speed", 1, 0, 10));
        speed.addListener(this, &ofApp::speedChanged);
		ofBackground(0);
	}
}
//--------------------------------------------------------------
void ofApp::speedChanged(float&){
    player.setSpeed(speed);
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
    player.drawDebug(0,0);
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
