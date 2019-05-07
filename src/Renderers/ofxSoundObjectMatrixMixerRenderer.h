//
//  ofxSoundObjectMatrixMixerRenderer.h
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#pragma once
#include "ofxSoundObjectBaseRenderer.h"
#include "ofxSoundMatrixMixer.h"
#include "ofxGui.h"
class ofxSoundMatrixMixerRenderer: public ofxSoundObjectBaseRenderer<ofxSoundMatrixMixer>{
public:
	virtual void draw() override;
	void enableSliders();
	void disableSliders();
	void toggleSliders();
	bool isSlidersEnabled();
	
	void setNonSliderMode(bool b);
	bool isNonSliderMode();
	
private:
	std::vector<std::vector<std::vector< std::unique_ptr<ofxFloatSlider>>>> sliders;
	std::vector<std::unique_ptr<ofxFloatSlider>> outputSliders;

	ofxFloatSlider masterSlider;	
	void initOrResizeNumSliders(const float & sliderWidth = 200);
	
	
	bool bSlidersEnabled = false;
	bool bMasterSliderSetup = false;
	
	bool bNonSliderMode = false;
};

