#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxSoundUtils::printInputSoundDevices();
	
	
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	// IMPORTANT!!!
	// The following line of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name 
	
	size_t inDeviceIndex = 0;
	size_t outDeviceIndex = 0;
	
	// Setup the sound stream.
	ofSoundStreamSettings settings;
	settings.bufferSize = 256;
	settings.numBuffers = 1;
	settings.numInputChannels =  inDevices[inDeviceIndex].inputChannels;
	settings.numOutputChannels = outDevices[outDeviceIndex].outputChannels;
	settings.sampleRate = 44100;
	settings.setInDevice(inDevices[inDeviceIndex]);
	settings.setOutDevice(outDevices[outDeviceIndex]);
	
	stream.setup(settings);
	
	input.setInputStream(stream);
	
	output.setOutputStream(stream);	
	
	
	wave.setup(0, 0, ofGetWidth(), ofGetHeight());
	
	
	input.connectTo(wave).connectTo(recorder).connectTo(output);

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	wave.draw();
	
	stringstream ss;
	
	ss << "Press the space bar to " << (recorder.isRecording()?"stop":"start") << " recording" <<endl;
	if(recorder.isRecording()){
		ss << "Recording to file: " << recorder.getFileName() << endl;
	}
	
	
	ofSetColor(255);
	ofDrawBitmapString(ss.str(), 20, 20);
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == ' '){
		if(recorder.isRecording()){
			recorder.stopRecording();
		}else{
			recorder.startRecording(ofToDataPath(ofGetTimestampString()+".wav", true));
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
