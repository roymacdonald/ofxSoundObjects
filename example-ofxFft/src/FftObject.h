#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxSoundObjects.h"


//#define FFT_IN_DRAW
//#define USE_SINGLE_CHANNEL
class FftObject : public ofxSoundObject {
public:
    FftObject();
    ~FftObject();
	void setup(unsigned int bufferSize = 2048);
	void plot(vector<float>& buffer, const ofRectangle &r);
    void process(ofSoundBuffer &input, ofSoundBuffer &output);
    void draw(const ofRectangle & r);

    unsigned int bufferSize;

    void setBins(int numChans);
    
	ofxFft* fft;
	
	ofMutex soundMutex;
#ifdef USE_SINGLE_CHANNEL
    vector<float> drawBins, middleBins, audioBins;
#else
	vector<vector<float> >drawBins, middleBins, audioBins;
#endif
    int numChannels;
    bool bIsProcessed;
    ofSoundBuffer buf;
    int numProceesed, numReceived, buffersPerDraw;
};
