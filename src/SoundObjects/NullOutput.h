//
//  NullObject.hpp
//
//  Created by smeyfroi on 2025-04-20.
//
#pragma once

#include "ofxSoundObject.h"

// An output sink that discards incoming audio when a SoundObject stream
// is being used live for audio analysis and output is not needed.
//
//   ...
//   soundStream.setup(settings);
//   soundStream.setInput(deviceInput);
//   soundStream.setOutput(nullOutput);
//
//   deviceInput.connectTo(*this).connectTo(nullOutput);
//
class NullOutput: public ofxSoundOutput {
  void process(ofSoundBuffer &input, ofSoundBuffer &output) override {};
};
