#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
    auto devices = ofSoundStreamListDevices();
    ofSoundStreamSettings settings;
    settings.bufferSize = 256;
    settings.numBuffers = 1;
    settings.numInputChannels = 64;
    settings.numOutputChannels = 0;
    
    // Set the following to the correct sound device.
    settings.setInDevice(devices[3]);//change the number between brackets for the one that has been printed on the console.
  //  settings.setOutDevice(devices[3]);// You can have different devices as input and output if you wish.
    stream.setup(settings);
    stream.setInput(input);
//    stream.setOutput(mixer);


    ofSoundStreamSettings outsettings;
    outsettings.bufferSize = 256;
    outsettings.numBuffers = 1;
    outsettings.numInputChannels = 0;
    outsettings.numOutputChannels = 2;
    
    outsettings.setInDevice(devices[1]);
    outsettings.setOutDevice(devices[1]);

    outstream.setup(outsettings);
    outstream.setOutput(mixer);



    int count = 0;
    waveforms.resize(9);
    float w = ofGetWidth()/3;
    float h = ofGetHeight()/3;
    
    for (int i = 0; i < 9; i++) {
        vector<int> v;
        v.resize(2);
        for(int j =0 ; j < v.size(); j++){
            v[j] = j+count;
        }

        waveforms[i].setup(w * (i%3), floor(i/3.0f)* h, w, h);
        input.getOrCreateChannelGroup(v).connectTo(waveforms[i]).connectTo(mixer);
        count += 2 ;
    }

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    for(auto& w: waveforms){
        
        w.draw();
        ofPushStyle();
        ofNoFill();
        ofSetColor(ofColor::yellow);
        ofDrawRectangle(w);
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
