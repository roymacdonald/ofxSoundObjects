//
//  2DCanvas.hpp
//  example-audio-receiver
//
//  Created by Roy Macdonald on 9/19/19.
//
//

#pragma once
#include "ofMain.h"


class ofx2DCanvas: public ofRectangle{
public:

	void begin(const ofRectangle& viewport);
	void end(bool bDrawInfo = true);
	
	void enableMouse();
	void disableMouse();
	void reset();
private:
	
	
	glm::vec2 pressPos = {0,0};
	glm::vec2 relativePressPos;
	glm::vec2 prevMouse, mouseVel;
	
	
	ofEventListeners mouseListeners;
	
	void mousePressed(ofMouseEventArgs & m);
	void mouseReleased(ofMouseEventArgs & m);
	void mouseDragged(ofMouseEventArgs & m);
	void mouseScrolled(ofMouseEventArgs & m);
	void keyPressed(ofKeyEventArgs& key);
	void keyReleased(ofKeyEventArgs& key);
	void updateMouse(ofMouseEventArgs& m, bool bMouseScroll);
	
	void mouseActionSetup(ofMouseEventArgs & m, bool bMouseScroll);
	
	
	bool bIsDragging = false;
	
	glm::vec2 scale = {1,1};
	glm::vec2 onPressScale  = {1,1};
	glm::vec2 translate = {0,0};
	glm::vec2 onPressTranslate = {0,0};
	bool bTranslate = false;
	
	enum TransformAxis {
		TRANSFORM_BOTH= 0,
		TRANSFORM_X,
		TRANSFORM_Y
	}transformAxis = TRANSFORM_BOTH;
	
	bool bTransformAxisSet = false;
	
	unsigned long lastTap = 0;
	
	bool bScrolling = false;
	bool bAltPressed = false;
};
