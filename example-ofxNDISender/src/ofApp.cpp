#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	
	ofxSoundUtils::printOutputSoundDevices();
	ofxSoundUtils::printInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 2;
	soundSettings.numOutputChannels = 2;
	soundSettings.sampleRate = 48000;
	soundSettings.bufferSize = 256;
	/// IMPORTANT!
	/// in the following line put the correct number for the audio device you want to use. These have been printed to the console
	soundSettings.setOutDevice(outDevices[0]);
	soundSettings.setInDevice(inDevices[0]);
	
	stream.setup(soundSettings);
	
	stream.setInput(input);
	stream.setOutput(output);
	
	
	/// SUPER IMPORTANT!
	/// In the following line put the NDI name of the sender. This is what's going to be used to identify it on the network.
	/// You can additionally pass a second string for the group where it's going to be published.
	//	sender.setup("ofxSoundObject NDI Sender");
	sender.setup("ofxSoundObject NDI Sender", "ofx");
	
	
	/// EVEN MORE IMPORTANT !
	/// even when you might want to send audio through NDI and not output to your speakers you need a soundstream as it will be used to clock the NDI stream. The sender needs to be connected to a signal chain which has an output stream at its end. Without it it will not work.
	/// As such you might also want to mute the output of the sender.
	
	sender.setMuteOutput(true);
	
	// the current sound source, which is setup in the setInput function will connect to the vuMeter, thus completing the signal chain
	vuMeter.connectTo(wave).connectTo(sender).connectTo(output);
	
	
    sinGen.setup(440);
	
	setInput(INPUT_SINE_WAVE);
	
}

//--------------------------------------------------------------
void ofApp::update(){
	if(inputIndex == INPUT_SINE_WAVE ){
		sinGen.freq = ofMap(ofGetMouseX(), 0, ofGetWidth(), 20, 20000);
		sinGen.amplitude = ofMap(ofGetMouseY(), 0, ofGetWidth(), 0, 1);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofSetBackgroundColor(60);
	
	ofSetColor(ofColor::white);
	
	vuMeter.draw();
	wave.draw();
	
	if(bDrawHelp){
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
		ofSetColor(40);
#else
		ofSetColor(ofColor::red, (unsigned char)ofMap(sin(ofGetElapsedTimef()*10), -1, 1, 50, 255));
#endif

		ofDrawRectangle(helpTextRect);
		ofSetColor(255);
		ofDrawBitmapString(helpText,helpTextRect.x + 20,20);
	}
	
	
}
//--------------------------------------------------------------
void ofApp::setHelpText(){
	stringstream ss;
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	ss << "Press [key] to : " << endl;;
	ss << "      [space]  : switch to next input ( file player/sine wave/ live mic )" << endl;;
	ss << "      [l]      : Load an audio file to the sound player" << endl;
	ss << "      [m]      : mute / unmute the ndi output to speakers." << endl;
	ss << "      [h]      : Toggle Draw this help text" << endl;
	
	ss  <<  "CURRENT INPUT: ";
	
	
		 if(inputIndex == INPUT_PLAYER){    ss  <<  "Audio File Player"; }
	else if(inputIndex == INPUT_SINE_WAVE){ ss  <<  "Sine Wave Generator"; }
	else if(inputIndex == INPUT_LIVE_MIC){  ss  <<  "Live input (mic)"; }
	ss << endl;
	
	if(inputIndex == INPUT_SINE_WAVE){
		ss << "Move the mouse to change the sine wave parameters. x axis: frequency. y axis: volume" << endl;
	}
#else
	ss << "ofxNDI use is disabled. !" <<endl <<endl;
	ss << "Go to the file ofxSoundObjectsConstants.h \nand uncomment the line that reads //#define OFX_SOUND_OBJECTS_USE_OFX_NDI" <<endl;
	ss << "You will also need to properly add ofxNDI to the project via Project Generator.\nRead the ofxSoundObject's readme file for more info" <<endl;
#endif
	helpText = ss.str();
}

//--------------------------------------------------------------
void ofApp::setViewports(){
	
	setHelpText();
	glm::vec3 pos (0,0,0);
	
	if(bDrawHelp){
		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(helpText, 20, 20);
		helpTextRect.set(pos.x, 0, ofGetWidth() - pos.x, bb.getMaxY() + 10);
		pos = helpTextRect.getBottomLeft();
	}
	
	ofRectangle vuRect(pos, 40, ofGetHeight() - pos.y);
	
	wave.setup({vuRect.getTopRight(), ofGetWidth() - vuRect.getMaxX(), (float)ofGetHeight() - pos.y});
	
	vuMeter.setup(vuRect,VUMeter::VU_DRAW_VERTICAL,VUMeter::VU_STACK_VERTICAL);
	
	
}
//--------------------------------------------------------------
void ofApp::setInput(InputIndex newInput){
	if(newInput != inputIndex){
		inputIndex = newInput;
		if(inputIndex == INPUT_SINE_WAVE){
			
			sinGen.connectTo(vuMeter);
			
		}else if(inputIndex == INPUT_PLAYER){
			if(!player.isLoaded()){
				loadDialogPlayer();
			}
			
			player.connectTo(vuMeter);
			
		}else if(inputIndex == INPUT_LIVE_MIC){
			input.connectTo(vuMeter);
		}
		setViewports();
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
		setInput( InputIndex((inputIndex +1) % (int)NUM_INPUTS));
	}else if(key == 'l'){
		loadDialogPlayer();
	}else if(key == 'm'){
		sender.setMuteOutput(!sender.isMuteOutput());
	}else if(key == 'h'){
		bDrawHelp ^= true;
		setViewports();
	}

	
}
//--------------------------------------------------------------
void ofApp::loadDialogPlayer(){
	ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac");
	if (result.bSuccess) {
		player.load(result.getPath());
		player.play();
		setInput(INPUT_PLAYER);
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
	setViewports();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
