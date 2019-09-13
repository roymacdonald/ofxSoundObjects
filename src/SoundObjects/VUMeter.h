//
//  ofxSoundObjectVURenderer.h
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/5/19.
//
//

#pragma once
#include "ofMain.h"
#include "ofxSoundObject.h"
#include "ofxSoundUtils.h"
#include "ofxSoundRendererUtils.h"

using namespace ofxSoundRendererUtils;
class ofxSoundMatrixMixer;
class VUMeter:  public ofxSoundObject{
public:
	friend class ofxSoundMatrixMixer;

	virtual  std::string getName() override{ return "VUMeter";}
	
	VUMeter();
	VUMeter(const VUMeter& a);
	VUMeter& operator=(const VUMeter& mom);
	
	
	enum DrawMode{
		/// The VU meter is draw vertically. meaning that it will move up and down
		VU_DRAW_VERTICAL =0,
		/// The VU meter is draw horizontally. meaning that it will move left to right
		VU_DRAW_HORIZONTAL
	};
	enum StackMode{
		/// stack mode only is used when the VUMeter is used with more than one channel of audio.
		
		/// The VU meter is stacked vertically. meaning that the channels VUMeters will be stacked one on top of the other.
		VU_STACK_VERTICAL =0,
		/// The VU meter is stacked horizontally. meaning that the channels VUMeters will be stacked sideways, fron left to right
		VU_STACK_HORIZONTAL
	} ;
	

	/// \brief get Draw Mode
	DrawMode getDrawMode();
	/// \brief get Stack Mode
	StackMode getStackMode();
	
	/// \brief set Draw Mode
	void setDrawMode(DrawMode newMode);
	/// \brief set Stack Mode
	void setStackMode(StackMode newMode);
	
	
	
	/// \brief setup the VU Meter and its drawing configuration
	///
	/// Setup the VU meter size and position where to be drawn and its draw and stack modes.
	///
	/// \param drawRectangle the rectangle where the vu meter will be drawn
	/// \param drawMode the draw mode to be use. look at DrawMode above
	/// \param stackMode the stack mode to be use. look at StackMode above
	///
	/// You can pass an ofRectangle defined elsewhere in the code.
	///
	/// ```
	///		ofRectangle r (100,200,300,500); // just an ofRectangle properly setup. It is up to you to do so
	///		 //  assuming that there is a VUMeter vuMeter; defined elsewhere.
	///		vuMeter.setup(r);// if you dont pass a second and/or third parameters the defaults are used
	///		// which is the same as the following
	///		vuMeter.setup(r, VU_DRAW_VERTICAL, VU_STACK_HORIZONTAL); // the second and third parameters passed here are the defaults used when you dont pass these.
	///	```
	/// or you can set it directly within the function call
	/// ```
	///		// put the correct values in the ofRectangle. these are x, y, width and height respectively.
	///		vuMeter.setup(ofRectangle(100, 200, 300, 500), VU_DRAW_VERTICAL, VU_STACK_HORIZONTAL);
	///		// or assuming the default second and third parameters
	///		// vuMeter.setup(ofRectangle(100, 200, 300, 500));
	///
	///```
	/// or instead of the previous use the shorter version with the initializer list for the ofRectangle; wrap the values passed to ofRectangle in the function above in curly brackets.
	/// ```
	///		vuMeter.setup({100, 200, 300, 500});// notice the curly brackets { and }
	///```
	void setup(const ofRectangle& drawRectangle, DrawMode drawMode = VU_DRAW_VERTICAL, StackMode stackMode = VU_STACK_HORIZONTAL);
	
	/// \brief setup the VU Meter and its drawing configuration
	///
	/// Setup the VU meter size and position where to be drawn and its draw and stack modes.
	///
	/// \param x X axis position of upper left corner where its going to be drawn
	/// \param y Y axis position of upper left corner where its going to be drawn
	/// \param w Width of the VUMeter
	/// \param h Height of the VUMeter
	/// \param drawMode the draw mode to be use. look at DrawMode above
	/// \param stackMode the stack mode to be use. look at StackMode above
	
	void setup(float x, float y, float w, float h, DrawMode drawMode = VU_DRAW_VERTICAL, StackMode stackMode = VU_STACK_HORIZONTAL);
	
	/// \brief Sets the rectangle where to draw the VU meter
	///
	/// Sets the rectangle where the VU meter will be drawn. This is the same one that is set using the setup function but calling setup also sets the draw and stack modes.
	///
	/// \param drawRectangle the rectangle that will define where to draw the VU meter.

