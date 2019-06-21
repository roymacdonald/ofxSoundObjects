//
//  ofxSoundPatchingGui.cpp
//  example-patchingGui
//
//  Created by Roy Macdonald on 6/19/19.
//
//

#include "ofxSoundPatchingGui.h"
#define PRINT_FUNC std::cout << __PRETTY_FUNCTION__ << std::endl;
//--------------------------------------------------------------
//------------- ofxSoundPatchConnection
//--------------------------------------------------------------
ofxSoundPatchConnection::ofxSoundPatchConnection(std::shared_ptr<ofxSoundPatcher> patcher, std::shared_ptr<ofxSoundObjectGui> input):PatcherChild(patcher){
//	PRINT_FUNC
	this->input = input;
//	enableMouse();
	bTryingToConnect = true;
}
//--------------------------------------------------------------
ofxSoundPatchConnection::ofxSoundPatchConnection(std::shared_ptr<ofxSoundPatcher> patcher, std::shared_ptr<ofxSoundObjectGui> input, std::shared_ptr<ofxSoundObjectGui> output):PatcherChild(patcher){
//	PRINT_FUNC
	this->input = input;
	this->output = output;
	bTryingToConnect = false;
}
//--------------------------------------------------------------
void ofxSoundPatchConnection::drawTo(const glm::vec2& pos){
	if(!input.expired()){
		auto i = input.lock();
		auto c = i->outputRect.getCenter();
		ofSetColor(0);
		ofDrawLine(c , pos);
		std::cout << "connection drawTo " << c << " --> " << pos << std::endl;
	}
}
//--------------------------------------------------------------
void ofxSoundPatchConnection::draw(){

	if(!input.expired() && !output.expired()){
		auto i = input.lock();
		auto o = output.lock();
		ofDrawLine(i->outputRect.getCenter(), o->inputRect.getCenter());
	}else if(!input.expired() && bTryingToConnect){
		auto i = input.lock();
		auto c = i->outputRect.getCenter();
		endPos = {ofGetMouseX(), ofGetMouseY()};
		ofDrawLine(c, endPos);
//		std::cout << "connection drawTo " << c << " --> " << pos << std::endl;
	}
}
//--------------------------------------------------------------
void ofxSoundPatchConnection::setOutput(std::shared_ptr<ofxSoundObjectGui> out){
	auto o = getOutput();
	if(o == out){
		ofLogWarning("ofxSoundPatchConnection::setOutput") << "output is already set to the same object";
		return;
	}
	if(o){
		o->disconnectInputFrom(getInput());
	}
	output = out;
}
////--------------------------------------------------------------
//void ofxSoundPatchConnection::mouseMoved(ofMouseEventArgs& m){
////	if(output.expired()){
////		drawTo(m);
////	}
//	
//}
////--------------------------------------------------------------
//void ofxSoundPatchConnection::mouseDragged(ofMouseEventArgs& m){
////	if(output.expired()){
////		drawTo(m);
////	}
//}
////--------------------------------------------------------------
//void ofxSoundPatchConnection::mouseReleased(ofMouseEventArgs& m) {
//	if(bTryingToConnect){
//		auto i = getInput();
//		if(i){
////			i->checkValidConnection(shared_from_this(),m);
//		}
//	}
//}
//--------------------------------------------------------------
std::shared_ptr<ofxSoundObjectGui> ofxSoundPatchConnection::getInput(){
	if(!input.expired()){
		return input.lock();
	}
	return nullptr;
}
//--------------------------------------------------------------
std::shared_ptr<ofxSoundObjectGui> ofxSoundPatchConnection::getOutput(){
	if(!output.expired()){
		return output.lock();
	}
	return nullptr;
}

