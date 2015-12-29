#include "FftObject.h"
FftObject::FftObject():ofxSoundObject(), fft(nullptr), bIsProcessed(false), numChannels(0){}
FftObject::~FftObject(){
    if (fft != nullptr) {
        delete fft;
        fft = nullptr;
    }
}
//--------------------------------------------------------------
void FftObject::setup(unsigned int bufferSize) {
    this->bufferSize = bufferSize;
    
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
    
    numChannels = 0;
    bIsProcessed = false;
    setBins(2);
}
//--------------------------------------------------------------

void FftObject::setBins(int numChans){
    if (numChans < numChannels) {
        drawBins.resize(numChans);
        middleBins.resize(numChans);
        audioBins.resize(numChans);
    }else if (numChans > numChannels){
        for (int i = numChannels; i < numChans; i++) {
            vector<float> db, mb, ab;
            db.resize(fft->getBinSize());
            mb.resize(fft->getBinSize());
            ab.resize(fft->getBinSize());
            drawBins.push_back(db);
            middleBins.push_back(mb);
            audioBins.push_back(ab);
        }
    }
    numChannels = numChans;
}
//--------------------------------------------------------------
void FftObject::draw(const ofRectangle & r, bool bDrawLogScale) {
    if (bIsProcessed) {
        
        soundMutex.lock();
        drawBins = middleBins;
        soundMutex.unlock();
        
        ofSetColor(255);
        ofRectangle rs;
        float h = r.height/numChannels;
        
        for (int i =0; i < numChannels; i++) {
            rs.set(0,h*i+r.y,r.width, h);
            rs.scaleFromCenter(0.95);
            plot(drawBins[i],rs, bDrawLogScale);
        }
    }
}
//--------------------------------------------------------------
void FftObject::plot(vector<float>& buffer, const ofRectangle &r, bool bDrawLogScale) {
    ofMesh m;
    m.clear();
    m.setMode(OF_PRIMITIVE_LINE_STRIP);

    int n = buffer.size();
    float offset = r.getMaxY();
    float xinc = r.width/n;
    for (int i = 0; i < n; i++) {
        if (bDrawLogScale) {
            ofVec3f v;
            v.x = ofMap(log10(i+1), 0, log10(n), r.x, r.getMaxX());
            v.y = ofMap(log10(buffer[i]+1), 0, log10(2), offset, r.y);
            m.addVertex(v);
            cout << v;
        }else{
            m.addVertex(ofVec3f(i*xinc+r.x, offset - buffer[i]*r.height ));
        }
        cout << endl;
        m.addColor(ofFloatColor::red);
    }
    ofPushStyle();
    m.draw();
    ofNoFill();
    ofDrawRectangle(r);
    ofPopStyle();
}
//--------------------------------------------------------------
void FftObject::process(ofSoundBuffer &input, ofSoundBuffer &output) {
    
    input.copyTo(output);//this sound object does not process sound so it will passthrough.
    if (numChannels != input.getNumChannels() ) {
        cout << "numChannels != input.getNumChannels()" << numChannels << " " << input.getNumChannels() << endl;
        setBins( input.getNumChannels());
    }
    
    if (input.getNumFrames() == bufferSize) {
        if (input.getNumChannels() == 1) {
            fft->setSignal(input.getBuffer());
        }else{
            for (int i =0; i < numChannels; i++) {
                ofSoundBuffer b;
                input.getChannel(b, i);
                fft->setSignal(b.getBuffer());
                float* curFft = fft->getAmplitude();
                memcpy(&audioBins[i][0], curFft, sizeof(float) * fft->getBinSize());
            }
        }
    }else{
        cout << "input.getNumFrames() != bufferSize" << endl;
    }
    
    soundMutex.lock();
    middleBins = audioBins;
    soundMutex.unlock();
    bIsProcessed = true;
}
