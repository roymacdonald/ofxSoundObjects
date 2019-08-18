//
//  ofxSlidersGrid.cpp
//  example-matrixMixer_tbb
//
//  Created by Roy Macdonald on 8/17/19.
//
//

#include "ofxSlidersGrid.h"

//----------------------------------------------------



//----------------------------------------------------
void ofxSlidersGrid::enable(){
	if(!bEnabled){
		bEnabled = true;
		mouseListeners.unsubscribeAll();
		mouseListeners.push(ofEvents().mousePressed.newListener(this, &ofxSlidersGrid::mousePressed));
		mouseListeners.push(ofEvents().mouseMoved.newListener(this, &ofxSlidersGrid::mouseMoved));
		mouseListeners.push(ofEvents().mouseDragged.newListener(this, &ofxSlidersGrid::mouseDragged));
		mouseListeners.push(ofEvents().mouseReleased.newListener(this, &ofxSlidersGrid::mouseReleased));
	}
}

//----------------------------------------------------
void ofxSlidersGrid::disable(){
	if(bEnabled){
		bEnabled = false;
		mouseListeners.unsubscribeAll();
	}
}
//----------------------------------------------------
void ofxSlidersGrid::toggle(){
	if(bEnabled){
		disable();
	}else{
		enable();
	}
}
//----------------------------------------------------
bool ofxSlidersGrid::isEnabled(){
	return bEnabled;
}
//----------------------------------------------------
void ofxSlidersGrid::checkOverGrid( const glm::vec2& p, bool bUpdateSlider, bool bPressed){
	bOverGrid = gridRect.inside(p);
	if(bPressed){
		pressedXInd = floor(ofMap(p.x, gridRect.x, gridRect.getMaxX(), 0, xSize, true));
		pressedYInd = floor(ofMap(p.y, gridRect.y, gridRect.getMaxY(), 0, ySize, true));
		overGridRect.x = gridRect.x + (overGridRect.width * pressedXInd );
		overGridRect.y = gridRect.y + (overGridRect.height * pressedYInd );
	}
	if(bUpdateSlider){
		float vol = ofMap( p.x - overGridRect.x, 0, overGridRect.width, 0, 1, true );
		
		if(parameters[pressedYInd][pressedXInd]){
			parameters[pressedYInd][pressedXInd]->param = vol;
		}
	}
}
//----------------------------------------------------
void ofxSlidersGrid::mousePressed(ofMouseEventArgs& args){
	checkOverGrid(args, true, true);
}
//----------------------------------------------------
void ofxSlidersGrid::mouseMoved(ofMouseEventArgs& args){
	checkOverGrid(args, false);
}
//----------------------------------------------------
void ofxSlidersGrid::mouseDragged(ofMouseEventArgs& args){
	checkOverGrid(args, true);
}
//----------------------------------------------------
void ofxSlidersGrid::mouseReleased(ofMouseEventArgs& args){
	checkOverGrid(args, true);
}

