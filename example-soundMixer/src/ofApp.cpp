#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    stream.setup(2, 0, 44100, 256, 1);
    
 //   players.resize(4);
    ofDirectory dir;
    dir.allowExt("mp3");
    dir.allowExt("wav");
    
    dir.listDir("sounds");
    volumeGroup.setName("PLAYER VOLUME");

	players.resize(dir.size());
    for(int i = 0; i < dir.size(); i++){
        players[i].load(dir.getPath(i));
        players[i].setLoop(true);
        players[i].connectTo(mixer);
        players[i].play();
        playersVolume.push_back(1);
        volumeGroup.add(playersVolume.back().set("Player " + ofToString(i), 1, 0, 1));
    }
    
    stream.setOutput(mixer);
    
    gui.setup(volumeGroup);
    
    ofBackground(0);
    
    ofAddListener(volumeGroup.parameterChangedE(), this, &ofApp::updateVolume);
}
//--------------------------------------------------------------
void ofApp::exit() {
	cout << "ofApp::exit" << endl;
	players.clear();
	stream.close();
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
void ofApp::keyPressed(int key){
	if (key >= '1' && key <= '9') {
		int i = key - '1';
		if (i < players.size()) {
			stream.setOutput(players[i]);
		}
	}
}
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
