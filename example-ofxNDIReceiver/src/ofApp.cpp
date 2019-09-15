#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	
		ofxSoundUtils::printOutputSoundDevices();
		
		auto outDevices = ofxSoundUtils::getOutputSoundDevices();
		
		
		ofSoundStreamSettings soundSettings;
		soundSettings.numInputChannels = 0;
		soundSettings.numOutputChannels = 2;
		soundSettings.sampleRate =48000;
		soundSettings.bufferSize = 256;
		soundSettings.numBuffers = 1;
		/// IMPORTANT!
		/// in the following line put the correct number for the audio device you want to use. These have been printed to the console
		soundSettings.setOutDevice(outDevices[0]);
		
        stream.setup(soundSettings);
	
	
		stream.setOutput(output);

	
		/// SUPER IMPORTANT!
		/// In the following line put the NDI source name or url that you want to connect to.
		/// Look at the console as the available sources are printed there.
		/// Note: log level needs to be verbose in order to see this in the console. 	ofSetLogLevel(OF_LOG_VERBOSE);
		/// If a blank string is passed it will connect to the first available source
		receiver.setup("ofxSoundObject NDI Sender", "ofx");
		receiver.connectTo(vuMeter).connectTo(wave).connectTo(output);
		
	
		setViewports();
	
	
}
//--------------------------------------------------------------
void ofApp::setHelpText(){
	stringstream ss;
#ifdef OFX_SOUND_OBJECTS_USE_OFX_NDI
	ss << "Press [key] to : " << endl;;
	ss << "      [h]      : Draw this help text" << endl;
	ss << "is receiver connected : " << boolalpha << receiver.isConnected() <<endl;
	ss << "Source Name: " << receiver.getSourceName() <<endl;
	ss << "Source URL: " << receiver.getSourceUrl() <<endl;
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
	
	vuMeter.setup(vuRect,VUMeter::VU_DRAW_VERTICAL,VUMeter::VU_STACK_HORIZONTAL);
	
	
}
//--------------------------------------------------------------
void ofApp::exit(){
    stream.close();
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(60);
	
	//this is called on each draw so to update the drawing of the stream metadata
//	setViewports();
	
	
	ofSetColor(ofColor::white);
	
	wave.draw();
	
	vuMeter.draw();
	
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
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'h'){
		bDrawHelp ^= true;
		setViewports();
	}
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
		setViewports();
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