void addLineToMesh(ofVboMesh& mesh, const glm::vec3& p0, const glm::vec3& p1, const ofFloatColor& color){
	auto i = mesh.getVertices().size();
	mesh.addVertex(p0);
	mesh.addVertex(p1);
	mesh.addIndex(i);
	mesh.addIndex(i+1);
	mesh.addColors({color,color});
}
void addRectToMesh(ofVboMesh& mesh, const ofRectangle& r, const ofFloatColor& color, bool bAddLinesIndices){
//	std::cout << "addRectToMesh " << r << std::endl;
	auto i = mesh.getVertices().size();
	
	mesh.addVertex(r.getTopLeft());
	mesh.addVertex(r.getTopRight());
	mesh.addVertex(r.getBottomRight());
	mesh.addVertex(r.getBottomLeft());
	mesh.addColors({color,color,color,color});
	
	if(bAddLinesIndices){
		mesh.addIndex(i);
		mesh.addIndex(i+1);
		
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
		
		mesh.addIndex(i+2);
		mesh.addIndex(i+3);
		
		mesh.addIndex(i+3);
		mesh.addIndex(i);
	}else{
		mesh.addIndex(i);
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
		
		mesh.addIndex(i);
		mesh.addIndex(i+2);
		mesh.addIndex(i+3);
		
	}
}
//----------------------------------------------------
void ofxSlidersGrid::buildMeshes(){
	std::cout << "ofxSlidersGrid::buildMeshes" << std::endl;
	
	lineGridMesh.clear();
	slidersGridMesh.clear();
	
	
	lineGridMesh.setMode(OF_PRIMITIVE_LINES);
	lineGridMesh.setUsage(GL_STATIC_DRAW);
	
	slidersGridMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	slidersGridMesh.setUsage(GL_DYNAMIC_DRAW);
	
	
	addRectToMesh(lineGridMesh, gridRect, ofColor(100), true);
	
	if(xSize > 0 && ySize > 0){
		
//		std::cout << "xSize: " << xSize << " ySize: " << . ySize << std::endl;
		
		auto p0 = gridRect.getTopLeft();
		auto p1 = gridRect.getTopRight();
		
		float h = gridRect.height / ySize;
		float w = gridRect.width /  xSize;
		
		
		for(size_t i = 0; i < ySize-1; i++){
			p0.y += h;
			p1.y += h;
			addLineToMesh(lineGridMesh, p0, p1, ofColor(70));
		}
		p0 = gridRect.getTopLeft();
		p1 = gridRect.getBottomLeft();
		for(size_t i = 0; i < xSize-1; i++){
			p0.x += w;
			p1.x += w;
			addLineToMesh(lineGridMesh, p0, p1, ofColor(70));
		}
		
		overGridRect.width = w;
		overGridRect.height = h;
		
		for(size_t y = 0; y < ySize; y++){
			for(size_t x = 0; x < xSize; x++){
				addRectToMesh(slidersGridMesh,
							  {gridRect.x + (x * w) + 1,
								  gridRect.y + (y * h) + 1,
								  w-2,
								  h-2},
							  ofColor::lightBlue,
							  false);
			}
		}
	}
}
//----------------------------------------------------
void ofxSlidersGrid::updateParameter(GridParameter & p){
	updateSlider(p.x, p.y, p.param.get());
}
//----------------------------------------------------
void ofxSlidersGrid::updateSlider(size_t x, size_t y, float val){
	
	//	float vol = ofMap( p.x - overGridRect.x, 0, overGridRect.width, 0, 1, true );
	size_t vInd = (x + y * xSize)*4;
	auto& v = slidersGridMesh.getVertices();
	
	v[vInd +1].x = v[vInd].x + (val * (overGridRect.width - 2));
	v[vInd +2].x = v[vInd +1].x;
	
	//	if(obj != nullptr){
	//		obj->setVolumeForChannel(val, overYInd,overXInd);
	//	}
}
void ofxSlidersGrid::GridParameter::setup(const size_t& _x, const size_t& _y, ofParameter<float>& p, ofxSlidersGrid* grid){
	param.makeReferenceTo(p);
	listener = param.newListener(this, &ofxSlidersGrid::GridParameter::notify);
	this->grid = grid;
	x = _x;
	y = _y;
}


//----------------------------------------------------
void ofxSlidersGrid::GridParameter::notify(float &){
	if(grid != nullptr){
		grid->updateParameter(*this);
	}
}

//----------------------------------------------------
void ofxSlidersGrid::setGridSize(size_t _x, size_t _y){
//	if(xSize != _x || ySize != _y){
		xSize = _x;
		ySize = _y;
//	std::cout << "xSize: " << xSize << " ySize: " << ySize << std::endl;
		parameters.resize(ySize);
		for(size_t y = 0; y < parameters.size(); y++){
			parameters[y].resize(xSize);
			for(size_t x = 0; x < parameters[y].size(); x++){
				parameters[y][x] = make_unique<ofxSlidersGrid::GridParameter>();
				//				auto& p = parameters[y][x];
				//				p.x = x;
				//				p.y = y;
			}
		}
	buildMeshes();
//	}
}
//----------------------------------------------------
void ofxSlidersGrid::linkParameter(size_t x, size_t y, ofParameter<float> & param){
	if(y < parameters.size()){
		if(x < parameters[y].size()){
			parameters[y][x]->setup(x, y, param,this);
		}
	}
}
//----------------------------------------------------
void ofxSlidersGrid::draw(const ofRectangle& _gridRect){
	
	//	if(obj != nullptr){
	
	if(gridRect != _gridRect){
	
		//			numChansIn = obj->getNumInputChannels();
		//			numChansOut = obj->getNumOutputChannels();
		//
		gridRect = _gridRect;
//
	   	buildMeshes();
	}

	//		mainMesh.draw();
	lineGridMesh.draw();
	slidersGridMesh.draw();
	
	if(bOverGrid){
		ofPushStyle();
		ofNoFill();
		ofSetColor(ofColor::red);
		ofDrawRectangle(overGridRect);
		ofPopStyle();
	}
	
	stringstream ss;
	ss << "xSize " << xSize << std::endl;
	ss << "ySize " << ySize << std::endl;
	ss << " v " << slidersGridMesh.getVertices().size() << std::endl;
	
	ofDrawBitmapStringHighlight(ss.str(), 100,100);
	
	ofSetColor(ofColor::yellow);
//	for(auto& v: slidersGridMesh.getVertices()){
//		ofDrawCircle(v, 5);
//	}
	
	
	//	}
}
