#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofFileDialogResult result = ofSystemLoadDialog();
	if (result.bSuccess) {
        player.load(result.getPath());
		
		
		//----- Sound stream setup begin -------.
		// the sound stream is in charge of dealing with your computers audio device.
		ofSoundStreamSettings soundSettings;
		soundSettings.numInputChannels = 0;
		soundSettings.numOutputChannels = 2;
		soundSettings.sampleRate = player.getSoundFile().getSampleRate();
		soundSettings.bufferSize = 256;
		soundSettings.numBuffers = 1;
		
        stream.setup(soundSettings);
		
		// it is important to set up which object is going to deliver the audio data to the sound stream.
		// thus, we need to set the stream's output. The output object is going to be the last one of the audio signal chain, which is set up further down
		stream.setOutput(output);
		//-------Sound stream setup end -------.
		
		// the waveformDraw receives the rectangle where it is going to be drawn
		fullFileWaveform.setup(0,0, ofGetWidth(), ofGetHeight()/3);
		
		wave.setup(0, fullFileWaveform.getMaxY(), ofGetWidth(), ofGetHeight() - fullFileWaveform.getMaxY());
        
		
		// Audio signal chain setup.
		// Each of our objects need to connect to each other in order to create a signal chain, which ends with the output; the object that we set as the sound stream output.
        player.connectTo(wave).connectTo(output);
        
		

		// ------- GUI setup begin -------
        gui.setup();
        gui.add(pan.set("PAN", 0, -1,1));
        gui.add(player.volume);
        pan.addListener(this, &ofApp::panChanged);
        gui.add(speed.set("Speed", 1, 0, 10));
		
		//----- gui listeners
		speed.addListener(this, &ofApp::speedChanged);
		// ------- GUI setup end -------
		
		//
		fullFileWaveform.makeMeshFromBuffer( player.getSoundFile().getBuffer());
		
		ofBackground(0);
		
		// Also we are going to read the data from the sound player and create a mesh to draw its waveform
		// we will create one waveform for each channel in the sound player

//		waveforms.resize(player.getSoundFile().getNumChannels());
//		float h = ofGetHeight()/waveforms.size();
		
		
		// we need to get the sound buffer from the sound file in order to get the whole file's data.
		// calling player.getBuffer(), which actually is a function, will return the players current buffer, the one that is being sent to the sound device, so it will not work for what we are trying to achieve.
//		ofSoundBuffer& buff = player.getSoundFile().getBuffer();
		
//		for(int i = 0; i < waveforms.size(); i++){
//			ofSoundBuffer b;
//			buff.getChannel(b, i); // we need to extract each channel from the sound buffer
//			
//			waveforms[i].setMode(OF_PRIMITIVE_LINE_STRIP);
//			for(int j = 0; j < b.getBuffer().size(); j++ ){
//				// map the buffer sample x position so we can fit the whole file into the app's window
//				float x = ofMap(j, 0, b.getBuffer().size(), 0, ofGetWidth());
//				// The sound data comes as a normalized float, which means that its values can go from -1 to 1.
//				// We want to map these values into the desired height of our mesh, and position it acordingly.
//				float y = ofMap(b.getBuffer()[j], -1, 1, i*h, (i+1)*h);
//				waveforms[i].addVertex({x, y,0.0});
//			}
//		}
		
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
//	for(auto&w:waveforms){
//		w.draw();
//	}
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
