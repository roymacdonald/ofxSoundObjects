
#include "waveformDraw.h"
//--------------------------------------------------------------
template<typename BufferType>
waveformDraw_<BufferType>::waveformDraw_():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	setName("waveForm");
	bRenderWaveforms = false;
	bMakeGrid = false;
	waveColor = ofColor::white;
	
	marginColor = ofColor::red;
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setup(const ofRectangle& r){
		this->set(r);
		bRenderWaveforms = true;
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setup(float x, float y, float w, float h){
		setup({x, y, w, h});
}


//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::process(ofSoundBuffer &input, ofSoundBuffer &output){
	makeMeshFromBuffer(input);
	output = input;
	
}

template<typename BufferType>
void waveformDraw_<BufferType>::updateFbo(){
    bUpdateFbo = true;
}
template<typename BufferType>
void waveformDraw_<BufferType>::initFbo(){
    if(bUseFbo && (!fbo.isAllocated() || !ofIsFloatEqual(fbo.getWidth(), this->width) || !ofIsFloatEqual(fbo.getHeight(), this->height))){
        fbo.allocate(width, height);
        fbo.begin();
        ofClear(0,0,0,0);
        fbo.end();
        bUpdateFbo = true;
        if(listeners.empty()){
            listeners.push(canvas.onTransformBegin.newListener([&](){
                bIsCanvasTransforming = true;
                onTransformRect.set(canvas.getCamera().screenToWorld(getMin(), (ofRectangle)*this), canvas.getCamera().screenToWorld(getMax(), (ofRectangle)*this));
            }));
            listeners.push(canvas.onTransformEnd.newListener([&](){
                bIsCanvasTransforming = false;
                updateFbo();
            }));
        }
    }
}

template<typename BufferType>
void waveformDraw_<BufferType>::enableFbo(){
    bUseFbo = true;
    
}
template<typename BufferType>
void waveformDraw_<BufferType>::disableFbo(){
    bUseFbo = false;
    fbo.clear();
    listeners.unsubscribeAll();
}

//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::drawWave(){
    ofPushStyle();
    ofNoFill();
    ofSetColor(marginColor);
    //Draw bounding box
    ofDrawRectangle({0,0,this->width , this->height});
