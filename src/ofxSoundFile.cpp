/*
 * ofxSoundFile.cpp
 *
 *  Created on: 25/07/2012
 *      Author: arturo
 */

#include "ofxSoundFile.h"
#include "ofLog.h"
#include "ofUtils.h"

#include "dr_wav.h"

using namespace std;
//--------------------------------------------------------------
bool ofxLoadSound(ofSoundBuffer &buff, string path){
	ofxSoundFile sf(path);
	if(sf.isLoaded()){
		buff = sf.getBuffer();
		//        sf.readTo(buff);
		return true;
	}else{
		return false;
	}
}

//--------------------------------------------------------------
// for now this only write 16 bit PCM WAV files.
// It can't really live in ofxSoundFile yet because
// ofxSoundFile doesn't hold a complete representation
// the sound file that can be written to disk. You'd
// need something that would let you stream the data to
// it via writeTo() or similar. Doesn't really fit wtih
// the current model.
bool ofxSaveSound(const ofSoundBuffer &buff,  string path){
	ofxSoundFile soundFile;
	return soundFile.save(path, buff);
}


//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile() {	
	reset();
}
//--------------------------------------------------------------
ofxSoundFile::~ofxSoundFile() {
	closeDrWavPtr();
}
//--------------------------------------------------------------
void ofxSoundFile::ThreadHelper::threadedFunction(){
	if(isThreadRunning()){
		ofAddListener(ofEvents().update, &soundFile, &ofxSoundFile::removeThreadHelperCB);
		soundFile.loadFile(true);
	}
}
//--------------------------------------------------------------
bool ofxSoundFile::removeThreadHelper(){
	if(threadHelper && !threadHelper->isThreadRunning()){
		ofRemoveListener(ofEvents().update, this, &ofxSoundFile::removeThreadHelperCB);
		threadHelper.reset();
		threadHelper = nullptr;
		return true;
	}
	return false;
}
//--------------------------------------------------------------
void ofxSoundFile::removeThreadHelperCB(ofEventArgs&){
	if(removeThreadHelper()){
		ofNotifyEvent(loadAsyncEndEvent);
	}
}
//--------------------------------------------------------------
ofxSoundFile::ofxSoundFile(string path) {
	ofxSoundFile();
	load(path);
}
//--------------------------------------------------------------
void ofxSoundFile::setFromAudioFile(ofxAudioFile& audiofile){
	if(!audiofile.loaded()){
		return;
	}
	auto tempPath =  path;
	reset();
	numFrames = audiofile.length();
	bLoaded = numFrames > 0;
	if(bLoaded){
		numChannels = audiofile.channels();
		sampleRate =  audiofile.samplerate();
		
		
		buffer.resize(numFrames*numChannels);
		buffer.copyFrom(audiofile.data(), numFrames, numChannels, sampleRate);
		
		audiofile.free();
		
		path = tempPath;
		
		duration = 1000* float(numFrames) / float(sampleRate);
		
		bCompressed = (ofFilePath::getFileExt(ofToLower(path)) == "mp3");
		
		ofLogVerbose("ofxSoundFile::load") << "file loaded. is mp3 : " << (bCompressed?"YES":"NO");
	}else{
		ofLogWarning("ofxSoundFile::load") << "file loaded has zero length ";
	}
}
//--------------------------------------------------------------
bool ofxSoundFile::loadFile( bool bAsync){
	if(!bAsync && threadHelper != nullptr){
		ofLogError("ofxSoundFile::load") << "Can not load a file while another is loading";
		return false;
	}
	if(isLoaded())reset();
	
	if( ofFile::doesFileExist( path ) ){
		closeDrWavPtr();
		
		ofxAudioFile audiofile; 
		audiofile.setVerbose(ofGetLogLevel() == OF_LOG_VERBOSE);
		audiofile.load( path );
		bool bL = audiofile.loaded();
		if (!bL){
			ofLogError("ofxSoundFile::load")<<"error loading file, double check the file path";
			return false;
		}
		
		if(bAsync){
			static ofMutex mutex;
			std::lock_guard<std::mutex> lck (mutex);
			setFromAudioFile(audiofile);
		}else{
			setFromAudioFile(audiofile);
		}
		
	}else{
		ofLogError()<<"input file does not exists";
	}
	
	return bLoaded;   
	
}
//--------------------------------------------------------------
void ofxSoundFile::closeDrWavPtr(){
	if(dr_wav_ptr != nullptr){
		drwav_uninit(dr_wav_ptr.get());
		dr_wav_ptr.reset();
		dr_wav_ptr = nullptr;
	}
}
//--------------------------------------------------------------
bool ofxSoundFile::openFileStream(std::string filepath){
	if(ofFilePath::getFileExt(ofToLower(filepath)) != "wav"){
		ofLogError("ofxSoundFile::openFileStream") << "files for streaming have to be of uncompressed type.";
		return false;
	}
	
	reset();
	
	dr_wav_ptr = std::make_unique<drwav>();
	if (!drwav_init_file_ex(dr_wav_ptr.get(), ofToDataPath(filepath, true).c_str(), NULL, NULL, DRWAV_SEQUENTIAL)) {
		closeDrWavPtr();
		ofLogVerbose("ofxSoundFile::openFileStream") << "failed opening file for streaming.";
		return false;
	}
	
	numFrames = dr_wav_ptr->totalPCMFrameCount;
	bLoaded = numFrames > 0;
	if(bLoaded){
		cout << "ofxSoundFile::openFileStream" << endl;
		sampleRate = dr_wav_ptr->sampleRate;
		numChannels = dr_wav_ptr->channels;
		
		path = filepath;
		
		duration = 1000* float(numFrames) / float(sampleRate);
		
		bCompressed = false;
		bLoaded = true;
		
		
		cout << "sampleRate: " << sampleRate << endl;
		cout << "numChannels : " << numChannels  << endl;
		cout << "path : " << path  << endl;
		cout << "duration : " << duration  << endl;
		cout << "bCompressed : " << bCompressed  << endl;
		cout << "bLoaded : " << bLoaded  << endl;
		
	}
	return bLoaded;
}
//--------------------------------------------------------------
bool ofxSoundFile::loadAsync(std::string filepath){
	if(threadHelper == nullptr){
		path = filepath;

		threadHelper = make_shared<ThreadHelper>(*this);
		
		return true;
	}
	return false;
}
//--------------------------------------------------------------
bool ofxSoundFile::load(string filepath){
	path = filepath;
	return loadFile(false);
}

