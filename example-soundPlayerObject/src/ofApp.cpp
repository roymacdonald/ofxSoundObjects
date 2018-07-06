#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
		
		ofSoundStreamSettings soundSettings;
		soundSettings.numInputChannels = 0;
		soundSettings.numOutputChannels = 2;
		soundSettings.sampleRate = player.getSoundFile().getSampleRate();
		soundSettings.bufferSize = 256;
		soundSettings.numBuffers = 1;
		
        stream.setup(soundSettings);
		
		wave.setup(0, 0, ofGetWidth(), ofGetHeight());
        
        player.connectTo(wave).connectTo(output);
        
		stream.setOutput(output);

        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        gui.add(player.volume);
        pan.addListener(this, &ofApp::panChanged);
        gui.add(speed.set("Speed", 1, 0, 10));
        speed.addListener(this, &ofApp::speedChanged);
		ofBackground(0);
		waveforms.resize(player.getSoundFile().getNumChannels());
		float h = ofGetHeight()/waveforms.size();
		
		ofSoundBuffer buff;
		player.getSoundFile().readTo(buff);
		
		for(int i = 0; i < waveforms.size(); i++){
			ofSoundBuffer b;
			buff.getChannel(b, i);
			waveforms[i].setMode(OF_PRIMITIVE_LINE_STRIP);
			for(int j = 0; j < b.getBuffer().size(); j++ ){
				waveforms[i].addVertex({ofMap(j, 0, b.getBuffer().size(), 0, ofGetWidth()), ofMap(b.getBuffer()[j], -1, 1, i*h, (i+1)*h),0.0});
			}
		}
		
		player.play();
		player.setLoop(true);
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
void ofApp::exit(){
    stream.close();
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	ofSetColor(ofColor::white);
	for(auto&w:waveforms){
		w.draw();
	}
	ofSetColor(ofColor::red);
	float playhead = ofMap(player.getPosition(), 0,1,0,ofGetWidth()); 
	ofDrawLine(playhead, 0, playhead, ofGetHeight());
	ofSetColor(ofColor::cyan);
	ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), ofGetHeight());
	
	ofSetColor(ofColor::white);
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
void ofApp::mouseDragged(int x, int y, int button){
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//player.setPosition(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1));
	player.setPositionMS(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, player.getDurationMS()));
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
