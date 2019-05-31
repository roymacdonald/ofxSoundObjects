//
//  ofxSoundObjectsConstants.h
//
//  Created by Roy Macdonald on 5/2/19.
//
//

#pragma once


//Uncomment the following line to enable multithreading. You will need the intel Threading Building Blocks for this. Instructions in the readme file
//#define OFX_SOUND_ENABLE_MULTITHREADING

/// \brief enum that defines the way in which audio is processed through 
/// 

#define OFX_SOUND_ENABLE_THREADED_RECORDER

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


enum ofxSoundChannelVisibility{
	VISIBLE = 0, // it should render the "normal view"
	MINIMIZED,//it should render the minimized view bwhich should use minimal space, less than normal view but never invisible
	HIDDEN,//It does not get rendered
	NUM_VISIBILITY_OPTS//
};
