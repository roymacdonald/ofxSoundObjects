//
//  ofxSoundObjectBaseRenderer.hpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/3/19.
//
//

#pragma once

template<class ClassToRender>
class ofxSoundObjectBaseRenderer{
public:
	ofxSoundObjectBaseRenderer(){}
	ofxSoundObjectBaseRenderer(ClassToRender* o);
	virtual void draw() = 0;
	
//	virtual void setObject(ClassToRender* o);
	
//protected:
	
	ClassToRender * obj = nullptr;
	
	/// render must be called only when something has changed and the gui needs to be redrawn. 
//	virtual void render() = 0;
	
};
