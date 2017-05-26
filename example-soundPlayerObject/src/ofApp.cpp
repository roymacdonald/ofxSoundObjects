#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
		if (player.load(result.getPath())) {
			ofSoundStreamSettings settings;
			settings.bufferSize = 256;
			settings.numBuffers = 1;
			settings.numInputChannels = 0;
			settings.numOutputChannels = 2;
			settings.sampleRate = player.getSoundFile().getSampleRate();
			cout << "SampleRate: " << settings.sampleRate << endl;
			wave.setup(0, 0, ofGetWidth(), ofGetHeight());
			player.connectTo(wave);

			//player.getSoundFile().readTo(wave.buffer, player.getSoundFile().getNumSamples());
			stream.setup(settings);
			//stream.setOutput(player);
			//stream.setOutput(output);
			stream.setOutput(wave);


			//player.connectTo(wave).connectTo(output);
			//player.connectTo(output);
			player.play();

			gui.setup();
			gui.add(pan.set("PAN", 0, -1, 1));
			pan.addListener(this, &ofApp::panChanged);
			ofBackground(0);
		}
	}
}
//--------------------------------------------------------------
void ofApp::panChanged(float&f){
	if (player.isLoaded()) {
		player.setPan(pan);
	}
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::exit() {
	if (player.isLoaded()) {
		stream.close();
	}

}
//--------------------------------------------------------------
void ofApp::draw(){
	if (player.isLoaded()) {
		wave.draw();

		gui.draw();
	}
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
