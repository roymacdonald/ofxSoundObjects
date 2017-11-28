#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    size_t count = 0;
    
    //this will open a dialog to select a folder in which you should have audio files, ideally more than one but not an excesive amount, say maximum 10.
    // Each audio file will be routed to a different output of your multi channel audio interface.
    ofFileDialogResult r = ofSystemLoadDialog("Select folder with audio files(wav, aiff, mp3)", true);
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
                    auto chans = players[i].getSoundFile().getNumChannels();
                    //create a vector<int> object that will define which are the channels you want to connect to
                    //
                    vector<int> v;
                    v.resize(chans);
                    for(int j =0 ; j < v.size(); j++){
                        v[j] = j+count;
                    }
                    // the following line of code is the important one
                    //
                    // As an alternative you could define the channels manually in the following way
                    // output.getOrCreateChannelGroup({0,1});
                    // which will create an output for channels 0 and 1.
                    // Channels start at 0. The first channel of your audio interface is 0 not 1.
                    ofxSoundObject& playerOutput = output.getOrCreateChannelGroup(v);
                    // the next line is just a regular way of connecting sound objects.
                    players[i].connectTo(playerOutput);
                    players[i].play();
                    count += chans ;
                }
            }
        }
    }
    // Setup the sound stream.
    ofSoundStreamSettings settings;
    settings.bufferSize = 256;
    settings.numBuffers = 1;
    settings.numInputChannels = 0;
    settings.numOutputChannels = count;
    if(players.size()){
        // we setup the samplerate of the sound stream according to the one of the first player
        settings.sampleRate = players[0].getSoundFile().getSampleRate();
    }
    
    auto devices = ofSoundStreamListDevices();
    // SUPER IMPORTANT!!!
    // this is where you set which audio interface to use.
    // The number you put inside the square braquets in devices[ ] is the one
    settings.setInDevice(devices[3]);
    settings.setOutDevice(devices[3]);
    
    
    stream.setup(settings);
    stream.setOutput(output);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
 
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
