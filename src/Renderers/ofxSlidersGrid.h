//
//  ofxSlidersGrid.h
//  example-matrixMixer_tbb
//
//  Created by Roy Macdonald on 8/17/19.
//
//

#pragma once
#include "ofMain.h"

class ofxSlidersGrid{
public:
	virtual void draw(const ofRectangle& _gridRect);
	
	void enable();
	void disable();
	void toggle();
	bool isEnabled();
	
	
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	
	class GridParameter{
	public:
		GridParameter(){}
		
		void setup(const size_t& _x, const size_t& _y, ofParameter<float>& p, ofxSlidersGrid* grid);
		ofParameter<float> param;
		size_t x, y;
	private:
		ofEventListener listener;
		void notify(float &);
		ofxSlidersGrid * grid = nullptr;
	};
	
	std::vector<std::vector<std::unique_ptr<GridParameter>>> parameters;//[y][x];
	
	void setGridSize(size_t x, size_t y);
	void linkParameter(size_t x, size_t y, ofParameter<float> & param);

protected:
	void updateParameter(GridParameter & p);
	
	void updateSlider(size_t x, size_t y, float val);
	
	void checkOverGrid( const glm::vec2& p, bool bUpdateSlider, bool bPressed = false);
	
	bool bOverGrid = false;
	ofRectangle overGridRect;
	//	size_t overXInd = 0;
	//	size_t overYInd = 0;
	size_t pressedXInd = 0;
	size_t pressedYInd = 0;
	
	ofRectangle gridRect;
	
	
	ofVboMesh lineGridMesh, slidersGridMesh;
	void buildMeshes();
private:
	size_t xSize = 0, ySize =0;
	
	
	ofEventListeners mouseListeners;
	
	bool bEnabled = false;
	
};

