//
//  ofxSoundPatchingGui.hpp
//  example-patchingGui
//
//  Created by Roy Macdonald on 6/19/19.
//
//

#pragma once
#include "ofxSoundObject.h"
#include "ofMain.h"

//class mouseListener{
//public:
//	ofEventListeners listeners;
//
//	void enableMouse(){
//		listeners.push(ofEvents().mousePressed.newListener(this, &mouseListener::mousePressed));
//		listeners.push(ofEvents().mouseReleased.newListener(this, &mouseListener::mouseReleased));
//		listeners.push(ofEvents().mouseDragged.newListener(this, &mouseListener::mouseDragged));
//		listeners.push(ofEvents().mouseMoved.newListener(this, &mouseListener::mouseMoved));
//	}
//	void disableMouse(){
//		listeners.unsubscribeAll();
//	}
//	
//	virtual void mousePressed(ofMouseEventArgs& m) {}
//	virtual void mouseReleased(ofMouseEventArgs& m) {}
//	virtual void mouseDragged(ofMouseEventArgs& m) {}
//	virtual void mouseMoved(ofMouseEventArgs& m) {}
//
//};

class ofxSoundObjectGui;
class ofxSoundPatcher;

class PatcherChild{
public:
	PatcherChild(std::shared_ptr<ofxSoundPatcher> patcher){
		this->patcher = patcher;
	}
	std::shared_ptr<ofxSoundPatcher> getPatcher(){
		if(!patcher.expired()){
			return patcher.lock();
		}
		return nullptr;
	}
private:
	weak_ptr<ofxSoundPatcher> patcher;
};


class ofxSoundPatchConnection: public PatcherChild, public enable_shared_from_this<ofxSoundPatchConnection>{
public:
	ofxSoundPatchConnection(std::shared_ptr<ofxSoundPatcher> patcher, std::shared_ptr<ofxSoundObjectGui> input);
	ofxSoundPatchConnection(std::shared_ptr<ofxSoundPatcher> patcher, std::shared_ptr<ofxSoundObjectGui> input, std::shared_ptr<ofxSoundObjectGui> output);
	
	void drawTo(const glm::vec2& pos);
	void draw();

//	void mouseDragged(ofMouseEventArgs& m);
//	void mouseMoved(ofMouseEventArgs& m);
//	void mouseReleased(ofMouseEventArgs& m);
	
	std::shared_ptr<ofxSoundObjectGui> getInput();
	std::shared_ptr<ofxSoundObjectGui> getOutput();
	void setOutput(std::shared_ptr<ofxSoundObjectGui> out);
	glm::vec2 endPos;
	bool bTryingToConnect = false;
private:
	
	std::weak_ptr<ofxSoundObjectGui> input; 
	std::weak_ptr<ofxSoundObjectGui> output;
	
};



class ofxSoundObjectGui:  public PatcherChild, public ofRectangle,  public std::enable_shared_from_this<ofxSoundObjectGui>{
public:
	ofxSoundObjectGui(shared_ptr<ofxSoundPatcher> patcher, const glm::vec2& pos);
	
	ofxSoundObjectGui(shared_ptr<ofxSoundPatcher> patcher);
	
	
	
	void updateRects();
	ofRectangle inputRect, outputRect;
	
	static float& ioWidth();
	static float& defaultWidth();
	
	static ofEvent<shared_ptr<ofxSoundObjectGui>> & inputRectPressed();
	static ofEvent<shared_ptr<ofxSoundObjectGui>> & inputRectReleased();
	static ofEvent<shared_ptr<ofxSoundObjectGui>> & outputRectPressed();
	static ofEvent<shared_ptr<ofxSoundObjectGui>> & outputRectReleased();
	
	
	void draw();

	
	
	void enableMouse();
	void disableMouse();
	
	bool mousePressed(ofMouseEventArgs& m);
	bool mouseReleased(ofMouseEventArgs& m);
	void mouseDragged(ofMouseEventArgs& m);
	void mouseMoved(ofMouseEventArgs& m);
	
	
//	std::vector<std::shared_ptr<ofxSoundPatchConnection>> outConnections;
//	std::shared_ptr<ofxSoundPatchConnection> inConnection = nullptr;
//	
	std::set<std::shared_ptr<ofxSoundObjectGui>> outConnections;
	std::shared_ptr<ofxSoundObjectGui> inConnection = nullptr;
	
	
	
//	bool checkValidConnection(shared_ptr<ofxSoundObjectGui> connection, const glm::vec2& pos);
	
	
	void connectOutputTo(shared_ptr<ofxSoundObjectGui> other);
	void disconnectOutputFrom(shared_ptr<ofxSoundObjectGui> other);
	void connectInputTo(std::shared_ptr<ofxSoundObjectGui> other);
	void disconnectInputFrom(shared_ptr<ofxSoundObjectGui> other);
	
	
protected:
	ofxSoundObject * obj = nullptr;
	
private:
	
	glm::vec2 clicOffset;
	bool bDragging = false;
	
	ofEventListeners mouseListeners;
	
	
	
	
//	enum ConnectionState{
//		DISCONNECTED = 0,
//		CONNECTING,
//		CONNECTED
//	
//	} connectionState = DISCONNECTED;
//	
//	void setConnectionState(ConnectionState newState);
	
	enum HighlightState{
		NO_HIGHLIGHT = 0,
		HIGHLIGHT_INPUT,
		HIGHLIGHT_OUTPUT,
		HIGHLIGHT_RECT
	} highlightState = NO_HIGHLIGHT;
	void drawHightlightRect( ofRectangle & rect, HighlightState state, const ofColor& hl_color = ofColor::yellow, const ofColor& color = ofColor(200), float lineWidth = 3);
};





class ofxSoundPatcher: public enable_shared_from_this<ofxSoundPatcher> {
public:
	ofxSoundPatcher();
	void createObjAt(const glm::vec2 & pos);
	void draw();
	std::shared_ptr<ofxSoundObjectGui> getInputAt(const glm::vec2& pos);
	
protected:
	void inputRectPressedCB(shared_ptr<ofxSoundObjectGui>& obj);
	void inputRectReleasedCB(shared_ptr<ofxSoundObjectGui>& obj);
	void outputRectPressedCB(shared_ptr<ofxSoundObjectGui>& obj);
	void outputRectReleasedCB(shared_ptr<ofxSoundObjectGui>& obj);

	ofEventListeners ioClicListeners;
	
	set<std::shared_ptr<ofxSoundPatchConnection>> connections;
	set<std::shared_ptr<ofxSoundPatchConnection>> newConnections;
	std::vector<std::shared_ptr<ofxSoundObjectGui>> objs;
	std::map<std::shared_ptr<ofxSoundObjectGui> , std::shared_ptr<ofxSoundPatchConnection>> objsInputConnectionsMap;
	
	void addConnection(std::shared_ptr<ofxSoundPatchConnection> connection);
	void removeConnection(std::shared_ptr<ofxSoundPatchConnection> connection);
	
};
