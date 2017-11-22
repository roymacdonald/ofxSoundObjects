/*
 * ofxSoundFile.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofConstants.h"
#include "ofSoundBuffer.h"
#include <sndfile.hh>

bool ofxLoadSound(ofSoundBuffer &buffer, string path);
bool ofxSaveSound(const ofSoundBuffer &buffer, string path);

class ofxSoundFile {
public:
	ofxSoundFile();
    
	ofxSoundFile(string path);

	virtual ~ofxSoundFile();
	void close();

	/// opens a sound file for reading with readTo().
	bool load(string _path);
	

	bool save(string _path, const ofSoundBuffer &buffer);

	/// reads a file into an ofSoundBuffer.
	/// by default, this will resize the buffer to fit the entire file.
	/// supplying a "samples" argument will read only the given number of samples
	bool readTo(ofSoundBuffer &buffer, long long samples = 0);
	
	/// seek to the sample at the requested index
	bool seekTo(long long sampleIndex);

	/// returns sound file duration in milliseconds
	const unsigned long getDuration() const;
	const int getNumChannels() const;
	const int getSampleRate() const;
	const unsigned long getNumSamples() const;
	//const bool isCompressed() const;
	const bool isLoaded() const;
	const string getPath() const;

private:
	
	SndfileHandle sndFileHandle;
	
	double scale;
	sf_count_t samples_read;

	float duration; //in secs
	string path;

};

