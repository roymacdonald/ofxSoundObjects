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
	
	
	output.connectTo(mixer);
	mixer.setOutputStream(stream);
	
	mixerRenderer.obj = &mixer;
	mixerRenderer.enableSliders();
	
//	output.setOutputStream(stream);
	
	// we resize our recorders vector so there is one recorder per audio channel.
	recorders.resize(inDevices[inDeviceIndex].inputChannels);
#ifdef USE_WAVES
	waves.resize(recorders.size());	
	float h = (ofGetHeight() - 40)/(float)waves.size();
	float y = 20;
	float w = 100;
#endif
	for(size_t i = 0; i < recorders.size(); i++){
		recorders[i] = make_unique<ofxSoundRecorderObject>();
#ifdef USE_WAVES
		waves[i].setup(10, y, w, h);
		y += h;
#endif
		// using the connectChannelTo method of the inputMultiplezer we can tell that each audio channel gets connected to a different sound object. In this case it will be a different sound recorder.
		//The last element of the chain is the output multiplexer which is putting back all the channels into a single sound buffer
#ifdef USE_WAVES
		input.connectChannelTo(i, waves[i]).connectTo(*recorders[i]).connectTo(output.getOrCreateChannel(i));
#else
		input.connectChannelTo(i, *recorders[i]).connectTo(output.getOrCreateChannel(i));
#endif
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
	ofBitmapFont bf;
	stringstream ss2;
	ss2 << "Sliders enabled: " << (mixerRenderer.isSlidersEnabled()?"YES":"NO") << endl;
	ss2 << "Non Slider Mode: " << (mixerRenderer.isNonSliderMode()?"YES":"NO") << endl;
	
	auto r2 = bf.getBoundingBox(ss2.str(), 0, 0);
	
	r2.x = ofGetWidth() - 20 - r2.width;

	ofDrawBitmapStringHighlight(ss2.str(), r2.x, 20);
	r2.y += 20;	

	ofRectangle mixRect;
	mixRect.x = 150;
	mixRect.y = r2.getMaxY() + 80;
 
	mixRect.width = ofGetWidth() - mixRect.x - 40;
	mixRect.height = ofGetHeight() - mixRect.y - 40; 
	
	mixerRenderer.draw(mixRect);
	
#ifdef USE_WAVES
	auto leftR = mixerRenderer.getLeftRect();
	
	ofRectangle waveRect;
	waveRect.x = 10;
	waveRect.y = leftR.y;
	waveRect.width = leftR.x - waveRect.x;
	waveRect.height = leftR.height/waves.size();
	
	for(int i =0; i< waves.size() && i < recorders.size(); i++){
		
		auto bRec = recorders[i]->isRecording();
		
		waves[i].set(waveRect);
		ofPushStyle();
		ofSetColor(bRec?ofColor::red:ofColor(50));
		ofFill();
		ofDrawRectangle(waveRect);
		ofNoFill();
		ofSetColor(100);
		ofDrawRectangle(waveRect);
		
		ofPopStyle();
		
		
		waves[i].draw();
	
		
		stringstream ss;
		
		ss << "Chan: " << i << " Key '" << i << "' to " << (bRec?"stop":"rec");
		
		if(bRec){
			ss << endl << "Duration: " << recorders[i]->getRecordingElapsedTime();
//			ss << "Recording to file: " << recorders[i]->getFileName() << endl;
			
		}
//		ss << "Recorder State: " << recorders[i]->getRecStateString();
		
		ofSetColor(255);
		ofDrawBitmapString(ss.str(), leftR.x, waveRect.y + 20);
		
		
		waveRect.y += waveRect.height;

		
	}
#endif
	
		
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}
//--------------------------------------------------------------
bool ofApp::toggleChannelRecording(int chan){
	if(chan >= 0 && chan < recorders.size() ){
		if(recorders[chan]->isRecording()){
			recorders[chan]->stopRecording();
		}else{
			recorders[chan]->startRecording(ofToDataPath("Chan_"+ofToString(chan)+"_"+ofGetTimestampString()+".wav", true));
		}
		return true;
	}
	return false;
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(!toggleChannelRecording(key  - '0')){
		if(key == 'e'){
			mixerRenderer.toggleSliders();
		}else if(key == 'n'){
			mixerRenderer.setNonSliderMode(!mixerRenderer.isNonSliderMode());
		}
	}
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
