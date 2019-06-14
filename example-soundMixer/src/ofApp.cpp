#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
	ofSoundStreamSettings streamSettings;
	streamSettings.numInputChannels = 0;
	streamSettings.numOutputChannels = 2;
	streamSettings.sampleRate = 48000; //make sure you pass the correct sample rate.
	streamSettings.bufferSize = 256;
	streamSettings.numBuffers = 1;
	
    stream.setup(streamSettings);
	
    ofDirectory dir;
    dir.allowExt("mp3");
    dir.allowExt("wav");
    
    dir.listDir("../../../../../examples/sound/soundPlayerExample/bin/data/sounds");
    volumeGroup.setName("PLAYER VOLUME");

	players.resize(dir.size());
	
	// change bLoadAsync to load files asynchronously. This means that files are loaded on a different thread, thus not blocking the thread from where you are loading these. 
	bool bLoadAsync = true;
	
    for(int i = 0; i < dir.size(); i++){
		cout << dir.getPath(i) << endl;
        players[i] = make_unique<ofxSoundPlayerObject>();
		players[i]->setLoop(true);
		if(bLoadAsync){
		//when you use loadAsync you will not be able to call play immediately after calling loadAsync, as it will not allow for such.
		// so if you want to play the file immediately after it has finished loading you can pass a boolean as a second argument to loadAsync. True means play once loaded
			players[i]->loadAsync(ofToDataPath(dir.getPath(i)), true);
		}else{
			players[i]->load(ofToDataPath(dir.getPath(i)));
			players[i]->play();
		}
		players[i]->connectTo(mixer);
		
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
		players[0]->drawDebug(gui.getShape().getMaxX(), gui.getShape().y);
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
