#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    int bufferSize = 512;
    stream.setup(2, 2, 44100, bufferSize, 4);
    stream.setOutput(output);
    stream.setInput(input);
    
    fft.setup(bufferSize);
    
    ofRectangle r(0, 0, ofGetWidth(), ofGetHeight()/2);
    r.scaleFromCenter(0.95);
    wave.set(r);

    sinGen.setup(440);
    
    ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac");
    if (result.bSuccess) {
        player.load(result.getPath());
        player.play();
    }
    player.connectTo(wave).connectTo(fft).connectTo(output);

    inputIndex = 0;
    ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    sinGen.freq = ofMap(ofGetMouseX(), 0, ofGetWidth(), 20, 20000);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    fft.draw(ofRectangle(0,ofGetHeight()/2,ofGetWidth(), ofGetHeight()/2));
    wave.draw();
    string msg;
    switch (inputIndex) {
        case 0:
            msg = "INPUT: Audio File Player";
            break;
        case 1:
            msg = "INPUT: Sine Wave Generator";
            break;
        case 2:
            msg  = "INPUT: Live input (mic)";
            break;
        default:
            break;
    }
    msg += "\nPress the space bar to change input.";
    ofDrawBitmapStringHighlight(msg, 20, 20);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        inputIndex  = (inputIndex +1)%3;
        switch (inputIndex) {
            case 0:
                player.connectTo(wave);
                break;
            case 1:
                sinGen.connectTo(wave);
                break;
            case 2:
                input.connectTo(wave);
                break;

            default:
                break;
        }
    }
    
}

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
