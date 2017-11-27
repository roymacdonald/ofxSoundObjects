#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    
    auto devices = ofSoundStreamListDevices();
//
//    for(int i = 0; i < devices.size(); i ++){
//        cout << i << " : " << devices[i] << endl;
//    }
//    
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
        ofSoundStreamSettings settings;
        settings.bufferSize = 256;
        settings.numBuffers = 1;
        settings.numInputChannels = 0;
        settings.numOutputChannels = 2;
        settings.sampleRate = player.getSoundFile().getSampleRate();
//        settings.setInDevice(devices[3]);
//        settings.setOutDevice(devices[3]);
        stream.setup(settings);
        stream.setOutput(output);
        
		player.play();
		
		wave.setup(0, 0, ofGetWidth(), ofGetHeight());
        
        player.connectTo(wave).connectTo(output);
        

        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        gui.add(player.volume);
        pan.addListener(this, &ofApp::panChanged);
        gui.add(speed.set("Speed", 1, 0, 10));
        speed.addListener(this, &ofApp::speedChanged);
		ofBackground(0);
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
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	
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
