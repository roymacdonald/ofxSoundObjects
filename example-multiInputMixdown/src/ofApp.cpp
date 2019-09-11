#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
	// IMPORTANT!!!
	// The following two lines of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name
	// You can use a different input and output device.
	
	size_t inDeviceIndex = 0;
	size_t outDeviceIndex = 0;
	
	
	ofxSoundUtils::printInputSoundDevices();
	ofxSoundUtils::printOutputSoundDevices();
	
	
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	
	
	
	

	
    ofSoundStreamSettings settings;
    settings.bufferSize = 256;
    settings.numBuffers = 1;
	settings.sampleRate = 44100;
	settings.numInputChannels = inDevices[inDeviceIndex].inputChannels;
    settings.numOutputChannels = 0;
    
	
	settings.setInDevice(inDevices[inDeviceIndex]);
	
	
    stream.setup(settings);
    stream.setInput(input);


    ofSoundStreamSettings outsettings;
    outsettings.bufferSize = 256;
    outsettings.numBuffers = 1;
	outsettings.sampleRate = settings.sampleRate;// set the samplerate of both devices to be the same
    outsettings.numInputChannels = 0;
    outsettings.numOutputChannels = outDevices[outDeviceIndex].outputChannels ;
    
    outsettings.setOutDevice(outDevices[outDeviceIndex]);
	
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
