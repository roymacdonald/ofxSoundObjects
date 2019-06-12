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


enum ofxSoundObjectsType{
	OFX_SOUND_OBJECT_SOURCE =0,// it generates sound somehow, thus it has nothing connected to it.
	OFX_SOUND_OBJECT_PROCESSOR,// the signal gets manipulated and has another object connected to it as well as it being conected to another one.
	OFX_SOUND_OBJECT_DESTINATION// it receives sound only, so it is not conected to any other object.
	
};
enum ofxSoundObjectsChannelCountModifier{
	OFX_SOUND_OBJECT_CHAN_UNCHAGED = 0, 
	OFX_SOUND_OBJECT_CHAN_MUX,// 
	OFX_SOUND_OBJECT_CHAN_DEMUX,
	OFX_SOUND_OBJECT_CHAN_MIXER,
	OFX_SOUND_OBJECT_CHAN_SPLITTER
};
enum ofxSoundChannelVisibility{
	VISIBLE = 0, // it should render the "normal view"
	MINIMIZED,//it should render the minimized view bwhich should use minimal space, less than normal view but never invisible
	HIDDEN,//It does not get rendered
	NUM_VISIBILITY_OPTS//
};
