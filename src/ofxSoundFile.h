/*
 * ofxSoundFile.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofMain.h"
#include "dr_wav.h"
#include "ofxAudioFile.h"
/// reads a sound file into an ofSoundBuffer.
/// encoding support varies by platform.
bool ofxLoadSound(ofSoundBuffer &buffer, std::string path);

/// writes an ofSoundBuffer as a 16-bit PCM WAV file.
bool ofxSaveSound(const ofSoundBuffer &buffer, std::string path);

class ofxSoundFile{
public:
	ofxSoundFile();
	~ofxSoundFile();
	ofxSoundFile(std::string path);

	
	
	/// close and reset do the same these are here just for convenience
	void reset();
	void close();
	
	/// opens a sound file and put into an ofSoundBuffer
	bool load(std::string filepath);
	/// loads a file asynchronusly
	bool loadAsync(std::string filepath);
	/// opens a file but it does not read its contents. use this for opening a file for streming its data on demand.
	bool openFileStream(std::string filepath);
	
	/// writes an ofSoundBuffer as a PCM WAV file

	static bool save(std::string _path, const ofSoundBuffer &buffer);
	
	/// reads a file into an ofSoundBuffer.
	/// if the file was open on non streaming mode (using load or loadAsync), this will resize the buffer to fit the entire file.
	/// supplying a "samples" argument will read only the given number of samples
	/// on streaming mode it will read from the file on the disk
	size_t readTo(ofSoundBuffer &buffer, std::size_t outNumFrames, std::size_t fromFrame = 0, bool loop = true);
	
	
	
	/// this function is only useful when steaming from a file.
	/// It will move the current reading position to the passed frame. It returns true if successful, false otherwise.
	bool seekToFrame(uint64_t frame);
	
	
	/// returns sound file duration in milliseconds
	const uint64_t 		& getDuration() const;
	const unsigned int 	& getNumChannels() const;
	const unsigned int 	& getSampleRate() const;
	const uint64_t 		& getNumFrames() const;
	OF_DEPRECATED_MSG("Use getNumFrames instead",const uint64_t 		getNumSamples() const){return getNumFrames();}
	const bool 			& isCompressed() const;
	const bool 			& isLoaded() const;
	const std::string 	& getPath() const;
	bool          isStreaming() const;
	ofSoundBuffer&  getBuffer();
	const ofSoundBuffer&  getBuffer() const;
	
	ofEvent<void> loadAsyncEndEvent;
	
	friend std::ostream& operator<<(std::ostream& os, const ofxSoundFile& M);
protected:
	
	
private:
	class ThreadHelper : public ofThread{
	public:
		ThreadHelper(ofxSoundFile & sndFile):soundFile(sndFile){
			startThread();
		}
		~ThreadHelper(){
			if(isThreadRunning()) waitForThread(true);
		}
		virtual void threadedFunction() override;
		ofxSoundFile& soundFile;
	};
	void removeThreadHelperCB(ofEventArgs&);
	bool removeThreadHelper();
	
	shared_ptr<ThreadHelper> threadHelper = nullptr;
	
	
	bool loadFile( bool bAsync);
	
	ofSoundBuffer buffer;
	
	
	void setFromAudioFile(ofxAudioFile& audioFile);
	
	
	
	bool bCompressed;
	bool bLoaded;
	uint64_t duration = 0;
	unsigned int numChannels;
	unsigned int sampleRate;
	uint64_t numFrames;
	std::string path;
	
	std::unique_ptr<drwav> dr_wav_ptr = nullptr;
	void closeDrWavPtr();

	uint64_t currentStreamReadFrame = 0;
	
};
