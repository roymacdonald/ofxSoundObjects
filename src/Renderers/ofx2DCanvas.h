//
//  2DCanvas.hpp
//  example-audio-receiver
//
//  Created by Roy Macdonald on 9/19/19.
//
//

#pragma once
#include "ofMain.h"


class ofx2DCanvas{
public:

    ofx2DCanvas();
    virtual ~ofx2DCanvas(){}
    
	void begin(const ofRectangle& viewport);
	void end(bool bDrawInfo = false);
	
	void enableMouse();
	void disableMouse();
	void reset();
    
    ofEvent<void> onTransformBegin, onTransformEnd;
    
    ofCamera& getCamera() {return cam;}
    
    const ofRectangle& getViewport(){return viewport;}
    
    
private:
    
    ofCamera cam;
    ofRectangle viewport;
    ofRectangle canvasConstraint;
    
    string getDebugInfo();
	
    
    void applyConstraints();
	
	glm::vec2 pressPos = {0,0};
    
	glm::vec2 prevMouse, mouseVel, currentMouse;
	
	
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
    
    
    void initCameraPos();
    bool bCamPosNeedsIniting = true;
    
    
};
