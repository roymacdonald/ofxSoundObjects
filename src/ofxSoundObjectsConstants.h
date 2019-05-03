//
//  ofxSoundObjectsConstants.h
//
//  Created by Roy Macdonald on 5/2/19.
//
//

#pragma once

/// \brief enum that defines the way in which audio is processed through 
/// 

enum ofxSoundObjectsMode{
	//This is the default mode 
	OFX_SOUND_OBJECT_PULL = 0,
	OFX_SOUND_OBJECT_PUSH,
	OFX_SOUND_OBJECT_INDEPENDENT,
	OFX_SOUND_OBJECT_OFFLINE
};

enum ofxSoundObjectsType{
	OFX_SOUND_OBJECT_SOURCE =0,
	OFX_SOUND_OBJECT_PROCESSOR,
	OFX_SOUND_OBJECT_DESTINATION
};
