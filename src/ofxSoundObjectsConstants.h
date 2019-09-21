//
//  ofxSoundObjectsConstants.h
//
//  Created by Roy Macdonald on 5/2/19.
//
//

#pragma once



/// define for enabling multi threading for processing faster and more efficiently large signal chains.
/// Currently only used on the matrix mixer object.
/// Disabled by default.
/// You will need the intel Threading Building Blocks for this. Instructions in the readme file
/// Uncomment the following line to enable multithreading.
//#define OFX_SOUND_ENABLE_MULTITHREADING

/// define for enabling the ofxSoundRecorderObject to write files on a different thread.
/// This is enabled by default but you might want to disable it for some reason. If you do, just comment the line below.

#define OFX_SOUND_ENABLE_THREADED_RECORDER


/// \brief enum that defines the way in which audio is processed through
/// Although this is already implemented it is mostly thought to be useful on an upcoming version of ofxSoundObjects
enum ofxSoundObjectsType{
	OFX_SOUND_OBJECT_SOURCE =0,// it generates sound somehow, thus it has nothing connected to it.
	OFX_SOUND_OBJECT_PROCESSOR,// the signal gets manipulated and has another object connected to it as well as it being conected to another one.
	OFX_SOUND_OBJECT_DESTINATION// it receives sound only, so it is not conected to any other object.
	
};

/// \brief enum that defines the way in which an ofxSoundObject will modify its incomming signals.
/// Still not implemented as it is to be useful on an upcoming version of ofxSoundObjects

enum ofxSoundObjectsChannelCountModifier{
	OFX_SOUND_OBJECT_CHAN_UNCHAGED = 0, 
	OFX_SOUND_OBJECT_CHAN_MUX,// 
	OFX_SOUND_OBJECT_CHAN_DEMUX,
	OFX_SOUND_OBJECT_CHAN_MIXER,
	OFX_SOUND_OBJECT_CHAN_SPLITTER
};

/// \bried enum that defines the visibility of a certain channel when rendering a matrix mixer.
enum ofxSoundChannelVisibility{
	VISIBLE = 0, // it should render the "normal view"
	MINIMIZED,//it should render the minimized view bwhich should use minimal space, less than normal view but never invisible
	HIDDEN,//It does not get rendered
	NUM_VISIBILITY_OPTS//
};
