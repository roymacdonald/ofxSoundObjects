#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
		
		fullFileWaveform.makeMeshFromBuffer(player.getSoundFile().getBuffer());
		
		ofxSoundUtils::printOutputSoundDevices();
		
		auto outDevices = ofxSoundUtils::getOutputSoundDevices();
		
		
		ofSoundStreamSettings soundSettings;
		soundSettings.numInputChannels = 0;
		soundSettings.numOutputChannels = 2;
		soundSettings.sampleRate = player.getSoundFile().getSampleRate();
		soundSettings.bufferSize = 256;
		soundSettings.numBuffers = 1;
		/// IMPORTANT!
		/// in the following line put the correct number for the audio device you want to use. These have been printed to the console
		soundSettings.setOutDevice(outDevices[0]);
		
        stream.setup(soundSettings);
		
		
		stream.setOutput(output);

		player.connectTo(vuMeter).connectTo(output);
		
		player.play();
		player.setLoop(true);
		
		
		gui.setup("VU Meter params");
		gui.setPosition(0, 0);
		
		gui.add( rmsColor);
		gui.add( peakColor);
		gui.add( peakHoldColor);
		gui.add( borderColor);
		gui.add( peakHoldTime);
		gui.add( peakReleaseTime);
		gui.add( vuMeterWH);
		gui.add(player.volume);
		
		
		/// we need to use listeners to change the VUMeters static (global) parameters when changes are done in the GUI
		/// in this case we are using lambda functions instead of creating class member functions for each listener. Think of this as inlined functions
		listeners.push( rmsColor.newListener([&](ofColor&){ VUMeter::getRmsColor()  = rmsColor ; VUMeter::getForceRebuild() = true;  }));
		listeners.push( peakColor.newListener([&](ofColor&){ VUMeter::getPeakColor()  = peakColor ; VUMeter::getForceRebuild() = true;  }));
		listeners.push( peakHoldColor.newListener([&](ofColor&){ VUMeter::getPeakHoldColor()  = peakHoldColor ; VUMeter::getForceRebuild() = true;  }));
		listeners.push( borderColor.newListener([&](ofColor&){ VUMeter::getBorderColor()  = borderColor ; VUMeter::getForceRebuild() = true;  }));
		listeners.push( peakHoldTime.newListener([&](uint64_t&){ VUMeter::getPeakHoldTime()  = peakHoldTime ; }));
		listeners.push( peakReleaseTime.newListener([&](uint64_t&){ VUMeter::getPeakReleaseTime()  = peakReleaseTime ; }));
		listeners.push( resetPeak.newListener([&](){ vuMeter.resetPeak();}));
		listeners.push( vuMeterWH.newListener([&](float&){setVuMeterAndFileWaveformViews();}));
		
		setVuMeterAndFileWaveformViews();
		
	}
	
}
//--------------------------------------------------------------
void ofApp::setHelpText(){
	stringstream ss;
	
	ss << "Current VUMeter draw mode:  " << (bDrawVertical ? "VERTICAL" : "HORIZONTAL") << endl;
	ss << "Current VUMeter stack mode: " << (bStackVertical ? "VERTICAL" : "HORIZONTAL") << endl;
	ss << endl;
	ss << "Press [key] to : " << endl;
	ss << "      [d]      : Change VUMeter Draw Mode" << endl;
	ss << "      [s]      : Change VUMeter Stack Mode" << endl;
	ss << "      [h]      : Draw this help text" << endl;
	ss << "      [g]      : Toggle drawing the GUI" << endl;
	ss << "      [space]  : Play/Pause player " << endl;
	ss << endl;
	ss << "Press and drag mouse over waveform to move playhead"<< endl;
	
	helpText = ss.str();
}

//--------------------------------------------------------------
void ofApp::setVuMeterAndFileWaveformViews(){
	
	setHelpText();
	glm::vec3 pos (0,0,0);
	
	if(bDrawGui){
		pos = gui.getShape().getTopRight();
	}
	
	if(bDrawHelp){
		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(helpText, 20, 20);
		helpTextRect.set(pos.x, 0, ofGetWidth() - pos.x, bb.getMaxY() + 10);
		pos = helpTextRect.getBottomLeft();
	}

	ofRectangle vuRect;
	if(bDrawVertical){
		vuRect.set(pos, vuMeterWH.get(), ofGetHeight() - pos.y);
		fullFileWaveform.setup({vuRect.getTopRight(), ofGetWidth() - vuRect.getMaxX(), (float)ofGetHeight() - pos.y});
	}else{
		vuRect.set(pos, ofGetWidth() - pos.x, vuMeterWH.get());
		fullFileWaveform.setup({vuRect.getBottomLeft(), ofGetWidth() - vuRect.x, ofGetHeight() - vuRect.getMaxY()});
	}
	vuMeter.setup(vuRect,
				  (bDrawVertical ? VUMeter::VU_DRAW_VERTICAL : VUMeter::VU_DRAW_HORIZONTAL),
				  (bStackVertical ? VUMeter::VU_STACK_VERTICAL : VUMeter::VU_STACK_HORIZONTAL));
	
	
}
//--------------------------------------------------------------
void ofApp::exit(){
    stream.close();
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::drawPlayhead(){

	ofSetColor(ofColor::red);
	float playhead = ofMap(player.getPosition(), 0,1, fullFileWaveform.getMinX(),fullFileWaveform.getMaxX());
	ofDrawLine(playhead, fullFileWaveform.getMinY(), playhead, fullFileWaveform.getMaxY());
	
	if(fullFileWaveform.inside(ofGetMouseX(), ofGetMouseY())){
		ofSetColor(ofColor::cyan);
		ofDrawLine(ofGetMouseX(), fullFileWaveform.getMinY(), ofGetMouseX(), fullFileWaveform.getMaxY());
	}
	

}
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(60);
	
	
	ofSetColor(ofColor::white);
	fullFileWaveform.draw();

	drawPlayhead();
	
	vuMeter.draw();
	
	
	if(bDrawGui) gui.draw();
	
	if(bDrawHelp){
		ofSetColor(255);
		ofDrawBitmapString(helpText,helpTextRect.x + 20,20);
	}
	
	
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'd'){
		bDrawVertical ^= true;
		setVuMeterAndFileWaveformViews();
	}else if(key == 's'){
		bStackVertical ^= true;
		setVuMeterAndFileWaveformViews();
	}else if(key == 'h'){
		bDrawHelp ^= true;
		setVuMeterAndFileWaveformViews();
	}else if(key == 'g'){
		bDrawGui ^= true;
		setVuMeterAndFileWaveformViews();
	}
}
//--------------------------------------------------------------
bool ofApp::setPlayerPositionToMouse(bool bPause ){
	if(fullFileWaveform.inside(ofGetMouseX(), ofGetMouseY())){
		if(bPause){
			bPlayerWasPlaying = player.isPlaying();
			player.setPaused(true);
		}
		player.setPositionMS(ofMap(ofGetMouseX(), fullFileWaveform.x, fullFileWaveform.getMaxX(), 0, player.getDurationMS()));
		return true;
	}
	return false;
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	setPlayerPositionToMouse(false);
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	setPlayerPositionToMouse(true);
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if(setPlayerPositionToMouse(false) && bPlayerWasPlaying){
		player.setPaused(false);
	}
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
		setVuMeterAndFileWaveformViews();
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
