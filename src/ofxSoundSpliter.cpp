//
//  ofxSoundObjectSplitter.cpp
//  CircularBufferMultiPlay
//
//  Created by Roy Macdonald on 06-05-24.
//

#include "ofxSoundSpliter.h"
#include "ofxSoundUtils.h"

ofxSoundSpliter::ofxSoundSpliter():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
    setName("Splitter");
}

//------------------------------------------------------------------------------------------
ofxSoundSpliter::~ofxSoundSpliter(){
    disconnect();
}

//------------------------------------------------------------------------------------------
ofxSoundObject* ofxSoundSpliter::getConnectionSource(size_t connectionIndex){
    std::lock_guard<std::mutex> lck(connectionMutex);
    if (connectionIndex >= connections.size()){
        return nullptr;
    }
    return connections[connectionIndex];
}

//------------------------------------------------------------------------------------------
size_t ofxSoundSpliter::getNumConnections(){
    std::lock_guard<std::mutex> lck(connectionMutex);
    return connections.size();
}

//------------------------------------------------------------------------------------------
void ofxSoundSpliter::audioOut(ofSoundBuffer &output) {
    if(_tickCount < output.getTickCount()){
        _tickCount = output.getTickCount();
        if(outputNumChannels != output.getNumChannels()){
            
            ofLogNotice("ofxSoundSpliter::audioOut") << "output num channels changed from " << outputNumChannels << " to " << output.getNumChannels();
            outputNumChannels = output.getNumChannels();
        }
        ofxSoundUtils::checkBuffers(output, workingBuffer);
        if(inputObject!=nullptr) {
            if(isBypassed()){
                inputObject->audioOut(output);
            }else{
                inputObject->audioOut(workingBuffer);
            }
        }
    }
//    printAudioOut();
    if(!isBypassed()){
        this->process(workingBuffer, output);
    }
}
//------------------------------------------------------------------------------------------
bool ofxSoundSpliter::isConnectedTo(ofxSoundObject& obj){
    std::lock_guard<std::mutex> lck(connectionMutex);
    for(auto c: connections){
        if(c == &obj){
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------
bool ofxSoundSpliter::getObjectConnectionIndex(ofxSoundObject& obj, size_t& index){
    
    std::lock_guard<std::mutex> lck(connectionMutex);
    for(size_t i = 0 ; i < connections.size(); i++){
        if(connections[i] == &obj){
            index = i;
            return true;
        }
    }
    return false;
    
}

//------------------------------------------------------------------------------------------
bool ofxSoundSpliter::disconnectOutput(ofxSoundObject &soundObject){
    if(isConnectedTo(soundObject)){
        std::lock_guard<std::mutex> lck(connectionMutex);
//        ofRemove(connections, [&](ofxSoundObject * o){ return o == &soundObject; });
        
        for (size_t i =0; i<connections.size(); i++) {
            if (&soundObject == connections[i]) {
                connections.erase(connections.begin() + i);
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------
ofxSoundObject & ofxSoundSpliter::connectTo(ofxSoundObject &soundObject) {
    if(isConnectedTo(soundObject)){
        ofLogWarning("ofxSoundSpliter::connectTo") << "Already connected to " << soundObject.getName();
        return soundObject;
    }
//    outputObjectRef = &soundObject;
    soundObject.setInput(this);

    {
        std::lock_guard<std::mutex> lck(connectionMutex);
        connections.push_back(&soundObject);
    }
    // if we find an infinite loop, we want to disconnect and provide an error
    if(!checkForInfiniteLoops()) {
        ofLogError("ofxSoundObject") << "There's an infinite loop in your chain of ofxSoundObjects";
//        disconnect();
//        std::lock_guard<std::mutex> lck(connectionMutex);
//        ofRemove(connections, [&](ofxSoundObject * o){ o == &soundObject; });
        disconnectOutput(soundObject);
    }
    return soundObject;
}

//------------------------------------------------------------------------------------------
void ofxSoundSpliter::disconnect() {
    std::lock_guard<std::mutex> lck(connectionMutex);
    for(auto c: connections){
        if(c){
            c->disconnectInput(this);
            c = nullptr;
        }
    }
    connections.clear();
    
}
