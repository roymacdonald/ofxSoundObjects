#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    stream.setup(2, 0, 48000, 256, 1);//make sure you pass the correct sample rate.
    
 //   players.resize(4);
    ofDirectory dir;
    dir.allowExt("mp3");
    dir.allowExt("wav");
    
    dir.listDir("sounds");
    volumeGroup.setName("PLAYER VOLUME");

	players.resize(dir.size());
    for(int i = 0; i < dir.size(); i++){
		cout << dir.getPath(i) << endl;
        players[i].load(ofToDataPath(dir.getPath(i)));
        players[i].connectTo(mixer);
        players[i].play();
		players[i].setLoop(true);// this has to go after calling play.
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
	if(players.size()){
		players[0].drawDebug(gui.getShape().getMaxX(), gui.getShape().y);
	}
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
