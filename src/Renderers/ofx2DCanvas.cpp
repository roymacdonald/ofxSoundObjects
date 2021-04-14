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
void ofx2DCanvas::end(bool bDrawInfo){
	ofPopMatrix();
	ofPopView();
//	if(bDrawInfo){
	
//	stringstream ss;
//		glm::vec2 size(width, height);
//	ss << "rect " << (ofRectangle)*this<<endl;
//	ss << "translate * scale " << (translate * scale) <<endl;
//	ss << "scale * size " << scale * size << endl;
//	ss << "translate - size "<< translate - size << endl;
//	
//	ss << "pressPos " << pressPos << endl;
//	ss << "relativePressPos " << relativePressPos << endl;
//	ss << "prevMouse " << prevMouse << endl;
//	ss << "mouseVel " << mouseVel << endl;
//	ss << "scale " << scale << endl;
//	ss << "onPressScale  " << onPressScale  << endl;
//	ss << "translate " << translate << endl;
//	ss << "onPressTranslate " << onPressTranslate << endl;
//	ss << "IsDragging " << boolalpha << bIsDragging << endl;
//	ss << "Translate " << boolalpha << bTranslate << endl;
//	ss << "TransformAxisSet " << boolalpha << bTransformAxisSet << endl;
//	ss << "Scrolling " << boolalpha << bScrolling << endl;
//	ss << "lastTap " << lastTap << endl;
	
//	ss << "transformAxis ";
//	switch(transformAxis){
//		case TRANSFORM_BOTH: ss << "BOTH" << endl; break;
//		case TRANSFORM_X: ss << "X" << endl; break;
//		case TRANSFORM_Y: ss << "Y" << endl; break;
//	
//	}
//
//	ofDrawBitmapStringHighlight(ss.str(), x+ 20, y+20);
//	}
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
	if(inside(m)){
		
		
		if(!bScrolling){
			auto curTap = ofGetElapsedTimeMillis();
			if(lastTap != 0 && curTap - lastTap < 300){
			reset();
			return;
			}
			lastTap = curTap;
			
			bIsDragging = true;
			bTranslate	=(m.button == OF_MOUSE_BUTTON_LEFT);
			pressPos = m;
		}else{
			
			bIsDragging = false;
			pressPos = {0,0};
		}
		
		prevMouse = m;
		onPressScale = scale;
		onPressTranslate = translate;
		
		
		
		relativePressPos = m - translate - getPosition();
		relativePressPos /= scale;
		
		bTransformAxisSet = !(ofGetKeyPressed(OF_KEY_SHIFT));
		transformAxis = TRANSFORM_BOTH;
		
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
}
//--------------------------------------------------------------
void ofx2DCanvas::mouseDragged(ofMouseEventArgs & m){
	updateMouse(m, false);
}
//--------------------------------------------------------------
void ofx2DCanvas::updateMouse(ofMouseEventArgs& m, bool bMouseScroll){
	if(!bMouseScroll) bScrolling = false;
	if(bScrolling){
	   }
	if(bIsDragging || bScrolling){
		
		glm::vec2  d;
		if(bScrolling){
			mouseVel = {m.scrollX, m.scrollY};
			pressPos += mouseVel;
			d = pressPos;
		}else{
			mouseVel = m  - prevMouse;
			d = m - pressPos;
		}
		
		
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
			mouseVel = (bAltPressed?0.1:1.0) * mouseVel /glm::vec2(width,height);
			scale += mouseVel + mouseVel * scale;
			translate = onPressTranslate - relativePressPos*(scale - onPressScale);
			
			if(transformAxis != TRANSFORM_BOTH){
				if(transformAxis == TRANSFORM_X){
					scale.y = onPressScale.y;
				}else if(transformAxis == TRANSFORM_Y){
					scale.x = onPressScale.x;
				}
			}
			
			if(scale.x < 1.0f) scale.x = 1.0f;
			if(scale.y < 1.0f) scale.y = 1.0f;
			
		}
		
		
		
		if(translate.x > 0.0f) translate.x = 0.0f;
		if(translate.y > 0.0f) translate.y = 0.0f;
		
		glm::vec2 size(width, height);
		auto ss = scale*size -size;
		
		
		
		if(translate.x < -ss.x) translate.x = -ss.x;
		if(translate.y < -ss.y) translate.y = -ss.y;
		
		
		prevMouse = m;
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
	scale = {1,1};
	onPressScale  = {1,1};
	translate = {0,0};
	onPressTranslate = {0,0};
	bIsDragging = false;
}
