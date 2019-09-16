# ofxSoundObjects
## Simple yet super powerful modular sound architecture for openFrameworks.
It allows to connect any sound source, process, display and output it in any way needed. It is capable of reading and writing audio files. 

This is an addon version for the ofSoundObject implementation originally conceived at the 2013 OFDev Conference held at the Yamaguchi Center For Arts and Media, Japan.
The developers involved in this were:

* [Adam Carlucci](https://github.com/admsyn/)
* [Marek Bareza](https://github.com/mazbox)
* [Arturo Castro](https://github.com/arturoc)
* [Roy Macdonald](https://github.com/roymacdonald/)


The original development branch can be found [here](https://github.com/admsyn/openFrameworks/tree/feature-sound-objects).

Made as addon, improoved and updated by Roy Macdonald.


## Continous Integration (CI) builds status
Platform                     | CI Status
-----------------------------|:---------
OSX                          | [![OSX Build Status](http://badges.herokuapp.com/travis/roymacdonald/ofxSoundObjects?env=BADGE=osx&label=build&branch=master)](https://travis-ci.org/roymacdonald/ofxSoundObjects)
Linux  64bit                 | [![Linux Build Status](http://badges.herokuapp.com/travis/roymacdonald/ofxSoundObjects?env=BADGE=linux&label=build&branch=master)](https://travis-ci.org/roymacdonald/ofxSoundObjects)
Windows MSYS2 32bits         | [![Build status](https://appveyor-matrix-badges.herokuapp.com/repos/roymacdonald/ofxsoundobjects/branch/master/1)](https://ci.appveyor.com/project/roymacdonald/ofxsoundobjects/branch/master)
Windows Visual Studio 32bits | [![Build status](https://appveyor-matrix-badges.herokuapp.com/repos/roymacdonald/ofxsoundobjects/branch/master/2)](https://ci.appveyor.com/project/roymacdonald/ofxsoundobjects/branch/master)
Windows Visual Studio 64bits | [![Build status](https://appveyor-matrix-badges.herokuapp.com/repos/roymacdonald/ofxsoundobjects/branch/master/3)](https://ci.appveyor.com/project/roymacdonald/ofxsoundobjects/branch/master)


## Dependencies
* [ofxAudioFile](https://github.com/npisanti/ofxAudioFile)
* [ofxFft](https://github.com/kylemcdonald/ofxFft) (only needed for the Fftobject)

### optional
* [ofxTbb](https://github.com/roymacdonald/ofxTbb) (only needed if you enable multithreading, which is disabled by default)
* [ofxNDI](https://github.com/nariakiiwatani/ofxNDI) (only needed for using ofxNDI. read [here](#ofxndi) for details.

## IMPORTANT.
### All platforms

#### LibAudioDecoder replaced by ofxAudioFile
This addon no longer uses libAudioDecoder, instead uses [ofxAudioFile](https://github.com/npisanti/ofxAudioFile). Make sure you have downloaded and correctly instaled it.


### Windows 
#### Debug mode does not sound on Windows
It seems that there is a bug in RtAudio (the library that provides OF with cross platform access to the sound devices) that disables audio output when running in debug mode. So, you'll need to switch to Release mode always.

#### VisualStudio 2017
Removed the dependency on libsndfile so there is no more need to add the props file to visualStudio.

## Multithreading
If you want to use multithreading, currently only implemented in the matrix mixer object, soon should be in the sound mixer too, you will need to use [ofxTbb](https://github.com/roymacdonald/ofxTbb) and in file `ofxSoundObjectsConstants.h` uncomment the line that reads

```
#define OFX_SOUND_ENABLE_MULTITHREADING
```

Carefully read the instructions in ofxTbb readme file and add ofxTbb to your project in Project Generator.


## Project Generator
Create or update your project using openFrameworks' Project Generator and add `ofxSoundObjects`, `ofxGui` and `ofxAudioFile` in the addons section. 


## Explanation
#### The ofxSoundObject
* Is a class for working with audio.
* It has an input and and output.
* By itself does nothing, it will just passthrough audio, yet it will buffer the audio data instead of passing it straight out. 
* It is intended to be extended uppon to become useful.
* Any two ofxSoundObject inherited classes can connect between them. 


### Usage

When inheriting from ofxSoundObject the only function to override is 

    void process(ofSoundBuffer &input, ofSoundBuffer &output)

When you have two instances from classes that inherit from ofxSoundObject, `soundObject1` and `soundObject2;`
when you say 
    
    soundObject1.connectTo(soundObject2);

it means that the audio data from  soundObject1 (and processed if so) is sent soundObject2, so the latter processes it and sends it to whatever it is connected to.

You can connect an ofxSoundObject directly to an ofSoundStream output, so your sound goes out to your sound device.

    soundStream.setOutput(soundObject);

To get sound input from your audio device you need to make an instance of ofxSoundInput, set it as the input for the sound stream and connect it to the rest of the sound objects. 

    soundStream.setInput(soundInput);
    soundInput.connectTo(whateverElseSoundObject);

The `connectTo` method returns a reference to the object being connected to, which allows to connect any ammount of objects on a single declaration.

    soundObject1.connectTo(soundObject2).connectTo(soundObject3).connectTo(soundObject3).connectTo(soundObject4);

When you want to draw the sound buffer data somehow make sure you use an ofMutex to avoid threads colliding. Check the waveformDraw included sound object and use as guideline.


## Examples
#### example-AudioInput
This will take the input audio data from the sound card, plot it in the display and send it to the output.
Notice that the waveformDraw class uses a mutex for avoiding crashes as different threads try to access and modify the object audio data.

WARNING: This example can create an extreme acoustic sound feedback, so be careful with the sound levels before running it.


#### example-AudioOutput
This will generate a sine wave and output it to the sound output.
The sine wave's parameters ar controlled via mouse position;

#### example-SoundObjects
This class has a NoiseGenerator, a LoPassFilter and a DigitalDelay instances, all of which inherit from ofxSoundObject. Their names are quite self explaning.
Chech how these are connected to each other
     
     noise.connectTo(filter).connectTo(delay).connectTo(output);

which means that noise is generated, the filtered by the loPassfilter, then delay (echo) is applied to finally being sent to the sound output.

#### example-SoundPlayerObject
This makes use of ofxSoundPlayerObject, that is a sound file player that extends ofxSoundObject, thus you can connect it to any other object and process and do whatever you might want to do with the playedback sound data. See below a more thorough explanation of this sound player. When starting this example a file dialog will pop up in which you should select an audio file to be played back.


#### example-SoundMixer
It makes use of the ofxSoundMixer object. It loads some audiofiles and plays them back. There's a gui in which you can modify the volume of each mixer channel.


#### example-ofxFft
it uses [ofxFft](https://github.com/kylemcdonald/ofxFft) to perform a Fast Fourier Transform and visualize it. You can change the input of it in runtime, choosing from a audio file playback, sine wave generator or live input.


#### example-matrixMixer
This examples shows how to use the matrix mixer. A matrix mixer is a special kind of mixer that allows you to mix any input into any output, allowing for complex mix scenarios or rerouting audio channels. This becomes very useful when using audio interfaces with lots of outputs. 


### Removed examples
I decided to remove the following examples because I removed the ofxSoundMultiplexer class. The functionality of these two is now achieved using the matrix mixer.

* example-soundPlayerMultiOutput
* example-multiInputMixdown




## Included soundObjects
The following are the classes that inherit from ofxSoundObject that are included in this addon. Use this as guidelines for implementing your own.

#### DigitalDelay

It will delay sound and put it into a feedback loop creating an "echo" kind of effect. It is a really common effect for sound manipulation.

Parameters available:

* **Delay**: The amount of samples that will be delayed, or how much time (samples) will be between each echo repetition.
* **Mix**: The mix amount of original sound with the delayed/processed sound. 
* **Feedback**: How much of the delayed sound is feedbacked. This is also the amount of repetitions. 0 means no repetitions. 1 means infinite repetitions. 


This object:

* Intakes sound.
* Processes sound.
* Outputs sound.


#### LowPassFilter.h

This is a basic kind of equalizer. It will only let pass through sounds that are below a certain frequency, which is user defined.

Parameters available:

* **Cutoff**: this is the limit frequency.
* **Resonance**: amount of emphasis applied to frequencies that are just before the cutoff frequency.


This object:

* Intakes sound.
* processes sound.
* Outputs sound.

#### NoiseGenerator.h

This will create white-ish noise. It will just put random valued samples into the buffer.

**No parameters available.**

This object:

* DOES NOT intake sound.
* Generates sound.
* Outputs sound.

#### SineWaveGenerator.h

This will create a sine wave signal and place it into the buffer.

Parameters available:

* **Frequency**: The sine wave signal frequency
* **Amplitude**: How "loud" this signal will be.
* **Phase**: The amount of offset that with signal will have. 


This object:

* DOES NOT intake sound.
* Generates sound.
* Outputs sound.


#### waveformDraw.h

This draws the waveform of the sound being input to it.
It inherits from ofRectangle which is used for the drawing area.
Parameters available:

* **x, y, width and height:** cordinates and size of the area where this will get drawn. 


This object:

* Intakes sound.
* DOES NOT generates or process sound. It passes through.
* Outputs sound.

## Other classes included this addon
The following are some useful classes included with this addon.

#### ofxSoundFile

This class provides access to sound data from a variety of sound file formats. It supports the most common ones like .mp3, .aiff, .ogg and .wav on all systems, plus several others depending on the system. It makes use of [ofxAudioFile](https://github.com/npisanti/ofxAudioFile) for reading files and writting files.

This class will decode data and make it accessible using the ofSoundBuffer instances.
Currently all the loaded files data is automatically converted into 32bit floating point regardless of its original bitdepth.
It provides 8, 16, 24 and 32bit WAV sound file writing.


#### ofxSoundPlayerObject

This is a completely implemented sound player. It makes use of ofxSoundFile to access sound files. It will deal with passing the required data to the sound stream, interpolating it if necesary and provide all the common sound player functionalities. It allows multiple instances of playback of the same soundfile, each acting independently from the others.

#### ofxSoundMixer

This will allow you to mix the sound from several ofxSoundObjects connected to it.
It inherits from ofxSoundObject so it is completely compatible with other ofxSoundObjects. 
It is really simple. Just allows volume adjustment per channel but you can extend it to make a really fancy sound mixer, just limited by your computer capabilities (not completely sure but thereticaly it should be true).

#### ofxSoundInput

You need to use this to connect the sound device input to a soundObject

#### ofxSoundMatrixMixer
This class implements a matrix mixer, which is a special kind of mixer that allows you to mix any input into any output, allowing for complex mix scenarios or rerouting audio channels. This becomes very useful when using audio interfaces with lots of outputs. By itself it does not have a gui but the `ofxSoundMatrixMixerRenderer` class implements one. The `example-matrixMixer` shows how to use both of these.


#### ofxNDI

There is a sound object specifically made for using ofxNDI to allow you to send and receive audio from other computers over a network.
There are two classes made for this `ofxNDISenderSoundObject` and `ofxNDIReceiverSoundObject`. Look at the examples on how to use these.

**This feature is disabled by default so you'll need to activate it.**
Do the following in order to activate the use of ofxNDI

Go to the file `src/ofxSoundObjectsConstants.h`, uncomment the line that reads `//#define OFX_SOUND_OBJECTS_USE_OFX_NDI` and save;
	
You'll also need to download and properly install in your addons [ofxNDI](https://github.com/nariakiiwatani/ofxNDI) add it to your project via Project Generator and then do the following in case you're using xcode.

Once you've created your new project copy the `config.make` file from either `ofxSoundObjects/example-ofxNDISender/config.make` or `ofxSoundObjects/example-ofxNDIReceiver/config.make` into your newly created project. Now update your project with project generator. You only need to do this when creating the project. 



