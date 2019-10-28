
#pragma once
#include "ofMain.h"
#include "ofxSoundObjects.h"

class SineWaveGenerator: public ofxSoundObject {
public:
	
	SineWaveGenerator():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){
		setName ("Sine Wave Gen");
		freq = 440;
		amplitude = 1;
		currentPhase = 0;
	}
    void setup(float freq, float amplitude = 1.0, float phase = 0){
        this->freq = freq;
        this->amplitude = amplitude;
        currentPhase = phase;
		
    }
	
	void process(ofSoundBuffer &in, ofSoundBuffer &out) override{
        float m = TWO_PI* freq / in.getSampleRate();

        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
		float a = amplitude;
		float s;
		float p = currentPhase;
        for(int i = 0; i < numFrames; i++) {
			s = sin(m*i + p) * a;
            for(int c = 0; c < numChannels; c++){
                out[i*numChannels + c ] = s;
            }
        }
        currentPhase = fmod(currentPhase + (m*numFrames), TWO_PI);
    }

	// these need to be atomic so there are no data races between threads
	std::atomic<float> freq;
    std::atomic<float> amplitude;
	std::atomic<float> currentPhase;
private:
};

