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
//		loadPath = "/Users/roy/openFrameworks/examples/sound/soundPlayerExample/bin/data/sounds";
#endif
		
		loadFolder(loadPath);
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
	
	inDeviceIndex = 0;
	outDeviceIndex = 0;
	

	// Setup the sound stream.
	ofSoundStreamSettings settings;
	settings.bufferSize = 256;
	settings.numBuffers = 1;
	settings.numInputChannels =  inDevices[inDeviceIndex].inputChannels;
	settings.numOutputChannels = outDevices[outDeviceIndex].outputChannels;
	
	if(players.size()){
		// we setup the samplerate of the sound stream according to the one of the first player
		if(players[0])
			settings.sampleRate = players[0]->getSoundFile().getSampleRate();
	}else{
		ofLogWarning("ofApp::setup", "could not set sample rate for soundstream") ;
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
void ofApp::loadFolder(const string& path){
	ofFile f(path);
	if(f.isDirectory()){
		ofDirectory dir(path);
		dir.allowExt("wav");
		dir.allowExt("aiff");
		dir.allowExt("mp3");
		dir.listDir();
		auto startIndex = players.size();
		players.resize( startIndex + dir.size());
		for (int i = 0; i < dir.size(); i++) {
			players[startIndex + i] = make_shared<ofxSoundPlayerObject>();
			if(players[startIndex + i]->load(dir.getPath(i))){
				players[startIndex + i]->connectTo(mixer);
				players[startIndex + i]->play();
				players[startIndex + i]->setLoop(true);
			}
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
	
	

	mixerRenderer.draw();
	stringstream ss;
	ss << "Press l key to load mixer settings." << endl;
	ss << "Press s key to save mixer settings." << endl;
	ss << "Press e key to toggle slider's mouse interaction." << endl;
	ss << "Press n key to toggle non-slider mode.";
#ifdef OFX_SOUND_ENABLE_MULTITHREADING 
	ss << endl << "Press the space bar to load more audio file players";
#endif
	ofBitmapFont bf;
	
	
	
	
	stringstream ss2;
	ss2 << "Sliders enabled: " << (mixerRenderer.isSlidersEnabled()?"YES":"NO") << endl;
	ss2 << "Non Slider Mode: " << (mixerRenderer.isNonSliderMode()?"YES":"NO") << endl;
	
	
	
	auto r2 = bf.getBoundingBox(ss2.str(), 0, 0);
	
	r2.x = ofGetWidth() - 20 - r2.width;
	
	ofDrawBitmapStringHighlight(ss2.str(), r2.x, 20);
	
	
	auto r = bf.getBoundingBox(ss.str(), 0, 0);
	
	r.x = r2.x - 20 - r.width;
	
	ofDrawBitmapStringHighlight(ss.str(), r.x, 20);
	
	
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
		loadFolder(loadPath);
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
