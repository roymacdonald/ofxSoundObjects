ofxSoundObjects
=====================================

This is an addon version for the ofSoundObject implementation originally conceived at the 2013 OFDev Conference held at the Yamaguchi Center For Arts and Media, Japan.

The original development branch can be found [here](https://github.com/admsyn/openFrameworks/tree/feature-sound-objects).


##Explanation
####The ofxSoundObject
* Is a class for working with audio.
* It has an input and and output.
* By itself does nothing, it will just passthrough audio, yet it will buffer the audio data instead of passing it straight out. 
* It is intended to be extended uppon to become useful.
* Any two ofxAudioObject inherited classes can connect between them. 
* ofxSoundInput and ofxSoundOutput classes inherit from ofxSoundObject.

###Usage

Say you have two instances from classes that inherit from ofxSoundObject, say
`soundObject1` and `soundObject2;`
when you say `soundObject1.connectTo(soundObject2)` it means that the audio data from  soundObject1 (and processed if so) is sent soundObject2, so the latter processes it and sends it to whatever it is connected to.



You will always need an instance of ofSoundStream to be able to get or send audio data to the audio interface.

##Examples
####AudioInput
This will take the input audio data from the sound card, plot it in the display and send it to the output.
Notice that the waveformDraw class uses a mutex for avoiding crashes as different threads try to access and modify the object audio data.

WARNING: This example can create an extreme acoustic sound feedback, so be careful with the sound levels before running it.


####AudioOutput
This will generate a sine wave and output it to the sound output.
The sine wave's parameters ar controlled via mouse position;

####SoundObjects
This class has a NoiseGenerator, a LoPassFilter and a DigitalDelay instances, all of which inherit from ofxSoundObject. Their names are quite self explaning.
Chech how these are connected to each other
`noise.connectTo(filter).connectTo(delay).connectTo(output);`

which means that noise is generated, the filtered by the loPassfilter, then delay (echo) is applied to finally being sent to the sound output.

####SoundPlayerObject
This makes use of the ofxBasicSoundPlayer, that is a sound file player that extends ofxSoundObject, which is not so basic at all, thus you can connect it to any other object and process and do whatever you might want to do with the playedback sound data. See below a more thorough explanation of this sound player. When starting this example a file dialog will pop up in which you should select an audio file to be played back.


####SoundMixer
It makes use of the ofxSoundMixer object. It loads some audiofiles and plays them back. There's a gui in which you can modify the volume of each mixer channel.

##Included soundObjects
The following are the classes that inherit from ofxSoundObject that are included in this addon. Use this as guidelines for implementing your own.

####DigitalDelay
It will delay sound and put it into a feedback loop creating an "echo" kind of effect. It is a really common effect for sound manipulation.Parameters available:
* **Delay**: The amount of samples that will be delayed, or how much time (samples) will be between each echo repetition.* **Mix**: The mix amount of original sound with the delayed/processed sound. 
* **Feedback**: How much of the delayed sound is feedbacked. This is also the amount of repetitions. 0 means no repetitions. 1 means infinite repetitions. 


This object:

* Intakes sound.
* Processes sound.
* Outputs sound.
####LowPassFilter.h
This is a basic kind of equalizer. It will only let pass through sounds that are below a certain frequency, which is user defined.Parameters available:
* **Cutoff**: this is the limit frequency.* **Resonance**: amount of emphasis applied to frequencies that are just before the cutoff frequency.
This object:

* Intakes sound.
* processes sound.
* Outputs sound.####NoiseGenerator.h
This will create white-ish noise. It will just put random valued samples into the buffer.

**No parameters available.**

This object:

* DOES NOT intake sound.
* Generates sound.
* Outputs sound.
####SineWaveGenerator.h
This will create a sine wave signal and place it into the buffer.

Parameters available:
* **Frequency**: The sine wave signal frequency* **Amplitude**: How "loud" this signal will be.
* **Phase**: The amount of offset that with signal will have. 


This object:

* DOES NOT intake sound.
* Generates sound.
* Outputs sound.
####waveformDraw.h
This draws the waveform of the sound being input to it.
It inherits from ofRectangle which is used for the drawing area.
Parameters available:

* **x, y, width and height:** cordinates and size of the area where this will get drawn. 


This object:

* Intakes sound.
* DOES NOT generates or process sound. It passes through.
* Outputs sound.

##Other classes included this addon
The following are some useful classes included with this addon.

####ofxSoundFile
This class provides access to sound data from a variety of sound file formats. It supports the most common ones like .mp3, .acc, .aiff and .wav on all systems, plus several others depending on the system. It makes use of [libaudiodecoder](http://www.oscillicious.com/libaudiodecoder) for MacOsX, iOS and Windows and [libsndfile](http://www.mega-nerd.com/libsndfile/) for Linux.
This class will decode data and make it accessible using the ofSoundBuffer instances.
It provides basic 16bit wav sound file writing.


####ofxBasicSoundPlayer
This is a completely implemented sound player. It makes use of ofxSoundFile to access sound files. It will deal with passing the required data to the sound stream, interpolating it if necesary and provide all the common sound player functionalities. It is not basic at all but nobody came with a better name when develiping it.

####ofxSoundMixer

This will allow you to mix the sound from several ofxSoundObjects connected to it.
It inherits from ofxSoundObject so it is completely compatible with other ofxSoundObjects. 
It is really simple. Just allows volume adjustment per channel but you can extend it to make a really fancy sound mixer, just limited by your computer capabilities (not completely sure but thereticaly it should be true).




