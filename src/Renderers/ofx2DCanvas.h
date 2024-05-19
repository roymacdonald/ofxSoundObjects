//
//  2DCanvas.hpp
//  example-audio-receiver
//
//  Created by Roy Macdonald on 9/19/19.
//
//

#pragma once
#include "ofMain.h"

class ofx2DCanvas;

class ofx2DCanvasTransformSync{
public:
    ofx2DCanvasTransformSync(ofx2DCanvas* _owner, ofx2DCanvas* _other );
    
    void onTransformUpdate();

    
    
    ofEventListeners listeners;
    
    ofx2DCanvas* owner = nullptr;
    ofx2DCanvas* other = nullptr;
    
};


class ofx2DCanvas{
public:

    ofx2DCanvas();
    virtual ~ofx2DCanvas(){}
    
	void begin(const ofRectangle& viewport);
	void end(bool bDrawInfo = false);
	
	void enableMouse();
	void disableMouse();
	void reset();
    
    ofEvent<void> onTransformBegin, onTransformEnd, onTransformUpdate;
    
    ofCamera& getCamera() {return cam;}
    
    const ofRectangle& getViewport(){return viewport;}
    
    void setCamNeedsUpdate();
    
    size_t getId(){return uniqueId;}
    
private:
    
    size_t uniqueId;
    
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
    
    
    void updateCameraPos();
    bool bCamPosNeedsUpdate = true;
    
    
};
