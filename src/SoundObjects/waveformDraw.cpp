
#include "waveformDraw.h"

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
	{
		std::lock_guard<std::mutex> lck(mutex1);
		ofxSoundUtils::checkBuffers(input, buffer, true);
		input.copyTo(this->buffer);
		
	}
	bRenderWaveforms = true;
	output = input;
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

	canvas.begin((ofRectangle)*this);
	
	ofPushStyle();
	ofNoFill();
	ofSetColor(100);
	ofDrawRectangle({0,0,this->width , this->height});
	ofPopStyle();
		
	ofPushMatrix();
	
	ofScale(this->width, this->height);
	if(gridSpacing > 0) gridMesh.draw();
	ofSetColor(ofColor::white);
	for(auto& w: waveforms){
		w.draw();
	}	
	ofPopMatrix();
	canvas.end();
}
//--------------------------------------------------------------

template<typename BufferType>
void waveformDraw_<BufferType>::makeMeshFromBuffer(const ofSoundBuffer& buffer){
	{
		std::lock_guard<std::mutex> lck(mutex1);
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
			size_t bi = wv.size() - 1;
			for(size_t i=0; i< wv.size(); i++, --bi){
				wv[i].y = ofMap(buffer[bi*chans + j], -1, 1, h*(j+1), h*j );
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
			
			float xInc = 1.0f/(float)(buffer.getNumFrames() -1);
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
				
				vector<ofFloatColor> colors(6, ofFloatColor(80.0f/255.0f));
				
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
//--------------------------------------------------------------
//--------------------------------------------------------------
void circularBufferWaveformDraw::process(ofSoundBuffer &input, ofSoundBuffer &output){
	{
		std::lock_guard<std::mutex> lck(mutex1);
		if(buffer.getNumFrames() != input.getNumFrames() * numBuffers){
			
			buffer.allocate(input.getNumFrames() * numBuffers, input.getNumChannels());
			cout << "circularBufferWaveformDraw::process  " << buffer.size() << endl;
			buffer.setSampleRate(input.getSampleRate());
		}
		buffer.push(input);
	}
	bRenderWaveforms = true;
	output = input;
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
		
		for (int j = 0; j < chans; j++) {
			
			auto & wv = waveforms[j].getVertices();
			size_t bIndex = buffer.getPushIndex() / buffer.getNumChannels();
			
			for(size_t i=0; i< wv.size(); i++){
				wv[i].y = ofMap(buffer[bIndex * chans + j], -1, 1, h*(j+1), h*j );
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












