//
//  2DCanvas.cpp
//  example-audio-receiver
//
//  Created by Roy Macdonald on 9/19/19.
//
//

#include "ofx2DCanvas.h"
#include "glm/mat4x4.hpp"


ofx2DCanvas::ofx2DCanvas(){
    cam.enableOrtho();
    cam.setPosition(0, 0, -100);
    cam.setNearClip(-1000000);
    cam.setFarClip(1000000);
    cam.setVFlip(true);
    applyConstraints();
}
//--------------------------------------------------------------
void ofx2DCanvas::setCamNeedsUpdate(){
    bCamPosNeedsUpdate = true;
}
//--------------------------------------------------------------
void ofx2DCanvas::begin(const ofRectangle& viewport){
    this->viewport = viewport;
    if(bCamPosNeedsUpdate){
        bCamPosNeedsUpdate = false;
        updateCameraPos();
        applyConstraints();
    }
    cam.begin(viewport);
}
string ofx2DCanvas::getDebugInfo(){
    stringstream ss;
    
    ss << "prevPos: " << prevMouse << "\n";
    ss << "currentMouse: " << currentMouse << "\n";
    ss << "mouseVel: " << mouseVel << "\n";
    ss << "pressPos" << pressPos << "\n";
    ss << "cam pos: " << cam.getPosition() << "\n";
    ss << "cam scale: " << cam.getScale() << "\n";
    ss << "bTranslate: " << boolalpha << bTranslate << "\n";
    ss << "viewport: " << viewport << "\n";
    ss << "canvasConstraint: " << canvasConstraint << "\n";
    
    
    return ss.str();
}
//--------------------------------------------------------------
void ofx2DCanvas::end(bool bDrawInfo){
    cam.end();
    if(bDrawInfo)
        ofDrawBitmapStringHighlight(getDebugInfo(), 20,20);
}
//--------------------------------------------------------------
void ofx2DCanvas::enableMouse(){
	mouseListeners.push(ofEvents().mousePressed.newListener(this, &ofx2DCanvas::mousePressed));
	mouseListeners.push(ofEvents().mouseReleased.newListener(this, &ofx2DCanvas::mouseReleased));
	mouseListeners.push(ofEvents().mouseDragged.newListener(this, &ofx2DCanvas::mouseDragged));
	mouseListeners.push(ofEvents().mouseScrolled.newListener(this, &ofx2DCanvas::mouseScrolled));
}
//--------------------------------------------------------------
void ofx2DCanvas::disableMouse(){
	mouseListeners.unsubscribeAll();
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseScrolled(ofMouseEventArgs & m){
	if(!bScrolling){
		mouseActionSetup(m, true);
	}else{
		updateMouse(m, true);
	}
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseActionSetup(ofMouseEventArgs & m, bool bMouseScroll){
	bScrolling = bMouseScroll;
	if(bScrolling){
		m.x = ofGetMouseX();
		m.y = ofGetMouseY();
	}
	if(viewport.inside(m)){
		
		if(!bScrolling){
			auto curTap = ofGetElapsedTimeMillis();
			if(lastTap != 0 && curTap - lastTap < 300){
                reset();
                return;
			}
			lastTap = curTap;
			
			bIsDragging = true;
			bTranslate	= (m.button == OF_MOUSE_BUTTON_LEFT);
		}else{
			
			bIsDragging = false;

		}
		
        pressPos = m;
        prevMouse = m;

		
		bTransformAxisSet = !(ofGetKeyPressed(OF_KEY_SHIFT));
		transformAxis = TRANSFORM_BOTH;
        
        ofNotifyEvent(onTransformBegin, this);
        
	}else{
		bScrolling = false;
	}
}
//--------------------------------------------------------------
void ofx2DCanvas::mousePressed(ofMouseEventArgs & m){
	mouseActionSetup(m, false);
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseReleased(ofMouseEventArgs & m){
	updateMouse(m, false);
	bIsDragging = false;
    ofNotifyEvent(onTransformEnd, this);
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseDragged(ofMouseEventArgs & m){
	updateMouse(m, false);
}
//--------------------------------------------------------------
void ofx2DCanvas::updateMouse(ofMouseEventArgs& m, bool bMouseScroll){
	if(!bMouseScroll) bScrolling = false;
	
	if(bIsDragging || bScrolling){
		
		glm::vec2  d;
		if(bScrolling){
			mouseVel = {m.scrollX, m.scrollY};
			pressPos += mouseVel;
			d = pressPos;
		}else{
            
            currentMouse = cam.screenToWorld({m.x,m.y,0}, viewport);
            mouseVel =  currentMouse - cam.screenToWorld({prevMouse.x,prevMouse.y,0}, viewport);   ;
            
            
            
			d = m - pressPos;
		}
		
        
        
		
		if(!bTransformAxisSet){
			auto l2 = glm::length2(d);
			if(l2 > 3){
				bTransformAxisSet =true;
				transformAxis = ((fabs(d.x) > fabs(d.y))?TRANSFORM_X:TRANSFORM_Y);
			}
		}
        
        if(transformAxis != TRANSFORM_BOTH){
            if(transformAxis == TRANSFORM_X){
                mouseVel.y = 0;
            }else if(transformAxis == TRANSFORM_Y){
                mouseVel.x = 0;
            }
        }
		
        
        glm::vec3 mousePre ;
        glm::vec3 newScale;
        if (!bTranslate) {
            mousePre = cam.screenToWorld(glm::vec3(pressPos,0));
            mouseVel = (bAltPressed?0.1:1.0) * mouseVel /glm::vec2(viewport.width,viewport.height);
            mouseVel *= -1.0f;
            newScale = cam.getScale() + glm::vec3(mouseVel, 0);
            if(newScale.x > 1.0f){
                newScale.x = 1;
            }
            if(newScale.y > 1.0f){
                newScale.y = 1;
            }
        }
        
    
        cam.move({- mouseVel.x, - mouseVel.y, 0});
            
            
        if(!bTranslate)
        {
            cam.setScale(newScale);
                // this move call is to keep the scaling centered below the mouse.
            cam.move(mousePre - cam.screenToWorld(glm::vec3(pressPos,0)));
        }
        applyConstraints();

		prevMouse = m;
	}
}
//--------------------------------------------------------------
void ofx2DCanvas::applyConstraints()
{
    
    canvasConstraint = ofRectangle(cam.screenToWorld(viewport.getMin(), viewport),  cam.screenToWorld(viewport.getMax(), viewport));
    
    ofRectangle canvasRect;
    
//    canvasRect.setFromCenter(0, 0, viewport.width, viewport.height);
    canvasRect.set(0, 0, viewport.width, viewport.height);
    glm::vec3 canvasAdjust = {0,0,0};
    bool bAdjustCanvas = false;
    if(canvasRect.x > canvasConstraint.x){
        canvasAdjust.x =  canvasRect.x - canvasConstraint.x;
        bAdjustCanvas = true;
    }
    if(canvasRect.getMaxX() < canvasConstraint.getMaxX()){
        canvasAdjust.x = canvasRect.getMaxX() - canvasConstraint.getMaxX() ;
        bAdjustCanvas = true;
    }
    
    if(canvasRect.y > canvasConstraint.y){
        canvasAdjust.y =  canvasRect.y - canvasConstraint.y ;
        bAdjustCanvas = true;
    }
    if(canvasRect.getMaxY() < canvasConstraint.getMaxY()){
        canvasAdjust.y =  canvasRect.getMaxY() - canvasConstraint.getMaxY();
        bAdjustCanvas = true;
    }
    

    if(bAdjustCanvas){
        cam.move(canvasAdjust);
    }
}


//--------------------------------------------------------------
void ofx2DCanvas::keyPressed(ofKeyEventArgs& key){
	bAltPressed = key.hasModifier(OF_KEY_ALT);
}
//--------------------------------------------------------------
void ofx2DCanvas::keyReleased(ofKeyEventArgs& key){
	if(bAltPressed && key.hasModifier(OF_KEY_ALT)){
		bAltPressed	= false;
	}
}
//--------------------------------------------------------------
void ofx2DCanvas::reset(){
    cam.setScale({1,1,1});
    updateCameraPos();
	bIsDragging = false;
    ofNotifyEvent(onTransformEnd, this);
}
//--------------------------------------------------------------
void ofx2DCanvas::updateCameraPos(){
    //    cam.setPosition(0, 0, -100);
    auto c = viewport.getCenter();
    cam.setPosition(c.x, c.y, -100);
}

