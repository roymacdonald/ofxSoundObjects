
#pragma once
#include "ofMain.h"
#include "ofxSoundObjects.h"

class SineWaveGenerator: public ofxSoundObject {
public:
	
	SineWaveGenerator():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
    void setup(float freq, float amplitude = 1.0, float phase = 0){
        this->freq = freq;
        this->amplitude = amplitude;
        currentPhase = phase;
    }
    virtual  std::string getName() override{ return "Sine Wave Gen";}
	
	void process(ofSoundBuffer &in, ofSoundBuffer &out) override{
        float m = TWO_PI* freq / in.getSampleRate();

        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
		float a = amplitude;
		float s;
        for(int i = 0; i < numFrames; i++) {
			s = sin(m*i + currentPhase) * a;
            for(int c = 0; c < numChannels; c++){
                out[i*numChannels + c ] = s;
            }
        }
        currentPhase += (m*numFrames);
    }

	// these need to be atomic so there are no data races between threads
	std::atomic<float> freq;
    std::atomic<float> amplitude;
private:
    float currentPhase = 0;
};

