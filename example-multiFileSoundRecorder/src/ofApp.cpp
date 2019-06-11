#include "ofApp.h"

//#define USE_LOAD_DIALOG // comment this line if you dont want to use the system load dialog

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxSoundUtils::printInputSoundDevices();
	ofxSoundUtils::printOutputSoundDevices();
	
	
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	// IMPORTANT!!!
	// The following two lines of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name 
	// You can use a different input and output device.
	
	size_t inDeviceIndex = 1;
	size_t outDeviceIndex = 1;
	
	
	cout << "========================" << endl;
	cout << ofxSoundUtils::getSoundDeviceString(inDevices[inDeviceIndex], true, true) << endl;
	
	cout << "========================" << endl;
	
	// Setup the sound stream.
	ofSoundStreamSettings settings;
	settings.bufferSize = 256;
	settings.numBuffers = 1;
	settings.numInputChannels =  inDevices[inDeviceIndex].inputChannels;
	settings.numOutputChannels = outDevices[outDeviceIndex].outputChannels;
	
	auto  sr = inDevices[inDeviceIndex].sampleRates;
	if(sr.size()){
		settings.sampleRate  =sr[0];
	}
	
	settings.setInDevice(inDevices[inDeviceIndex]);
	settings.setOutDevice(outDevices[outDeviceIndex]);
	
	
	stream.setup(settings);
	
	input.setInputStream(stream);

	output.setOutputStream(stream);
	
	// we resize our recorders vector so there is one recorder per audio channel.
	recorders.resize(inDevices[inDeviceIndex].inputChannels);
	waves.resize(recorders.size());
	
	
	float h = (ofGetHeight() - 40)/(float)waves.size();
	float y = 20;
	float w = ofGetWidth() - 40;

	for(size_t i = 0; i < recorders.size(); i++){
		recorders[i] = make_unique<ofxSoundRecorderObject>();
		waves[i].setup(20, y, w, h);
		y += h;
	// using the connectChannelTo method of the inputMultiplezer we can tell that each audio channel gets connected to a different sound object. In this case it will be a different sound recorder.
		//The last element of the chain is the output multiplexer which is putting back all the channels into a single sound buffer
		input.connectChannelTo(i, waves[i]).connectTo(*recorders[i]).connectTo(output.getOrCreateChannel(i));
	}
}
//--------------------------------------------------------------
void ofApp::update(){
	
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	stream.stop();
	stream.close();
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	for (auto& w: waves){
		w.draw();
	}
	
	ofBitmapFont bf;
	float y = 20;
	for(int i = 0; i < recorders.size(); i++){
		stringstream ss;
		auto bRec = recorders[i]->isRecording();
		ss << "Press the " << i << " key to " << (bRec?"stop":"start") << " recording channel " << i <<endl;
		if(bRec){
			ss << "Duration: " << recorders[i]->getRecordingElapsedTime() << endl;
			ss << "Recording to file: " << recorders[i]->getFileName() << endl;
			
		}
		ss << "Recorder State: " << recorders[i]->getRecStateString();
		
		ofDrawBitmapStringHighlight(ss.str(), 20, y, (bRec?ofColor::red:ofColor::black), (bRec?ofColor::black:ofColor::white));
		
		y += bf.getBoundingBox(ss.str(), 0, 0).height + 8;
	}	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}
//--------------------------------------------------------------
void ofApp::toggleChannelRecording(int chan){
	if(chan >= 0 && chan < recorders.size() ){
		if(recorders[chan]->isRecording()){
			recorders[chan]->stopRecording();
		}else{
			recorders[chan]->startRecording(ofToDataPath("Chan_"+ofToString(chan)+"_"+ofGetTimestampString()+".wav", true));
		}
	}
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	toggleChannelRecording(key  - '0');
	
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
