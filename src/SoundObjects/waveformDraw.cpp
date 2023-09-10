
#include "waveformDraw.h"
//--------------------------------------------------------------
template<typename BufferType>
waveformDraw_<BufferType>::waveformDraw_():
ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
//bBypass(false),
_bDrawWaveformDots(false)

{
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
    canvas.setCamNeedsUpdate();
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
    if(gridSpacing > 0) {
        gridMesh.draw();
        
        sampleDistance = (this->width  / ((float)buffer.getNumFrames() * canvas.getCamera().getScale().x));
        
        if(sampleDistance > 4.0f ){
            gridMeshPerSample.draw();
        }
        if(sampleDistance > 4.0f/8 ){
            gridMeshEvery8.draw();
        }
    }
    
    //Draw center line
    auto chans = getNumChannels();
    if(chans > 0){
        float h  = 1.0f/chans;
        for(int i = 0; i < chans; ++i){
            ofDrawLine(0, h*(0.5f + i), 1,h*(0.5f + i) );
        }
    }
    
    
    ofSetColor(waveColor);
    if(_bDrawWaveformDots.load()){
        glPointSize(3);
    }
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
	
		
		if(bRenderWaveforms){
			makeWaveformMesh();
			updateWaveformMesh();
			bRenderWaveforms = false;
		}
    makeGrid();

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
				waveforms[i].setMode(_bDrawWaveformDots.load()? OF_PRIMITIVE_POINTS:OF_PRIMITIVE_LINE_STRIP);
//                waveforms[i].setMode(OF_PRIMITIVE_LINE_STRIP);
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

void makeGridLines(ofVboMesh & mesh, const vector<glm::vec3>& verts, size_t stride, const ofFloatColor& color){
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.setUsage(GL_STATIC_DRAW);
    
//    float xSpace = 1.0f / (float)(numLines);
    for(int i = 0; i < verts.size(); i+=stride){
        const auto& v = verts[i];
        mesh.addVertex({v.x , 0, 0});
        mesh.addVertex({v.x , 1, 0});
        mesh.addColor(color);
        mesh.addColor(color);
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

//				float xSpace = (float) gridSpacing / (float)(buffer.getNumFrames());
//				ofRectangle r(0,0, xSpace, 1);
				
				vector<ofFloatColor> colors(6, gridColor);
                if(waveforms.size()){
                    const auto&  verts = waveforms[0].getVertices();
				for(int i = 0; i < buffer.getNumFrames(); i+= (gridSpacing *2)){
                    if(i + gridSpacing < verts.size()){
                    const auto& v0 = verts[i];
                    const auto& v1 = verts[i + gridSpacing];
                    gridMesh.addVertex({v0.x, 0.0f, 0.0f});
                    gridMesh.addVertex({v1.x, 0.0f, 0.0f});
                    gridMesh.addVertex({v0.x, 1.0f, 0.0f});
										
					
                    gridMesh.addVertex({v1.x, 0.0f, 0.0f});
                    gridMesh.addVertex({v1.x, 1.0f, 0.0f});
                    gridMesh.addVertex({v0.x, 1.0f, 0.0f});
					
					gridMesh.addColors(colors);
                    }
//					r.x+= (xSpace*2);
				}
                }
                if(waveforms.size()){
                    const auto&  verts = waveforms[0].getVertices();
                    makeGridLines(gridMeshPerSample, verts, 1, gridColorPerSample);
                    makeGridLines(gridMeshEvery8, verts, 8, gridColorEvery8);
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
template<typename BufferType>
void waveformDraw_<BufferType>::setBackgroundColor(const ofColor& color){
	backgroundColor = color;
}
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
template<typename BufferType>
const ofColor&  waveformDraw_<BufferType>::getBackgroundColor(){
	return backgroundColor;
}
//--------------------------------------------------------------
template<typename BufferType>
const ofColor&  waveformDraw_<BufferType>::getMarginColor(){
	return marginColor;
}

//--------------------------------------------------------------
template<typename BufferType>
bool waveformDraw_<BufferType>::isDrawingWaveformDots(){
    return _bDrawWaveformDots.load();
}
//--------------------------------------------------------------
template<typename BufferType>
void waveformDraw_<BufferType>::setDrawWaveformDots(bool bDrawDots){
    if(_bDrawWaveformDots.load() != bDrawDots){
        _bDrawWaveformDots = bDrawDots;

        for (auto & w : waveforms) {
            w.setMode(_bDrawWaveformDots.load()? OF_PRIMITIVE_POINTS:OF_PRIMITIVE_LINE_STRIP);
        }

    }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void circularBufferWaveformDraw::allocate(size_t numFrames, size_t numChannels){
    buffer.allocate(numFrames, numChannels);
}

//--------------------------------------------------------------
void circularBufferWaveformDraw::pushBuffer(ofSoundBuffer& _buffer){
    std::lock_guard<std::mutex> lck(mutex1);
    buffer.push(_buffer);
    bRenderWaveforms = true;
    
}
//--------------------------------------------------------------
void circularBufferWaveformDraw::process(ofSoundBuffer &input, ofSoundBuffer &output){
	
//    std::lock_guard<std::mutex> lck(mutex1);
//    buffer.push(input);
//    bRenderWaveforms = true;
//
    pushBuffer(input);
	output = input;
}

//--------------------------------------------------------------
void circularBufferWaveformDraw::setNumBuffers(size_t numBuffers){
    buffer.setNumBuffersToStore(numBuffers);
}

//--------------------------------------------------------------
size_t circularBufferWaveformDraw::getNumBuffers(){
   return buffer.getNumBuffersToStore();
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

//        size_t bIndex = buffer.getNumFrames()-1;
//        if(!bDrawUnindexed.load()){
            size_t bIndex = buffer.getPushIndex() / buffer.getNumChannels();
//        }
		for (int j = 0; j < chans; j++) {
			
			auto & wv = waveforms[j].getVertices();

            if(buffer.size() != (wv.size() * chans)){
                ofLogWarning("buffer.size() != (waveforms[j].getNumVertices() * chans)) " ) << buffer.size()  << " != " << (wv.size() * chans);
            }
            
//            auto lastIndex = wv.size() - 1;
            
//            if(!bDrawUnindexed.load()){
//                bIndex = buffer.getPushIndex() / buffer.getNumChannels();
                
            size_t a  = wv.size() - 1;
                for(size_t i = bIndex; i< wv.size(); i++){
                    wv[a].y = ofMap(buffer[(i * chans) + j], -1, 1, h*(j+1), h*j );
                    a --;
                }
//            if(a != (bIndex -1)){
//                cout << "ouch!@\n";
//            }
                for(size_t i = 0; i< bIndex; i++){
                    wv[a].y = ofMap(buffer[(i * chans) + j], -1, 1, h*(j+1), h*j );
                    a--;
                }
//			for(size_t i=0; i< wv.size(); i++){
//				wv[i].y = ofMap(buffer[(bIndex * chans) + j], -1, 1, h*(j+1), h*j );
//				if(bIndex > 0){
//					-- bIndex ;
//				}else{
//					bIndex = buffer.getNumFrames()-1;
//				}
//			}
//            }else{
//                for(size_t i=0; i< wv.size(); i++){
//                    wv[lastIndex - i].y = ofMap(buffer[(i * chans) + j], -1, 1, h*(j+1), h*j );
//                }
//            }
		}
	}
}

template class waveformDraw_<ofSoundBuffer>;
template class waveformDraw_<ofxCircularSoundBuffer>;












