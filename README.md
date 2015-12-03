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



