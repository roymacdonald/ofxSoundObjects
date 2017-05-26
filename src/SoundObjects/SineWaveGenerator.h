
#pragma once
#include "ofMain.h"
#include "ofxSoundObjects.h"

class SineWaveGenerator: public ofxSoundObject {
public:
    void setup(float freq, float amplitude = 1.0, float phase = 0, int sampleRate = 44100){
        this->freq = freq;
        this->amplitude = amplitude;
        this->phase = phase;
		this->sampleRate = sampleRate;
        currentPhase = 0;
		phaseAdder = 0;
    }
    
    void process(ofSoundBuffer &in, ofSoundBuffer &out) {

		while (phase > TWO_PI) {
			phase -= TWO_PI;
		}
		float phaseAdderTarget = (freq / (float)sampleRate) * TWO_PI;
		
			phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
			for (int i = 0; i < out.getNumFrames(); i++) {
				phase += phaseAdder;
				float sample = sin(phase);
				out[i*out.getNumChannels()] = sample * amplitude;
			out[i*out.getNumChannels() + 1] = sample * amplitude;
		}
		


//        float m = TWO_PI* freq / in.getSampleRate();
////        frameIndex*myBuffer.getNumChannels()) + channelIndex
//        int numFrames = out.getNumFrames();
//        int numChannels = out.getNumChannels();
//        for(int i = 0; i < numFrames; i++) {
//            float s = sin(m*i + phase+currentPhase) * amplitude;
//            for(int c = 0; c < numChannels; c++){
//                out[i*numChannels + c ] = s;
//            }
//        }
//        currentPhase = m*numFrames;
    }
    

    float freq;
    float amplitude;
    float phase, phaseAdder;
	int sampleRate;
private:
    float currentPhase;
	
};