//--------------------------------------------------------------
bool ofxSoundFile::save(string path, const ofSoundBuffer &buff){
	// check that we're writing a wav and complain if the file extension is wrong.
	ofFile f(path);
	if(ofToLower(f.getExtension())!="wav") {
		path += ".wav";
		ofLogWarning() << "Can only write wav files - will save file as " << path;
	}
	{
		fstream file(ofToDataPath(path).c_str(), ios::out | ios::binary);
		if(!file.is_open()) {
			ofLogError() << "Error opening sound file '" << path << "' for writing";
			return false;
		}
	}
	
	
	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
	format.channels = buff.getNumChannels();
	format.sampleRate = buff.getSampleRate();
	format.bitsPerSample = 32;
	
	
	drwav* wav_handle = NULL;
	wav_handle = drwav_open_file_write( path.c_str(), &format);

	if( wav_handle != NULL){
		drwav_uint64 samplesWritten = drwav_write_pcm_frames(wav_handle, buff.getNumFrames(), buff.getBuffer().data());
		if(samplesWritten != buff.getNumFrames()){
			ofLogWarning("ofxSoundFile::save") << "samplesWritten " << samplesWritten << " not the same as the passed buffer " <<  buff.getNumFrames() << endl;
		}
		drwav_uninit(wav_handle);
		wav_handle  = NULL;
		
	}
	
	return true;
}


