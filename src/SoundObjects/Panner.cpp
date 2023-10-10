//
//  Panner.cpp
//  ofxHapPlayerMultiAudio
//
//  Created by Shadow Tuner on 10-10-23.
//

#include "Panner.h"
#include "ofSoundUtils.h"

Panner::Panner():
ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
_pan(0),
leftPan(1), rightPan(1)
{
    setPan(0);
}

Panner::Panner(const Panner& a){
    this->_pan = a.getPan();
}

Panner& Panner::operator=(const Panner& mom){
    this->_pan = mom.getPan();
    return *this;
}

void Panner::setPan(float pan){
    float left, right;
    ofStereoVolumes(1, pan, left, right);
    _pan = pan;
    leftPan = left;
    rightPan = right;
}

float Panner::getPan() const{
    return _pan;
}


void Panner::process(ofSoundBuffer &input, ofSoundBuffer &output){
    
    output = input;
    if(isBypassed())return;
    
    if(output.getNumChannels() == 2){
        output.stereoPan(leftPan, rightPan);
    }else{
        if(bShowNotice){
            ofLogNotice("Panner::process") << "Not applying pan to buffer as it only works on stereo signals";
            bShowNotice = false;
        }
    }
}
