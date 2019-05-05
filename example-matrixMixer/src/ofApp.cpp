#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
	
	
	

	
	//this will open a dialog to select a folder in which you should have audio files, ideally more than one but not an excesive amount, say maximum 10.
	// Each audio file will be routed to a different output of your multi channel audio interface.
	// by default the file dialog should open in the soundplayer example's folder that has some audio files so you can simply pres open if you want to try those 
	ofFileDialogResult r = ofSystemLoadDialog("Select folder with audio files(wav, aiff, mp3)", true, "../../../../../examples/sound/soundPlayerExample/bin/data/sounds");
	if(r.bSuccess){
		ofFile f(r.getPath());
		if(f.isDirectory()){
			ofDirectory dir(r.getPath());
			dir.allowExt("wav");
			dir.allowExt("aiff");
			dir.allowExt("mp3");
			dir.listDir();
			players.resize(dir.size());
			for (int i = 0; i < dir.size(); i++) {
				if(players[i].load(dir.getPath(i))){
					players[i].connectTo(mixer);
					players[i].play();
					players[i].setLoop(true);
				}
			}
		}
	}

	input.connectTo(mixer);
	
	auto inDevices = ofxSoundObjects::getInputSoundDevices();
	auto outDevices = ofxSoundObjects::getOutputSoundDevices();
	
	
	ofxSoundObjects::printInputSoundDevices();
	ofxSoundObjects::printOutputSoundDevices();
	
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
		settings.sampleRate = players[0].getSoundFile().getSampleRate();
	}
	
	
	settings.setInDevice(inDevices[inDeviceIndex]);
	settings.setOutDevice(outDevices[outDeviceIndex]);
	
	
	stream.setup(settings);
	stream.setInput(input);
	stream.setOutput(mixer);

	
//		mixerRenderer.setObject(&mixer);
	mixerRenderer.obj = &mixer;

	mixerSettingsXmlPath = "mixerSettings.xml";
	mixer.load(mixerSettingsXmlPath);
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	

	mixerRenderer.draw();

//	stringstream ss;
//	for(auto & c: output.getChannelGroups()){
//		auto pl = ((ofxSoundPlayerObject*)c.second.getInputObject());
//		auto& f = pl->getSoundFile();
//		
//		ss << "Playing " << ofFilePath::getBaseName(f.getPath()) << " to channels " << ofToString(c.first)<<endl;
//		ss << "        " << msToMMSS(pl->getPositionMS()) << " - " << msToMMSS(pl->getDurationMS()) << endl <<endl;
//		
//	}
//	ofDrawBitmapStringHighlight(ss.str(), 100, 130);
 
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