	void setDrawRectangle(const ofRectangle& drawRectangle);
	
	/// \brief Resets the peaks being held.
	///
	/// Resets the peaks being held. Useful when you have a long peak hold time and you want to manually reset its value
	
	void resetPeak();
	
	/// \brief calculate the values of the VU Meter using the passed ofSoundBuffer.
	///
	/// Calculate the values of the VU Meter using the passed ofSoundBuffer.
	/// This is the function that is called internally when you have  a VUMeter object as part of a larger chain of ofxSoundObjects.
	/// This is made public as it can be useful on certain scenarios
	void calculate(ofSoundBuffer &input);
	
	/// \brief Draw the vu meter
	/// Draw the VU Meter. Call this within the ofApp::draw() function, either directly or through a nested function
	void draw();
	
	/// \brief Retrieve the RMS value for a certain channel.
	///
	/// Retrieve the RMS (Root Mean Square) value for a certain channel.
	///
	/// \param channel The channel number from which you want its RMS value. Notice that the first channel is 0 not 1.
	/// \returns float value with the RMS. Its range is 0 to 1
	float getRmsForChannel(size_t channel) const;
	
	
	/// \brief Retrieve the current peak value for a certain channel.
	///
	/// Retrieve the current peak value for a certain channel. This is the peak of the last processed buffer.
	///
	/// \param channel The channel number from which you want its peak value. Notice that the first channel is 0 not 1.
	/// \returns float The peak value. Its range is 0 to 1
	float getPeakForChannel(size_t channel) const;
	

	
	///---------- static functions ------------------
	/// Static Functions behave as globals for all the instances of this class, and will affect all of these.
	///
	/// You can use the following static functions as setters or getters.
	/// If you want to set the color of the RMS value you can do, for example
	///	VUMeter::getRmsColor() = ofColor::blue;
	/// or to set the peak release time
	///	VUMeter::getPeakReleaseTime() = 100;
	/// or simply to retrieve this one of this values.
	/// ofColor peakHoldColor = VUMeter::getPeakHoldColor();
	///
	/// \brief the color used for the filled region of the VUMeter which corresponds to its RMS value
	static ofColor& getRmsColor();
	/// \brief the color used for the current peak line of the VUMeter
	static ofColor& getPeakColor();
	/// \brief the color used for the held peak line of the VUMeter
	static ofColor& getPeakHoldColor();
	/// \brief the color used for the lines that surround the the VUMeter
	static ofColor& getBorderColor();
	/// \brief When the VU Meter is clipping- the RMS and/or peak values are more than 1 - the vumeter will be drawn with this color
	static ofColor& getClippingColor();
	/// \brief The peak hold time. This is the amount of time that the held peak is held after being detected.
	static uint64_t& getPeakHoldTime();
	/// \brief The peak release time is how long it takes for the held peak value to reset. This is mostly used as how long it takes for the animation of it to fall down.
	static uint64_t& getPeakReleaseTime();
	/// \brief Force rebuilding the VUMeter.
	/// Force rebuilding the VUMeter.
	/// in case that you've modified the colors using any of the above static functions you must call VUMeter::getForceRebuild() = true; for these changes to take effect.
	static bool& getForceRebuild();
protected:

	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	
	void buildMeshes();
	
	void updateMeshes();
	
	DrawMode drawMode = VU_DRAW_VERTICAL;
	StackMode stackMode = VU_STACK_HORIZONTAL;

	
	
	ofRectangle drawRect;
	std::vector <ofRectangle> vuRects;
	
private:
	
	bool bNeedsUpdate = false;
	bool bNeedsBuildMeshes = true;
	struct VuData{
		std::vector<float>rms;
		std::vector<float>peak;
		std::vector<float>lastPeak;
		std::vector<float>holdPeak;
		std::vector<bool> bClippingPeak;
		std::vector<bool> bClippingRms;
		void resize(const size_t& newSize){
			ofxSoundUtils::resize_vec(rms, newSize);
			ofxSoundUtils::resize_vec(peak, newSize);
			ofxSoundUtils::resize_vec(lastPeak, newSize);
			ofxSoundUtils::resize_vec(holdPeak, newSize);
			ofxSoundUtils::resize_vec(bClippingPeak, newSize);
			ofxSoundUtils::resize_vec(bClippingRms, newSize);

		}
		
	}processData, drawData;
	
	uint64_t lastPeakTime;
	mutable ofMutex mutex;
	
	
	ofVboMesh lineMesh, fillMesh, borderMesh;
	size_t prevNumChans = 0;
	
	
	
};