//    ofSetColor(255,100);
    
    
    ofPopStyle();
        
    ofPushMatrix();
    
    ofScale(this->width, this->height);
    if(gridSpacing > 0) gridMesh.draw();
    
    //Draw center line
    auto chans = getNumChannels();
    if(chans > 0){
        float h  = 1.0f/chans;
        for(int i = 0; i < chans; ++i){
            ofDrawLine(0, h*(0.5f + i), 1,h*(0.5f + i) );
        }
    }
    
    
    ofSetColor(waveColor);
    for(auto& w: waveforms){
        w.draw();
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::begin(){
    canvas.begin(*this);
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::end(){
    canvas.end();
}

//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::draw(const ofRectangle& viewport){

	if(!viewport.isZero() && viewport != (ofRectangle)*this){
			this->set(viewport);
			bRenderWaveforms = true;
		}
	
		makeGrid();
	
		if(bRenderWaveforms){
			makeWaveformMesh();
			updateWaveformMesh();
			bRenderWaveforms = false;
		}
	if(!bCanvasIsSetup){
		bCanvasIsSetup = true;
		canvas.enableMouse();
	}
//
    if(bUseFbo){
        initFbo();
        if(bUpdateFbo){
            bUpdateFbo = false;
            fbo.begin();
            ofClear(0, 0, 0,0);
            canvas.begin(ofRectangle(0,0,width, height));
            drawWave();
            canvas.end();
            fbo.end();
        }
        if(bIsCanvasTransforming){
            begin();
            fbo.draw(onTransformRect);
        }
        else{
            fbo.draw((ofRectangle)*this);
        }

        if(bIsCanvasTransforming){
            end();
        }

    }else{
        begin();
        drawWave();
        end();
    }

}
//--------------------------------------------------------------

template<typename BufferType>
void waveformDraw_<BufferType>::makeMeshFromBuffer(const ofSoundBuffer& buffer, bool bRenderToFbo){
	{
        
		std::lock_guard<std::mutex> lck(mutex1);
        if(bRenderToFbo){
            bUseFbo = true;
        }

		ofxSoundUtils::checkBuffers(buffer, this->buffer, true);
		
		buffer.copyTo(this->buffer);
		bRenderWaveforms = true;
	}
}
//--------------------------------------------------------------

template<typename BufferType>
void waveformDraw_<BufferType>::setGridSpacingByNumSamples(size_t spacing){
	if(gridSpacing != spacing){
		gridSpacing = spacing;
		
		bMakeGrid = true;
	}
}
//--------------------------------------------------------------

template<typename BufferType>
void waveformDraw_<BufferType>::updateWaveformMesh(){
	auto chans = buffer.getNumChannels();
	if(chans > 0 && buffer.size() >0){
		float h = 1.0f / float(chans);
		for (int j = 0; j < chans; j++) {
			auto & wv = waveforms[j].getVertices();
			for(size_t i=0; i< wv.size(); i++){
				wv[i].y = ofMap(buffer[i*chans + j], -1, 1, h*(j+1), h*j );
			}
		}
	}
}
//--------------------------------------------------------------

template<typename BufferType>
void waveformDraw_<BufferType>::makeWaveformMesh(){
	if (buffer.size() >0) {
		
		auto chans = buffer.getNumChannels();
		if(chans != waveforms.size() || (waveforms.size() > 0 && waveforms[0].getNumVertices() != buffer.getNumFrames())){
			waveforms.resize(chans);
			for (int i = 0; i < chans; i++) {
				waveforms[i].clear();
				waveforms[i].setMode(OF_PRIMITIVE_LINE_STRIP);
				waveforms[i].setUsage(GL_DYNAMIC_DRAW);				
			}
			float h = 1.0f / float(chans);
			
			float xInc = 1.0f/(float)(buffer.getNumFrames());
			glm::vec3 v;
			v.x =0;
			for(int i=0; i<buffer.getNumFrames(); i++){
				for (int j = 0; j < chans; j++) {
					v.y = ofMap(0, -1, 1, h*(j+1), h*j );
					waveforms[j].addVertex(v);
				}
				v.x += xInc;
			}
		}
	}
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setGridColor(const ofColor& color){
    gridColor = ofFloatColor(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f );
    
    if(gridMesh.getColors().size()){
        vector<ofFloatColor> newColors (gridMesh.getColors().size(), gridColor);
        gridMesh.getColors() = newColors;
    }
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::makeGrid(){
	if(bMakeGrid){
		gridMesh.clear();
		if(gridSpacing > 0){
			if (buffer.size() >0) {
				bMakeGrid = false;
				gridMesh.setMode(OF_PRIMITIVE_TRIANGLES);
				gridMesh.setUsage(GL_STATIC_DRAW);

				float xSpace = (float) gridSpacing / (float)(buffer.getNumFrames());
				ofRectangle r(0,0, xSpace, 1);
				
				vector<ofFloatColor> colors(6, gridColor);
				
				for(int i = 0; i < buffer.getNumFrames(); i+= (gridSpacing *2)){
					
					gridMesh.addVertex(r.getTopLeft());
					gridMesh.addVertex(r.getTopRight());
					gridMesh.addVertex(r.getBottomLeft());
										
					
					gridMesh.addVertex(r.getTopRight());
					gridMesh.addVertex(r.getBottomRight());
					gridMesh.addVertex(r.getBottomLeft());
					
					gridMesh.addColors(colors);
					
					r.x+= (xSpace*2);
				}
				
			}
		}
	}
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setWaveColor(const ofColor& color){
	waveColor = color;
}
//--------------------------------------------------------------
//template<typename BufferType>
//void waveformDraw_<BufferType>::setBackgroundColor(const ofColor& color){
//	backgroundColor = color;
//}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setMarginColor(const ofColor& color){
	marginColor = color;
}
//--------------------------------------------------------------
template<typename BufferType>
const ofColor&  waveformDraw_<BufferType>::getWaveColor(){
	return waveColor;
}
//--------------------------------------------------------------
//template<typename BufferType>
//const ofColor&  waveformDraw_<BufferType>::getBackgroundColor(){
//	return backgroundColor;
//}
//--------------------------------------------------------------
template<typename BufferType>
const ofColor&  waveformDraw_<BufferType>::getMarginColor(){
	return marginColor;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void circularBufferWaveformDraw::process(ofSoundBuffer &input, ofSoundBuffer &output){
	{
		std::lock_guard<std::mutex> lck(mutex1);
		buffer.push(input);
	}
	bRenderWaveforms = true;
	output = input;
}
//--------------------------------------------------------------
void circularBufferWaveformDraw::setNumBuffers(size_t numBuffers){
    buffer.setNumBuffersToStore(numBuffers);
}
//--------------------------------------------------------------
void circularBufferWaveformDraw::updateWaveformMesh() {
	auto chans = buffer.getNumChannels();
//	cout <<"circularBufferWaveformDraw::updateWaveformMesh()\n" <<" chans " << chans<< "  waveforms.size(): "<< waveforms.size() << endl;
	if(chans > 0 && waveforms.size()){
	
		
		if(waveforms[0].getNumVertices() != buffer.getNumFrames()) {
			cout << "waveformDraw<CircularSoundBuffer>::updateWaveformMesh : waveform num vertices != buffer numFrames" << endl;
			return;
		}
		
		float h = 1.0f / float(chans);

        size_t bIndex = buffer.getPushIndex() / buffer.getNumChannels();
        
		for (int j = 0; j < chans; j++) {
			
			auto & wv = waveforms[j].getVertices();

			
			for(size_t i=0; i< wv.size(); i++){
				wv[i].y = ofMap(buffer[(bIndex * chans) + j], -1, 1, h*(j+1), h*j );
				if(bIndex > 0){
					-- bIndex ;
				}else{
					bIndex = buffer.getNumFrames()-1;
				}
			}
		}
	}
}

template class waveformDraw_<ofSoundBuffer>;
template class waveformDraw_<ofxCircularSoundBuffer>;












