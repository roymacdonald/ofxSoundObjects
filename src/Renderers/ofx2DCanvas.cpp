//
//  2DCanvas.cpp
//  example-audio-receiver
//
//  Created by Roy Macdonald on 9/19/19.
//
//

#include "ofx2DCanvas.h"
//--------------------------------------------------------------
void ofx2DCanvas::begin(const ofRectangle& viewport){
	this->set(viewport);
	ofPushView();
	ofViewport(viewport);
	ofSetupScreenOrtho();
	ofPushMatrix();
	ofTranslate( translate);
	ofScale(scale.x, scale.y, 1);

}
//--------------------------------------------------------------
void ofx2DCanvas::end(){
	ofPopMatrix();
	ofPopView();
	
}
//--------------------------------------------------------------
void ofx2DCanvas::enableMouse(){
//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	mouseListeners.push(ofEvents().mousePressed.newListener(this, &ofx2DCanvas::mousePressed));
	mouseListeners.push(ofEvents().mouseReleased.newListener(this, &ofx2DCanvas::mouseReleased));
	mouseListeners.push(ofEvents().mouseDragged.newListener(this, &ofx2DCanvas::mouseDragged));
	
}
//--------------------------------------------------------------
void ofx2DCanvas::disableMouse(){
//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	mouseListeners.unsubscribeAll();
}

//--------------------------------------------------------------
void ofx2DCanvas::mousePressed(ofMouseEventArgs & m){
	if(inside(m)){
		auto curTap = ofGetElapsedTimeMillis();
		if(lastTap != 0 && curTap - lastTap < 300){
			reset();
			return;
		}
		lastTap = curTap;
		
		
		pressPos = m;
		prevMouse = m;
		bIsDragging = true;
		onPressScale = scale;
		onPressTranslate = translate;
		bTranslate	=(m.button == OF_MOUSE_BUTTON_LEFT);
		
		
		relativePressPos = m - translate - getPosition();
		relativePressPos /= scale;
		
		bTransformAxisSet = !(ofGetKeyPressed(OF_KEY_SHIFT));
		transformAxis = TRANSFORM_BOTH;
		
	}
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseReleased(ofMouseEventArgs & m){
	updateMouse(m);
	bIsDragging = false;
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseDragged(ofMouseEventArgs & m){
	updateMouse(m);
}
//--------------------------------------------------------------
void ofx2DCanvas::updateMouse(glm::vec2& m){
	if(bIsDragging){
		
		mouseVel = m  - prevMouse;
		
		
		
		auto d = m - pressPos;
		
		if(!bTransformAxisSet){
			auto l2 = glm::length2(d);
			if(l2 > 3){
				bTransformAxisSet =true;
				transformAxis = ((fabs(d.x) > fabs(d.y))?TRANSFORM_X:TRANSFORM_Y);
			}
		}
		
		
		if(bTranslate){
			translate += mouseVel;
			
			if(transformAxis != TRANSFORM_BOTH){
				if(transformAxis == TRANSFORM_X){
					translate.y = onPressTranslate.y;
				}else if(transformAxis == TRANSFORM_Y){
					translate.x = onPressTranslate.x;
				}
			}
			
			
			
		}else{
			mouseVel = 1 * mouseVel /glm::vec2(width,height);
			scale += mouseVel + mouseVel * scale;
			translate = onPressTranslate - relativePressPos*(scale - onPressScale);
			
			if(transformAxis != TRANSFORM_BOTH){
				if(transformAxis == TRANSFORM_X){
					scale.y = onPressScale.y;
				}else if(transformAxis == TRANSFORM_Y){
					scale.x = onPressScale.x;
				}
			}
		}
		
		
		prevMouse = m;
	}
}

//--------------------------------------------------------------
void ofx2DCanvas::reset(){
	scale = {1,1};
	onPressScale  = {1,1};
	translate = {0,0};
	onPressTranslate = {0,0};
	bIsDragging = false;
}
