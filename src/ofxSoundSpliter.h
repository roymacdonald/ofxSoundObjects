//
//  ofxSoundObjectSplitter.hpp
//  CircularBufferMultiPlay
//
//  Created by Roy Macdonald on 06-05-24.
//

#pragma once

#include "ofMain.h"
#include "ofxSoundObject.h"
/**
 * This class represents a simple splitter which connects and sends the same audio buffer to many different objects connected to it.
 * It is the opposite to an ofSoundMixer
 */
class ofxSoundSpliter: public ofxSoundObject {
public:
    ofxSoundSpliter();
    virtual ~ofxSoundSpliter();

    /// returns the connected object at the specified index
    ofxSoundObject* getConnectionSource(size_t connectionIndex);

    /// get the number of connected objects.
    size_t getNumConnections();

    void audioOut(ofSoundBuffer &output) override;
    bool isConnectedTo(ofxSoundObject& obj);
    
    ///\ brief Gets the connection index for the passed object, if it is connected.
    ///\param obj the object for which you want to find the connection channel
    ///\param index the index, if found will be passed back in this parameter, as it is a reference.
    ///\returns true if there is a connection for the passed object, false otherwise.
    bool getObjectConnectionIndex(ofxSoundObject& obj, size_t& index);

    virtual ofxSoundObject &connectTo(ofxSoundObject &soundObject) override;
    virtual void disconnect() override;
    bool disconnectOutput(ofxSoundObject &soundObject);
    
protected:

    /// This is the method you implement to process the signal from inputs to outputs.
    

    /// this pulls the audio through from earlier links in the chain.
    /// you can override this to add more interesting functionality
    /// like signal splitters, sidechains etc.
    
    vector<ofxSoundObject*> connections;
    std::mutex connectionMutex;
    
};

