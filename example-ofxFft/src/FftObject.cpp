#include "FftObject.h"
FftObject::FftObject():ofxSoundObject(), fft(nullptr), bIsProcessed(false), numChannels(0){}
FftObject::~FftObject(){
    if (fft != nullptr) {
        delete fft;
        fft = nullptr;
    }
    numProceesed = numReceived = buffersPerDraw =0;
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
#ifdef USE_SINGLE_CHANNEL
    drawBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());
    audioBins.resize(fft->getBinSize());
#else
    if (numChans < numChannels) {
        
        drawBins.resize(numChans);
        middleBins.resize(numChans);
        audioBins.resize(numChans);
    }else{
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
#endif
}
//--------------------------------------------------------------
void FftObject::draw(const ofRectangle & r) {
    if (bIsProcessed) {

#ifdef FFT_IN_DRAW
        if (buf.getNumFrames() >= bufferSize) {
            numProceesed ++;
            //       for (int i =0; i < numChannels; i++) {
            if (buf.getNumChannels() == 1) {
                fft->setSignal(buf.getBuffer());
            }else{
            ofSoundBuffer b;
            buf.getChannel(b, 0);
            fft->setSignal(b.getBuffer());
            }
            float* curFft = fft->getAmplitude();
            memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
            //     }
        }else{
            cout << "input.getNumFrames() < bufferSize" << endl;
            
        }
#else
        soundMutex.lock();
//        for (int i = 0; i < drawBins.size(); i++) {
//            drawBins[i] = middleBins[i];
//        }

        drawBins = middleBins;
        soundMutex.unlock();

#endif
        ofSetColor(255);
        ofDrawBitmapString("Frequency Domain", r.x, r.y + 10);
        ofRectangle rs;
#ifdef USE_SINGLE_CHANNEL
        float h = r.height;///numChannels;
        rs= r;
        rs.scaleFromCenter(0.95);
#ifdef FFT_IN_DRAW
            plot(audioBins,rs );
#else
            plot(drawBins,rs );
#endif

#else
            float h = r.height/numChannels;

        for (int i =0; i < numChannels; i++) {
        rs.set(0,h*i+r.y,r.width, h);
        rs.scaleFromCenter(0.95);
#ifdef FFT_IN_DRAW
            plot(audioBins[i],rs );
#else
            plot(drawBins[i],rs );
#endif
    }
#endif
        //}
    }
    
    string msg = ofToString((int) ofGetFrameRate()) + " fps\n";
    msg += "Num Processed: " + ofToString(numProceesed) + "\n";
    msg += "Num Received: " + ofToString(numReceived) + "\n";
    msg += "Num Channels: " + ofToString(numChannels) + "\n";
    msg += "buffersPerDraw: " + ofToString(buffersPerDraw) + "\n";
    ofDrawBitmapStringHighlight(msg, 80, 20);
    
    buffersPerDraw = 0;
}
//--------------------------------------------------------------
void FftObject::plot(vector<float>& buffer, const ofRectangle &r) {
    ofMesh m, mm;
    m.clear();
    m.setMode(OF_PRIMITIVE_LINE_STRIP);
    mm.setMode(OF_PRIMITIVE_LINE_STRIP);

    int n = buffer.size();
    float offset = r.getMaxY();
    float xinc = r.width/n;
    bool bUseLog = true;
    float lgmx;
    if (bUseLog) {
        lgmx = log(n-1);
    }else{
    
    }
    for (int i = 0; i < n; i++) {
        if (bUseLog) {
            ofVec3f v;
            v.x = ofMap(log10(i+1), 0, log10(n), r.x, r.getMaxX());
            v.y = ofMap(log10(buffer[i]), log10(0), log10(1), offset, r.y);
            m.addVertex(v);
            m.addColor(ofFloatColor::red);
            v.y = ofMap(log10(buffer[i]+1), log10(1), log10(2), offset, r.y);
            mm.addVertex(v);
            mm.addColor(ofFloatColor::white);
            
        }else{
            m.addVertex(ofVec3f(i*xinc+r.x, offset - buffer[i]*r.height ));
        }
    }
    ofPushStyle();
  //  ofSetColor(ofColor::red);
    //m.disableColors();
    m.draw();
    mm.draw();
    ofNoFill();
    ofDrawRectangle(r);
    ofPopStyle();
}
//--------------------------------------------------------------
void FftObject::process(ofSoundBuffer &input, ofSoundBuffer &output) {
#ifdef FFT_IN_DRAW
    if(input.size()!=buf.size()) {
        ofLogVerbose("ofSoundObject") << "working buffer size != output buffer size.";
        buf.resize(input.size());
        buf.setNumChannels(input.getNumChannels());
        buf.setSampleRate(input.getSampleRate());
    }
    soundMutex.lock();
    input.copyTo(buf);
    soundMutex.unlock();
#endif
    input.copyTo(output);//this sound object does not process sound so it will passthrough.
    numReceived ++;
#ifndef USE_SINGLE_CHANNEL
    if (numChannels != input.getNumChannels() ) {
            cout << "numChannels != input.getNumChannels()" << numChannels << " " << input.getNumChannels() << endl;
        setBins( input.getNumChannels());
    }
#endif
    
    if (input.getNumFrames() == bufferSize) {
        numProceesed ++;
        if (input.getNumChannels() == 1) {
            fft->setSignal(input.getBuffer());
        }else{
#ifndef USE_SINGLE_CHANNEL
        for (int i =0; i < numChannels; i++) {
            ofSoundBuffer b;
            input.getChannel(b, i);
            fft->setSignal(b.getBuffer());
            float* curFft = fft->getAmplitude();
            memcpy(&audioBins[i][0], curFft, sizeof(float) * fft->getBinSize());
        }
#else
            ofSoundBuffer b;
            input.getChannel(b, 0);
            fft->setSignal(b.getBuffer());
            float* curFft = fft->getAmplitude();
            memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
#endif
        }
    }else{
        cout << "input.getNumFrames() < bufferSize" << endl;
    }

#ifndef FFT_IN_DRAW
    soundMutex.lock();
//    for (int i = 0; i < audioBins.size(); i++) {
//        middleBins[i] = audioBins[i];
//    }
    middleBins = audioBins;
    buffersPerDraw++;
    soundMutex.unlock();
     //*/
#endif
    bIsProcessed = true;
}
