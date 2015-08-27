#include "ofxSoundObjects.h"

//------------------------------------------------------------
void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, ofBaseApp * appPtr){
    if( appPtr == NULL ){
        appPtr = ofGetAppPtr();
    }
    ofxSoundStreamSetup(nOutputChannels, nInputChannels, appPtr, 44100, 256, 4);
}

//------------------------------------------------------------
void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, int sampleRate, int bufferSize, int nBuffers){
    ofxSoundStreamSetup(nOutputChannels, nInputChannels, ofGetAppPtr(), sampleRate, bufferSize, nBuffers);
}

//------------------------------------------------------------
void ofxSoundStreamSetup(int nOutputChannels, int nInputChannels, ofBaseApp * appPtr, int sampleRate, int bufferSize, int nBuffers){
    ofGetSoundStream().setup(appPtr, nOutputChannels, nInputChannels, sampleRate, bufferSize, nBuffers);
    if(ofGetSystemSoundMixer().getNumChannels() == 0){
        ofGetSoundStream().setOutput(&ofGetSystemSoundMixer());
    }
}
