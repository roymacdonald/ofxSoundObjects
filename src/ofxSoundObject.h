#pragma once

#include "ofConstants.h"
#include "ofBaseTypes.h"
#include "ofSoundBuffer.h"
#include "ofxSoundObjectsConstants.h"
#include "ofSoundStream.h"
// this #define is for use by addon writers, to conditionally support sound objects in addons
// (while allowing backwards compatibility with previous versions of openFrameworks)
#define OF_SOUND_OBJECT_AVAILABLE

//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
/**
 * ofxSoundObject is a node in your dsp chain. It can have one input,
 * and one output. If it doesn't have an input, it's the beginning
 * of the dsp chain (i.e. an oscillator or a microphone input).
 */
class ofSoundBuffer;
class ofxSoundInputMultiplexer;

class ofxSoundObject: public ofBaseSoundOutput {
public:
	ofxSoundObject();
	ofxSoundObject(ofxSoundObjectsType);
	virtual ~ofxSoundObject() {
		disconnect();
	}

	friend class ofxSoundInputMultiplexer; 
	
	
	/// Connects the output of this ofxSoundObject to the input of the parameter ofxSoundObject
	ofxSoundObject &connectTo(ofxSoundObject &soundObject);
    void disconnect();
	virtual void disconnectInput(ofxSoundObject * input);

	/// This is the method you implement to process the signal from inputs to outputs.
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) {
		// default behaviour is pass-through.
		input.copyTo(output);
	}

	/// this pulls the audio through from earlier links in the chain.
	/// you can override this to add more interesting functionality
	/// like signal splitters, sidechains etc.
	virtual void audioOut(ofSoundBuffer &output) override;

	/// this checks the dsp chain to ensure there are no infinite loops
	/// - might want to override this if you make a splitter
	/// returns true if there are no infinite loops.
	virtual bool checkForInfiniteLoops();
	ofxSoundObject *getInputObject();

	
	///this returns the object that is at the begining of the objects chain. It should be an audio input, a sound file player or some kind of signal generator, like a synth.
	ofxSoundObject * getSignalSourceObject();
	
	
	///this returns the object that is at the end of the objects chain. It should be an audio output or a sound file writer
	ofxSoundObject * getSignalDestinationObject();
	
	
    /// This sets/gets the number of channels that this sound object should process.
    /// By default it will use the number of channels from the ofSoundBuffer passed by the previous link in the chain.
    /// For a 2 channel setup there should be no need to set this, it is mainly for scenarios with more inputs or outputs.
//    virtual void setNumChannels(int num);
	virtual size_t getNumChannels();

    ofSoundBuffer& getBuffer();
    const ofSoundBuffer& getBuffer() const;
	
	
	ofxSoundObjectsType getType(){return type;}
	
	
	ofxSoundObjectsMode getMode(){return mode;}
	
	void setOutputStream(ofSoundStream& stream);
	void setOutputStream(ofSoundStream* stream);
	ofSoundStream* getOutputStream();

	/// Returns the device ID that is connected to this output
	virtual int getDeviceId();
	
	/// Returns info about the device that is connected to this output
	virtual ofSoundDevice getDeviceInfo();
	
	
protected:

	// this is the previous dsp object in the chain
	// that feeds this one with input.
	ofxSoundObject *inputObject = nullptr;
    ofxSoundObject *outputObjectRef = nullptr;
    virtual void setInput(ofxSoundObject *obj);
	
	ofxSoundObjectsMode mode = OFX_SOUND_OBJECT_PULL;
    
//    int numChannels = 0;

	ofxSoundObjectsType type = OFX_SOUND_OBJECT_PROCESSOR;
	
private:
	ofSoundStream* outputStream = nullptr;
	
	// ofxSoundObjects reference their source, not their destination
	// because it's not needed in a pullthrough audio architecture.
	// this lets that be set under the hood via connectTo()

	// a spare buffer to pass from one sound object to another
	ofSoundBuffer workingBuffer;
};

//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
/**
 * This class represents input from the sound card in your dsp chain.
 */
class ofxSoundInput: public ofBaseSoundInput, public ofxSoundObject {
public:
	ofxSoundInput();
	virtual size_t getNumChannels() override;
	// copy audio in to internal buffer
	virtual void audioIn(ofSoundBuffer &input) override;
	virtual void audioOut(ofSoundBuffer &output) override;
	
	/// Returns the device ID that is connected to this input
	virtual int getDeviceId() override;
	
	
	void setInputStream(ofSoundStream& stream);
	void setInputStream(ofSoundStream* stream);
	ofSoundStream* getInputStream();
	

protected:
	ofSoundBuffer inputBuffer;
private:
	ofSoundStream* inputStream = nullptr;
	
};

/**
 * This class represents the output in your dsp chain.
 */
class ofxSoundOutput: public ofxSoundObject {
public:
	ofxSoundOutput();
};