//--------------------------------------------------------------                  
//--------------------------------------------------------------
void ofxSoundFile::reset(){
	closeDrWavPtr();
	removeThreadHelper();
	buffer.clear();
	bCompressed = false;
	bLoaded = false;
	duration = 0;
	numChannels = 0;
	sampleRate = 0;
	numFrames = 0;
	path = "";
	currentStreamReadFrame = 0;
}
//--------------------------------------------------------------
void ofxSoundFile::close(){
	reset();
}
//--------------------------------------------------------------
const bool & ofxSoundFile::isLoaded() const{
	return bLoaded;
}
//--------------------------------------------------------------
const unsigned int & ofxSoundFile::getNumChannels() const{
	return numChannels; 
}
//--------------------------------------------------------------
const uint64_t & ofxSoundFile::getDuration() const{
	return duration;
}
//--------------------------------------------------------------
const unsigned int & ofxSoundFile::getSampleRate() const{
	return sampleRate;
}
//--------------------------------------------------------------
const uint64_t & ofxSoundFile::getNumFrames() const{
	return numFrames;
}
//--------------------------------------------------------------
const bool & ofxSoundFile::isCompressed() const{
	return bCompressed;
}
//--------------------------------------------------------------
const string & ofxSoundFile::getPath() const{
	return path;
}
//--------------------------------------------------------------
ofSoundBuffer&  ofxSoundFile::getBuffer(){
	return buffer;
}
//--------------------------------------------------------------
const ofSoundBuffer&  ofxSoundFile::getBuffer() const{
	return buffer;
}
//--------------------------------------------------------------
 bool ofxSoundFile::isStreaming() const{
	return (dr_wav_ptr != nullptr);
}
//--------------------------------------------------------------
bool ofxSoundFile::seekToFrame(uint64_t frame){
	if(!isStreaming())return false;
	
	if(drwav_seek_to_pcm_frame(dr_wav_ptr.get(), frame)){
		currentStreamReadFrame = frame;
		return true;
	}
	return false;
}
//--------------------------------------------------------------
size_t ofxSoundFile::readTo(ofSoundBuffer & buf, std::size_t outNumFrames, std::size_t fromFrame , bool bLoop ){
	if(isLoaded()){
		if (isStreaming()){
			
			buf.resize(outNumFrames * getNumChannels());
			buf.setNumChannels(getNumChannels());
			buf.setSampleRate(sampleRate);
			
			
			if(currentStreamReadFrame != fromFrame){
				if(bLoop) fromFrame %= numFrames;
				if(!seekToFrame(fromFrame)){return 0;}
			}
			size_t readFrames = drwav_read_pcm_frames_f32(dr_wav_ptr.get(), outNumFrames, buf.getBuffer().data());
			
			if(bLoop && readFrames < outNumFrames && readFrames > 0){
				seekToFrame(0);
				readFrames += drwav_read_pcm_frames_f32(dr_wav_ptr.get(), outNumFrames - readFrames , &buf.getBuffer()[readFrames]);
			}
			
			currentStreamReadFrame += readFrames;
			
			return readFrames;
		}else{
			if(outNumFrames!=0){
				// will read the requested number of samples
				// clamp to the number of samples we actually have
				outNumFrames = std::min(outNumFrames, static_cast<size_t>(getNumFrames()));
			}else{
				outNumFrames = getNumFrames();
			}
			
			buffer.copyTo(buf, outNumFrames, getNumChannels(), fromFrame, bLoop);
			
			return outNumFrames;
		}
	}
	return 0;
}
//--------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const ofxSoundFile& f){
	os << ofFilePath::getBaseName(f.getPath())  << std::endl;
	os << "  Duration    " << f.getDuration() << std::endl;
	os << "  NumChannels " << f.getNumChannels() << std::endl;
	os << "  SampleRate  " << f.getSampleRate() << std::endl;
	os << "  NumSamples  " << f.getNumFrames();
	return os;
}
