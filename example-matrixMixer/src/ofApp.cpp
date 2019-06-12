#include "ofApp.h"

//#define USE_LOAD_DIALOG // comment this line if you dont want to use the system load dialog

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxSoundUtils::printInputSoundDevices();
	ofxSoundUtils::printOutputSoundDevices();
	
	
#ifdef USE_LOAD_DIALOG
//	this will open a dialog to select a folder in which you should have audio files.
	// Each audio file will be connected to a new input of your matrix mixer.
	// by default the file dialog should open in the soundplayer example's folder that has some audio files so you can simply pres open if you want to try those 
	ofFileDialogResult r = ofSystemLoadDialog("Select folder with audio files(wav, aiff, mp3)", true, "../../../../../examples/sound/soundPlayerExample/bin/data/sounds");
	if(r.bSuccess){
		loadPath = r.getPath();
#else
		// change this path if you want to use another one and not use the system load dialog
		loadPath = ofToDataPath("../../../../../examples/sound/soundPlayerExample/bin/data/sounds");

#endif
		
		loadFolder(loadPath, false);
#ifdef USE_LOAD_DIALOG
	}
#endif
	//Here we also connect the audio input to the mixer
	//
	input.connectTo(mixer);
	
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
		// IMPORTANT!!!
	// The following two lines of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name 
	// You can use a different input and output device.
	
	inDeviceIndex = 1;
	outDeviceIndex = 0;
	

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
	mixer.setOutputStream(stream);
	
	mixer.setOutputVolumeForAllChannels(1);
	