//--------------------------------------------------------------
//------------- ofxSoundObjectGui
//--------------------------------------------------------------
//----------  STATICS
float& ofxSoundObjectGui::ioWidth(){
	static std::unique_ptr<float> w = make_unique<float>(20.0f);
	return *w;
}
//--------------------------------------------------------------
float& ofxSoundObjectGui::defaultWidth(){
	static std::unique_ptr<float> w = make_unique<float>(ioWidth() *2 + 100.0f);
	return *w;
}
//--------------------------------------------------------------
ofEvent<shared_ptr<ofxSoundObjectGui>> &  ofxSoundObjectGui::inputRectPressed(){
	static std::unique_ptr<ofEvent<shared_ptr<ofxSoundObjectGui>>> e = make_unique<ofEvent<shared_ptr<ofxSoundObjectGui>>>();
	return *e;
}
//--------------------------------------------------------------
ofEvent<shared_ptr<ofxSoundObjectGui>> &  ofxSoundObjectGui::inputRectReleased(){
	static std::unique_ptr<ofEvent<shared_ptr<ofxSoundObjectGui>>> e = make_unique<ofEvent<shared_ptr<ofxSoundObjectGui>>>();
	return *e;
}
//--------------------------------------------------------------
ofEvent<shared_ptr<ofxSoundObjectGui>> &  ofxSoundObjectGui::outputRectPressed(){
	static std::unique_ptr<ofEvent<shared_ptr<ofxSoundObjectGui>>> e = make_unique<ofEvent<shared_ptr<ofxSoundObjectGui>>>();
	return *e;
}
//--------------------------------------------------------------
ofEvent<shared_ptr<ofxSoundObjectGui>> &  ofxSoundObjectGui::outputRectReleased(){
	static std::unique_ptr<ofEvent<shared_ptr<ofxSoundObjectGui>>> e = make_unique<ofEvent<shared_ptr<ofxSoundObjectGui>>>();
	return *e;
}


//---------- 
//--------------------------------------------------------------
ofxSoundObjectGui::ofxSoundObjectGui(shared_ptr<ofxSoundPatcher> patcher, const glm::vec2& pos):PatcherChild(patcher){
//	this->patcher = patcher;
	this->set(pos.x,pos.y, defaultWidth(), ioWidth());
	updateRects();
	enableMouse();
}
//--------------------------------------------------------------
ofxSoundObjectGui::ofxSoundObjectGui(shared_ptr<ofxSoundPatcher> patcher):ofxSoundObjectGui(patcher, {0,0}){}
//--------------------------------------------------------------
void ofxSoundObjectGui::updateRects(){
	inputRect.set(this->x, this->y, ioWidth(), ioWidth());
	outputRect.set(this->getMaxX() - ioWidth(), this->y, ioWidth(), ioWidth());
}	
//--------------------------------------------------------------
void ofxSoundObjectGui::drawHightlightRect( ofRectangle & rect, HighlightState state, const ofColor& hl_color, const ofColor& color, float lineWidth){
	bool bHL = (highlightState==state);
	ofSetColor(bHL?hl_color:color);
	ofSetLineWidth(bHL?lineWidth:1);
	ofDrawRectangle(rect);

}
//--------------------------------------------------------------
void ofxSoundObjectGui::draw(){

	ofFill();
	ofSetColor(70);
	ofDrawRectangle(*this);
	ofNoFill();
	ofSetColor(200);
	drawHightlightRect(*this, HIGHLIGHT_RECT);
	drawHightlightRect(inputRect, HIGHLIGHT_INPUT);
	drawHightlightRect(outputRect, HIGHLIGHT_OUTPUT);
	ofSetColor(0);
//	if(connectionState == CONNECTING){
//		auto c = outputRect.getCenter();
//		ofDrawLine(c.x, c.y, ofGetMouseX(), ofGetMouseY());
//	}else{
//		if(inConnection != nullptr){
//			ofDrawLine(inConnection->outputRect.getCenter(), inputRect.getCenter());
//		}
//	}
//	auto c = outputRect.getCenter();
//	for(auto& o : outConnections){
//		if(o){
//			ofDrawLine(c, o->inputRect.getCenter());
//		}
//	}
//	for(auto& oc: outConnections){
//		oc->draw();
//	}
}
////--------------------------------------------------------------
//bool ofxSoundObjectGui::checkValidConnection(shared_ptr<ofxSoundObjectGui> connection, const glm::vec2& pos){
//	PRINT_FUNC
//	if(connection){
//		auto p = getPatcher();
//		if(p){
//			auto i = p->getInputAt(pos);
//			if(i != nullptr){
//				i->setInputConnection(connection);
//				return true;
//			}else{
//				
//			}
//		}
//	}
//	return false;
//}

//--------------------------------------------------------------
void ofxSoundObjectGui::enableMouse(){
	mouseListeners.push(ofEvents().mousePressed.newListener(this, &ofxSoundObjectGui::mousePressed));
	mouseListeners.push(ofEvents().mouseReleased.newListener(this, &ofxSoundObjectGui::mouseReleased));
	mouseListeners.push(ofEvents().mouseDragged.newListener(this, &ofxSoundObjectGui::mouseDragged));
	mouseListeners.push(ofEvents().mouseMoved.newListener(this, &ofxSoundObjectGui::mouseMoved));
}
//--------------------------------------------------------------
void ofxSoundObjectGui::disableMouse(){
	mouseListeners.unsubscribeAll();
}


