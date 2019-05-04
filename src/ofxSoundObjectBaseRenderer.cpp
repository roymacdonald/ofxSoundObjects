//
//  ofxSoundObjectBaseRenderer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/3/19.
//
//

#include "ofxSoundObjectBaseRenderer.h"
//--------------------------------------------
template<class ClassToRender>
ofxSoundObjectBaseRenderer<ClassToRender>::ofxSoundObjectBaseRenderer(ClassToRender* o):obj(o){}
//--------------------------------------------
template<class ClassToRender>
void ofxSoundObjectBaseRenderer<ClassToRender>::setObject(ClassToRender* o){
	obj = o;
}
