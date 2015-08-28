#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    stream.setup(2, 0, 44100, 256, 1);
    
    players.resize(4);
    ofDirectory dir;
    dir.allowExt("mp3");
    dir.allowExt("wav");
    
    dir.listDir("sounds");
    volumeGroup.setName("PLAYER VOLUME");
    
    for(int i = 0; i < dir.size(); i++){
        players.push_back(ofxBasicSoundPlayer());
        players.back().load(dir.getPath(i));
        players.back().setLoop(true);
        players.back().connectTo(mixer);
        players.back().play();
        playersVolume.push_back(1);
        volumeGroup.add(playersVolume.back().set("Player " + ofToString(i), 1, 0, 1));
    }
    
    stream.setOutput(mixer);
    
    gui.setup(volumeGroup);
    
    ofBackground(0);
    
    ofAddListener(volumeGroup.parameterChangedE(), this, &ofApp::updateVolume);
}
//--------------------------------------------------------------
void ofApp::updateVolume(ofAbstractParameter& p){
    for (int i = 0; i < playersVolume.size(); i++) {
        mixer.setChannelVolume(i, playersVolume[i]);
    }
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
    
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
