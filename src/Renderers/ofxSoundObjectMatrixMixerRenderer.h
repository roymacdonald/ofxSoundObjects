//
//  ofxSoundObjectMatrixMixerRenderer.h
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#pragma once
#include "ofxSoundObjectBaseRenderer.h"
#include "ofxSoundMatrixMixer.h"
//#include "ofxGui.h"
#include "ofxSlidersGrid.h"
#include "ofxSingleSoundPlayer.h"

#define DISABLE_SLIDERS
class ofxSoundMatrixMixerRenderer: public ofxSoundObjectBaseRenderer<ofxSoundMatrixMixer>{
public:
	virtual void draw(const ofRectangle& mixerRect) override;
    ///\brief enable sliders. These will get drawn and you will be able to adjust with your mouse.
    void enableSliders();
    ///\brief disable sliders. You will no longer be able to adjust these with your mouse, yet still able to draw them
	void disableSliders();
    ///\brief toggle between enabled and disabled sliders
	void toggleSliders();
	
    ///\brief get sliders enabled state
    ///\return true if enabled false otherwise.
    bool isSlidersEnabled();
	
    
    ///\brief set nonSliderMode.
    ///\param b pass true to set into non slider mode, which will disable sliders.
	
    OF_DEPRECATED_MSG("Use enableSliders() or disableSliders().", void setNonSliderMode(bool b));
    
    
    
    ///\ getNonSliderMode status
    ///\return bool, true if non slider mode enabled.
    OF_DEPRECATED_MSG("Dont use this. use isSlidersEnabled instead",
    bool isNonSliderMode());
    
    
    
    
    ///\brief enable drawing playheads when a sound player is connected.
    void enablePlayheadsDraw();
    ///\brief disable drawing playheads when a sound player is connected.
    void disablePlayheadsDraw();
    
    ///\brief get If drawing playheads is enabled.
    /// disabled by default
    ///\return true if enabled false otherwise
    bool isPlayheadsDrawEnabled();
    
	
	void setMinChannelHeight(const float& minHeight);
	
		  ofRectangle& 	getLeftRect()		{return leftR;}
	const ofRectangle&  getLeftRect() const {return leftR;}
		  ofRectangle& 	getBottomRect()		  {return bottomR;}
	const ofRectangle&  getBottomRect() const {return bottomR;}
		  ofRectangle& 	getGridRect()		{return gridR;}
	const ofRectangle&  getGridRect() const {return gridR;}
		  ofRectangle& 	getBottomLeftRect()		  {return bottomLeftR;}
	const ofRectangle&  getBottomLeftRect() const {return bottomLeftR;}

	

	void drawStatus(float x = 20, float y =20);
	
    
    
    
    
protected:
    
	ofRectangle leftR;
	ofRectangle bottomR;
	ofRectangle gridR;
	ofRectangle bottomLeftR;
	ofRectangle drawRect;

	float leftW = 200;
	float bottomH = 50;
	float chanW = 10;
	
	ofVboMesh mainMesh, lineGridMesh, playheadMesh;
	std::vector<ofVboMesh> textMeshes;
	void buildMeshes();
	void buildPlayheads();
	void buildOutputSliders();
	void buildSlidersGrid();
	ofxSlidersGrid slidersGrid;
	ofxSlidersGrid outputSliders;
	ofRectangle outputSlidersRect;
	void updatePlayheads();
	
	bool bDisableTextRendering = false;
private:
	
	std::map<size_t, ofxBaseSoundPlayer* > playheadsIndices;
	size_t numChansIn = 0, numChansOut =0;

	bool bMasterSliderSetup = false;
	
	bool bNonSliderMode = false;
	
	float minHeight = 1;
	
    bool bDrawPlayheads = false;
    
	float vuWidth = 20;
};