//		mixerRenderer.setObject(&mixer);
	mixerRenderer.obj = &mixer;
	mixerRenderer.enableSliders();
	mixerSettingsXmlPath = "mixerSettings.xml";
	
	
}
//--------------------------------------------------------------
void ofApp::loadFolder(const string& path, bool bReload){
	ofFile f(path);
	
	//set bLoadAsync to true if you want to load the audio files on a different thread. 
	bool bLoadAsync = true;
	if(f.isDirectory()){
		ofDirectory dir(path);
		dir.allowExt("wav");
		dir.allowExt("aiff");
		dir.allowExt("mp3");
		dir.listDir();
		size_t startIndex = 0;
		if(!bReload) {
			startIndex = players.size();
			players.resize( startIndex + dir.size());
		}
		for (int i = 0; i < dir.size(); i++) {
		if(!bReload) {
			players[startIndex + i] = make_shared<ofxSoundPlayerObject>();
		}
			if(!bLoadAsync){
				if(players[startIndex + i]->load(dir.getPath(i))){
					players[startIndex + i]->connectTo(mixer);
		
					players[startIndex + i]->play();
			
				}
			}else{
				if(players[startIndex + i]->loadAsync(dir.getPath(i), true)){	
					players[startIndex + i]->connectTo(mixer);
					//				players[startIndex + i]->play();// dont call play immediately after calling load Async. 
					// instead set to true the second argument of ofxSoundPlayerObject::loadAsync (few lines above)
					// when this argument is set to true it will start playing immediately after loading, otherwise it will not.	
				}
			}
			players[startIndex + i]->setLoop(true);
		}
	}	
}
//--------------------------------------------------------------
void ofApp::update(){
	
//	float x = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1, true);
//	float y = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, 1, true);
	//the following sets the volume for the second connection (sound player) for its first input channel and its second output channels based on the mouse x position 
//	mixer.setVolumeForConnectionChannel(x, 1, 0, 0);
	
	// the following sets the volume of the matrix channels. In this case it would be the channel number 7 for which you can find out to which sound player it belongs by calling 
	//mixer.getConnectionIndexAtInputChannel(7);
//	mixer.setVolumeForChannel(y, 7, 1);
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	stream.stop();
	stream.close();
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	

	
	stringstream ss;
	ss << "Press l key to load mixer settings." << endl;
	ss << "Press s key to save mixer settings." << endl;
	ss << "Press e key to toggle slider's mouse interaction." << endl;
	ss << "Press n key to toggle non-slider mode." << endl;
	ss << "Press up or down arrow keys to inc/dec selected connection"<< endl;
	ss << "Press left or right arrow keys to inc/dec selected Channel";
#ifdef OFX_SOUND_ENABLE_MULTITHREADING 
	ss << endl << "Press the space bar to load more audio file players";
#endif
	ofBitmapFont bf;
	
	
	
	
	stringstream ss2;
	ss2 << "Sliders enabled: " << (mixerRenderer.isSlidersEnabled()?"YES":"NO") << endl;
	ss2 << "Non Slider Mode: " << (mixerRenderer.isNonSliderMode()?"YES":"NO");
	
	
	auto r2 = bf.getBoundingBox(ss2.str(), 0, 0);
	
	r2.x = ofGetWidth() - 20 - r2.width;
	r2.y = 20;
	ofDrawBitmapStringHighlight(ss2.str(), r2.x, r2.y);
	
	
	auto r = bf.getBoundingBox(ss.str(), 0, 0);
	
	r.x = r2.x - 20 - r.width;
	
	ofDrawBitmapStringHighlight(ss.str(), r.x, 20);
	
	
	// Use the selected connection and channel to draw its rms and peak values taken from the mixer VUMeter
	
	auto rms = mixer.getVUMeterForConnection(selectedConnection).getRmsForChannel(selectedChannel);
	auto peak = mixer.getVUMeterForConnection(selectedConnection).getPeakForChannel(selectedChannel);
	
	
	stringstream ss3;
	ss3 << "Selected Connection " << selectedConnection << endl; 
	ss3 << "Selected Channel    " << selectedChannel;
	
	auto r3 = bf.getBoundingBox(ss3.str(), 0, 0);
	r3.x = r2.x;
	auto tempY = r2.getMaxY() + 10;
	r3.y += tempY;
	ofDrawBitmapStringHighlight(ss3.str(), r3.x, tempY);
	
	r3.y += r3.height + 5;
	r3.x -= 4;
	r3.height = r.getMaxY() + 20  - r3.y;
	
	
	auto rRms = r3;
	auto rPeak = r3;
	rPeak.width = ofMap(peak, -1, 1, 0, r3.width);
	rRms.width = ofMap(rms, -1, 1, 0, r3.width);
	
	ofPushStyle();
	ofSetColor(ofColor::yellow);
	ofDrawRectangle(rPeak);
	
	
	ofSetColor(ofColor::red);
	ofDrawRectangle(rRms);
	
	ofPopStyle();
	
	
	ofRectangle mixRect;
	mixRect.x = 20;
	mixRect.y = 20 + r.y + r.height +20; 
	mixRect.width = ofGetWidth() - 40;
	mixRect.height = ofGetHeight() - mixRect.y -20; 
	
	
	mixerRenderer.draw(mixRect);
	
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 's'){
		mixer.save(mixerSettingsXmlPath);
	}else if(key == 'l'){
		mixer.load(mixerSettingsXmlPath);
	}else if(key == 'e'){
		mixerRenderer.toggleSliders();
	}else if(key == 'n'){
		mixerRenderer.setNonSliderMode(!mixerRenderer.isNonSliderMode());
	}else if(key == ' '){
		loadFolder(loadPath, false);
	}else if(key == 'r'){
		loadFolder(loadPath, true);
	}else if(key == OF_KEY_UP){
		(++selectedConnection)%= mixer.getNumInputObjects();
	}else if(key == OF_KEY_DOWN){
		--selectedConnection;
		if(selectedConnection < 0) selectedConnection = mixer.getNumInputObjects() - 1;
	}else if(key == OF_KEY_LEFT){
		--selectedChannel;
		if(selectedChannel < 0) selectedChannel = mixer.getConnectionNumberOfChannels(selectedConnection) - 1;
	}else if(key == OF_KEY_RIGHT){
		(++selectedChannel)%= mixer.getConnectionNumberOfChannels(selectedConnection);
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
