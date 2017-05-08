/*
 * ofxSoundFile.h
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#pragma once

#include "ofConstants.h"
#include "ofSoundBuffer.h"

#if defined (TARGET_OSX) || defined (TARGET_WIN32) || defined (TARGET_OS_IPHONE)
	#define OF_USING_LAD // libaudiodecoder
#elif defined (TARGET_LINUX)
	#define OF_USING_SNDFILE // libsndfile
#endif

#if defined (OF_USING_SNDFILE)
	#include <sndfile.h>
#elif defined (OF_USING_LAD)
    #include "audiodecoder.h"
#endif
#if defined (OF_USING_MPG123)
	#include <mpg123.h>
#endif

/// reads a sound file into an ofSoundBuffer.
/// encoding support varies by platform.
/// Windows and OSX use libaudiodecoder for decoding, Linux uses libsndfile
bool ofxLoadSound(ofSoundBuffer &buffer, string path);

/// writes an ofSoundBuffer as a 16-bit PCM WAV file.
bool ofxSaveSound(const ofSoundBuffer &buffer, string path);

class ofxSoundFile {
public:
	ofxSoundFile();
	ofxSoundFile(string path);

	virtual ~ofxSoundFile();
	void close();

	/// opens a sound file for reading with readTo().
	/// encoding support varies by platform.
	/// Windows and OSX use libaudiodecoder for decoding, Linux uses libsndfile
	bool load(string _path);
	
	/// writes an ofSoundBuffer as a 16-bit PCM WAV file
	bool save(string _path, const ofSoundBuffer &buffer);

	/// reads a file into an ofSoundBuffer.
	/// by default, this will resize the buffer to fit the entire file.
	/// supplying a "samples" argument will read only the given number of samples
	bool readTo(ofSoundBuffer &buffer, unsigned int samples = 0);
	
	/// seek to the sample at the requested index
	bool seekTo(unsigned int sampleIndex);

	/// returns sound file duration in milliseconds
	const unsigned long getDuration() const;
	const int getNumChannels() const;
	const int getSampleRate() const;
	const unsigned long getNumSamples() const;
	const bool isCompressed() const;
	const bool isLoaded() const;
	const string getPath() const;

private:
	
	bool sfReadFile(ofSoundBuffer & buffer);
	bool mpg123ReadFile(ofSoundBuffer & buffer);
	bool ladReadFile(ofSoundBuffer & buffer);
	
	bool sfOpen(string path);
	bool mpg123Open(string path);
	bool ladOpen(string path);
	
	void initDecodeLib();
	
#ifdef OF_USING_SNDFILE
	// soundfilelib info
	SNDFILE* sndFile;
	int subformat;
	double scale;
	sf_count_t samples_read;
#endif
	
#ifdef OF_USING_MPG123
	// mpg123 info
	mpg123_handle * mp3File;
	static bool mpg123Inited;
#endif
	
#ifdef OF_USING_LAD
	AudioDecoder* audioDecoder;
#endif

	// common info
	int channels;
	float duration; //in secs
	unsigned int samples;
	int samplerate;
	int bitDepth;
	string path;
	bool bCompressed;
	bool bLoaded;
};

