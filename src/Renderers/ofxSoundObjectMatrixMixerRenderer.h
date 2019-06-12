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
	virtual void draw(const ofRectangle& mixerRect) override;
	void enableSliders();
	void disableSliders();
	void toggleSliders();
	bool isSlidersEnabled();
	
	void setNonSliderMode(bool b);
	bool isNonSliderMode();
	
	void setMinChannelHeight(const float& minHeight);
	
	ofRectangle& 		getLeftRect(){return leftR;}
	const ofRectangle&  getLeftRect() const {return leftR;}
	ofRectangle& 		getBottomRect(){return bottomR;}
	const ofRectangle&  getBottomRect() const {return bottomR;}
	ofRectangle& 		getGridRect(){return gridR;}
	const ofRectangle&  getGridRect() const {return gridR;}
	ofRectangle& 		getBottomLeftRect(){return bottomLeftR;}
	const ofRectangle&  getBottomLeftRect() const {return bottomLeftR;}

	

protected:

	ofRectangle leftR;
	ofRectangle bottomR;
	ofRectangle gridR;
	ofRectangle bottomLeftR;

	float leftW = 200;
	float bottomH = 50;
	float chanW = 10;

private:
	std::vector<std::vector<std::vector< std::unique_ptr<ofxFloatSlider>>>> sliders;
	std::vector<std::unique_ptr<ofxFloatSlider>> outputSliders;

	ofxFloatSlider masterSlider;	
	void initOrResizeNumSliders(const float & sliderWidth = 200);
	
	
	bool bSlidersEnabled = false;
	bool bSlidersWereEnabled = false;
	bool bMasterSliderSetup = false;
	
	bool bNonSliderMode = false;
	
	float minHeight = 1;
};

