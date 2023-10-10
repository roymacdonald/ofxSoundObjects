//
//  Panner.hpp
//  ofxHapPlayerMultiAudio
//
//  Created by Shadow Tuner on 10-10-23.
//
#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"

#include <atomic>


///\brief This object sets the pan for a mono or stereo buffer passing through it
class Panner:  public ofxSoundObject{
public:
    
    Panner();
    Panner(const Panner& a);
    Panner& operator=(const Panner& mom);
    
    ///\brief set the pan applied to the signal
    ///\param pan the pan value. range is -1 to 1. (left - right)
    void setPan(float pan);
    
    ///\brief get the pan applied to the signal
    ///\returns pan in the range -1 to 1. (left - right)
    float getPan() const;
    
    
    virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
private:
    
    std::atomic<float> _pan;
    std::atomic<float> leftPan, rightPan;
    
    bool bShowNotice = true;
    
};
