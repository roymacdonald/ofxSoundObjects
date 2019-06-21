#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(0);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
		// the second argument passed to player.load is a boolean indicating if you want to 
		// open the file in streaming mode (default is false). This means that the audio data
		// is not actually read and loaded from disk into memory when you call player.load, 
		// instead it is being read from the file on disk as your app requests it. It really 
		// depends on the scenario which mode is better. Non streaming mode will load the 
		// complete file into ram memory, thus its memory footprint can be large if the file 
		// size is large (it can actually be much larger than the file size itself). So if you
		// load a lot of files which are large it might be better to use the streaming mode. 
		// On the other hand, streaming mode will allocate a very small amount of memory, 
		// although it will read from the disk directly on demand and perform any decompresion
		// and sound format conversion, which might be less performant than non streaming mode.
		// The difference in terms of performance between these two modes depends on your 
		// computer so you might want to bench mark both. 
        player.load(result.getPath(), true);
	
		
		
		//----- Sound stream setup begin -------.
		// the sound stream is in charge of dealing with your computers audio device.
		// lets print to the console the sound devices that can output sound.
		ofxSoundUtils::printOutputSoundDevices();
		
		auto outDevices = ofxSoundUtils::getOutputSoundDevices();
		
		// IMPORTANT!!!
		// In the following line of code is where you set which audio interface to use.
		// the index is the number printed in the console inside [ ] before the interface name 
		
		int outDeviceIndex = 0;
		
		ofSoundStreamSettings soundSettings;
		soundSettings.numInputChannels = 0;
		soundSettings.numOutputChannels = 2;
		soundSettings.sampleRate = player.getSoundFile().getSampleRate();
		soundSettings.bufferSize = 256;
		soundSettings.numBuffers = 1;
		soundSettings.setOutDevice(outDevices[outDeviceIndex]);
	
        stream.setup(soundSettings);
		
		
		// it is important to set up which object is going to deliver the audio data to the sound stream.
		// thus, we need to set the stream's output. The output object is going to be the last one of 
		// the audio signal chain, which is set up further down.
		stream.setOutput(output);
		//-------Sound stream setup end -------.
		
		// waveformDraw::setup receives the rectangle where it is going to be drawn
		// fullFileWaveform will be in the top 1/3 of the screen.
  
		fullFileWaveform.setup(0,0, ofGetWidth(), ofGetHeight()/3);
		// wave will be on the lower 2/3 of the screen
		// wave will render the waveform of the sound data as it is passing through the signal change.
		wave.setup(0, fullFileWaveform.getMaxY(), ofGetWidth(), ofGetHeight() - fullFileWaveform.getMaxY());
		
		// The fullFileWaveform will render a static mesh, which does not change over time. 
		// waveformDraw has a handy function which will create its internal meshes from an ofSoundBuffer
		// object passed. In this case we pass to it the complete sound buffer of the sound file loaded.
		
		fullFileWaveform.makeMeshFromBuffer( player.getSoundFile().getBuffer());
		
		
		// Audio signal chain setup.
		// Each of our objects need to connect to each other in order to create a signal chain, which ends with the output; the object that we set as the sound stream output.
		// notice that here we dont connect fulFileWaveform. It does not need to.
		player.connectTo(wave).connectTo(output);

        
		

		// ------- GUI setup begin -------
        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        gui.add(player.volume);
        gui.add(speed.set("Speed", 1, 0, 10));
		
		//----- gui listeners
		pan.addListener(this, &ofApp::panChanged);
		speed.addListener(this, &ofApp::speedChanged);
		// ------- GUI setup end -------
		
		player.play();
			 
		
		// set if you want to either have the player looping (playing over and over again) or not (stop once it reaches the its end).
		player.setLoop(true);
		
		if(player.getIsLooping()){
			// if the player is not looping you can register  to the end event, which will get triggered when the player reaches the end of the file. 
			playerEndListener = player.endEvent.newListener(this, &ofApp::playerEnded);
		}
	}
	
}
//--------------------------------------------------------------
void ofApp::playerEnded(size_t & id){
	// This function gets called when the player ends. You can do whatever you need to here.
	// This event happens in the main thread, not in the audio thread.
	cout << "the player's instance " << id << "finished playing" << endl;
	
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
void ofApp::exit(){
    stream.close();
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	ofSetColor(ofColor::white);
	
	fullFileWaveform.draw();

	ofSetColor(ofColor::red);
	float playhead = ofMap(player.getPosition(), 0,1,0,ofGetWidth()); 
	ofDrawLine(playhead, 0, playhead, ofGetHeight());
	ofSetColor(ofColor::cyan);
	ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), ofGetHeight());
	
	ofSetColor(ofColor::white);
	wave.draw();
	
	ofSetColor(ofColor::yellow);
	player.drawDebug(20, gui.getShape().getMaxY() + 20);


    gui.draw();
	
	ofDrawBitmapStringHighlight("click any where on the window to set the playhead", 20, ofGetHeight() - 30); 
	
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//player.setPosition(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1));
	player.setPositionMS(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, player.getDurationMS()));
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