//--------------------------------------------------------------
void ofxSoundObjectGui::connectOutputTo(shared_ptr<ofxSoundObjectGui> other){
	if(other.get() == this){
		ofLogWarning("ofxSoundObjectGui::connectTo") << "Can not connect one object to itself.";
		return;
	}
	if(outConnections.count(other) == 0){
		outConnections.insert(other);
		if(obj){
			obj->connectTo(*(other->obj));
		}
		other->connectInputTo(shared_from_this());
//		setConnectionState(CONNECTED);
	}else{
		ofLogWarning("ofxSoundObjectGui::connectTo") << "Already connected!";
	}
}
//--------------------------------------------------------------
void ofxSoundObjectGui::disconnectOutputFrom(shared_ptr<ofxSoundObjectGui> other){
	if(other){
		outConnections.erase(other);
//		if(outConnections.size()==0){
//			setConnectionState(DISCONNECTED);
//		}
	}
}
//--------------------------------------------------------------
void ofxSoundObjectGui::connectInputTo(std::shared_ptr<ofxSoundObjectGui> connection){
	PRINT_FUNC
	if(connection){
		disconnectInputFrom(inConnection);
		inConnection = connection;
//		setConnectionState(CONNECTED);		
	}
}
//--------------------------------------------------------------
void ofxSoundObjectGui::disconnectInputFrom(shared_ptr<ofxSoundObjectGui> connection){
	PRINT_FUNC
	if(inConnection != nullptr ){
		if(inConnection != connection) return;
		if(obj != nullptr){// && connection != nullptr ){
			obj->disconnectInput(inConnection->obj);
		}
		inConnection->disconnectOutputFrom(shared_from_this());
		inConnection = nullptr;
	}

	
//	setConnectionState(DISCONNECTED);
}
////--------------------------------------------------------------
//void ofxSoundObjectGui::setConnectionState(ConnectionState newState){
////	if(connectionState == newState){
//		connectionState = newState;
//		cout << "New connection state: ";
//		switch(connectionState){
//			case DISCONNECTED: cout << "DISCONNECTED"<<endl; break; 
//			case CONNECTING: cout << "CONNECTING"<<endl; break;
//			case CONNECTED: cout << "CONNECTED"<<endl; break;
//		}
////	}
//}
//--------------------------------------------------------------
bool ofxSoundObjectGui::mousePressed(ofMouseEventArgs& m){
	PRINT_FUNC
	if(inside(m)){
		clicOffset =  m - glm::vec2(getPosition());
		if(outputRect.inside(m)){
			auto s =shared_from_this();
			ofNotifyEvent(outputRectPressed(), s);
//			setConnectionState(CONNECTING);
			
//			outConnections.push_back(make_shared<ofxSoundObjectGui>(shared_from_this()));
//			std::cout << "new connection" << std::endl;
			// return true;
		}
		else if(inputRect.inside(m)){
			if(inConnection != nullptr){
				auto s =shared_from_this();
				ofNotifyEvent(inputRectPressed(), s);
//				auto tempIn = inConnection;
//				disconnectInput();
//				tempIn->setConnectionState(CONNECTING);
			}
			
		}else{
			bDragging = true;
		}
		return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundObjectGui::mouseReleased(ofMouseEventArgs& m){
	if(bDragging){
		bDragging = false;
		return true;
	}
		if(inside(m)){	
			if(outputRect.inside(m)){
				auto s =shared_from_this();
				ofNotifyEvent(outputRectReleased(), s);
				return true;
			}
			else if(inputRect.inside(m)){
				auto s =shared_from_this();
				ofNotifyEvent(inputRectReleased(), s);
				return true;
			}
			return true;
		}
		// && connectionState == CONNECTING){
//		auto p = getPatcher();
//		if(p){
//			auto i = p->getInputAt(m);
//			if(i != nullptr){
//				connectTo(i);
//			}else{
////				setConnectionState(outConnections.size()?CONNECTED:DISCONNECTED);
//			}
//		}
	
	return false;
}
//--------------------------------------------------------------
void ofxSoundObjectGui::mouseDragged(ofMouseEventArgs& m){
	if(bDragging ){
		setPosition(glm::vec3(m - clicOffset, 0.0f));
		updateRects();
	
		
	}
}
//--------------------------------------------------------------
void ofxSoundObjectGui::mouseMoved(ofMouseEventArgs& m){
	if(inside(m)){
		if(inputRect.inside(m)){
			highlightState = HIGHLIGHT_INPUT;
		}else if(outputRect.inside(m)){
			highlightState = HIGHLIGHT_OUTPUT;
		}else{
			highlightState = HIGHLIGHT_RECT;
		}
	}else{
		highlightState = NO_HIGHLIGHT;
	}
}
//--------------------------------------------------------------
//------------- ofxSoundPatcher
//--------------------------------------------------------------
ofxSoundPatcher::ofxSoundPatcher(){
	
	ioClicListeners.push(ofxSoundObjectGui::inputRectPressed().newListener(this, &ofxSoundPatcher::inputRectPressedCB));
	ioClicListeners.push(ofxSoundObjectGui::inputRectReleased().newListener(this, &ofxSoundPatcher::inputRectReleasedCB));
	ioClicListeners.push(ofxSoundObjectGui::outputRectPressed().newListener(this, &ofxSoundPatcher::outputRectPressedCB));
	ioClicListeners.push(ofxSoundObjectGui::outputRectReleased().newListener(this, &ofxSoundPatcher::outputRectReleasedCB));
	
}
//--------------------------------------------------------------
void ofxSoundPatcher::inputRectPressedCB(shared_ptr<ofxSoundObjectGui>& gui_obj){
	if(objsInputConnectionsMap.count(gui_obj)){
		objsInputConnectionsMap[gui_obj]->bTryingToConnect = true;
		objsInputConnectionsMap[gui_obj]->setOutput(nullptr);
	}
}
//--------------------------------------------------------------
void ofxSoundPatcher::inputRectReleasedCB(shared_ptr<ofxSoundObjectGui>& gui_obj){
	if(gui_obj){
	for(auto& c: newConnections){
		if(gui_obj->inputRect.inside(c->endPos)){
			c->setOutput(gui_obj);
//			auto i = c->getInput();

			if(objsInputConnectionsMap.count(gui_obj)){
				removeConnection(objsInputConnectionsMap[gui_obj]);
			}
			//			for(auto& cc: connections){
//				if(cc->getOutput() == gui_obj){
//					removeConnection(cc);
//					break;
//				}
//			}
			
//			auto i = c->getInput();
//			if(i){
//				i->connectOutputTo(gui_obj);
//			}
			addConnection(c);
//			newConnections.erase(c);
			break;
		}
	}
	
	}
}
//--------------------------------------------------------------
void ofxSoundPatcher::outputRectPressedCB(shared_ptr<ofxSoundObjectGui>& gui_obj){
	newConnections.insert(make_shared<ofxSoundPatchConnection>(shared_from_this(), gui_obj));
}
//--------------------------------------------------------------
void ofxSoundPatcher::outputRectReleasedCB(shared_ptr<ofxSoundObjectGui>& gui_obj){
	
}
//--------------------------------------------------------------
void ofxSoundPatcher::createObjAt(const glm::vec2 & pos){
	//PRINT_FUNC
	objs.push_back(make_shared<ofxSoundObjectGui>(shared_from_this(), pos));
}
//--------------------------------------------------------------
void ofxSoundPatcher::draw(){
	for(auto& o: objs){
		o->draw();
	}
	ofSetColor(ofColor::black);
	for(auto& c: connections){
		c->draw();
	}
	ofSetColor(ofColor::red);
	for(auto& c: newConnections){
		c->draw();
	}
}
//--------------------------------------------------------------
std::shared_ptr<ofxSoundObjectGui> ofxSoundPatcher::getInputAt(const glm::vec2& pos){
//	PRINT_FUNC
	for(auto& o: objs){
		if(o->inputRect.inside(pos)){
			return o;
		}
	}
	return nullptr;
}


//--------------------------------------------------------------
void ofxSoundPatcher::addConnection(std::shared_ptr<ofxSoundPatchConnection> connection){
	auto i = connection->getInput();
	auto o = connection->getOutput();
	if(connections.count(connection) == 0 && i && o){
		connections.insert(connection);
		objsInputConnectionsMap[o] = connection;
		i->connectOutputTo(o);
	}
	if(newConnections.count(connection)){
		newConnections.erase(connection);
	}
}
//--------------------------------------------------------------
void ofxSoundPatcher::removeConnection(std::shared_ptr<ofxSoundPatchConnection> connection){
	auto i = connection->getInput();
	auto o = connection->getOutput();
	if(i && o && connections.count(connection) > 0) {
		o->disconnectInputFrom(i);
		objsInputConnectionsMap.erase(o);
		connections.erase(connection);
	}
	if(newConnections.count(connection)){
		newConnections.erase(connection);
	}	
}




