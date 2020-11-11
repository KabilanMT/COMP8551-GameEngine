#pragma once

#include <Bass\bass.h>
#include "../logger.h"
#include <string>
#include "../Systems/AudioMixer.h"


using namespace std;
struct Sound {
    Sound(string name, bool ifLoop = false) : name(name), ifLoop(ifLoop) {}

    HSTREAM sound;
    string name; //path of the file
    string path = "";    //path to store the sound
    bool ifLoop; //set if loop the sound
    bool ifReverb = false; //set if reverb the sound
    float volume = -1; //volumn of the sound

    AudioMixer *audio;
 
   void audioMixerProperties() {
      //echo
       BASS_DX8_ECHO echo;
        echo.fWetDryMix = audio->echowet;
        echo.fFeedback = audio->echofeed;
        echo.fLeftDelay = audio->echoleft;
        echo.fRightDelay = audio->echoright;
        echo.lPanDelay = audio->echodelay;
        
        
        //distort
        BASS_DX8_DISTORTION distort;
        distort.fGain = audio->dgain;
        distort.fEdge = audio->dedge;
        distort.fPostEQCenterFrequency = audio->dEQcenter;
        distort.fPostEQBandwidth = audio->dEQbandwidth;
        distort.fPreLowpassCutoff = audio->dcutoff;

        //chorus
        BASS_DX8_CHORUS chorus;
        chorus.fWetDryMix = audio->cwetdry;
        chorus.fDepth = audio->cdepth;
        chorus.fFeedback = audio->cfeedback;
        chorus.fFrequency = audio->cfrequency;
        chorus.lWaveform = audio->cwave;
        chorus.fDelay = audio->cdelay;
        chorus.lPhase = audio->cphase;
 
        //flanger
        BASS_DX8_FLANGER flanger;
        flanger.fWetDryMix = audio->fwetdry;
        flanger.fDepth = audio->fdepth;
        flanger.fFeedback = audio->ffeed;
        flanger.fFrequency = audio->ffrequency;
        flanger.lWaveform = audio->fwave;
        flanger.fDelay = audio->fdelay;
        flanger.lPhase = audio->fphase;

        //gargle
        BASS_DX8_GARGLE gargle;
        gargle.dwRateHz = audio->grate;
        gargle.dwWaveShape = audio->gwaveshape;

        //compression
        BASS_DX8_COMPRESSOR compress;
        compress.fGain = audio->comgain;
        compress.fAttack = audio->comatk;
        compress.fRelease = audio->comrelease;
        compress.fThreshold = audio->comthreshold;
        compress.fRatio = audio->comratio;
        compress.fPredelay = audio->comdelay;
        
    }

    void setUpSound() {
        string fullpath = path + name;
        if (!(sound = BASS_StreamCreateFile(false, fullpath.c_str(), 0, 0, BASS_SAMPLE_MONO))) {
            Logger::getInstance() << "Load error: " << BASS_ErrorGetCode() << " \n";
        }
        if (ifLoop == true) {
            setIfLoop(ifLoop);
        }
        BASS_ChannelGetAttribute(sound, BASS_ATTRIB_VOL, &volume);
    }

    void play() {
        if (!BASS_ChannelPlay(sound, FALSE)) {
            Logger::getInstance() << "Play error: " << BASS_ErrorGetCode() << " \n";
        }
    }

    void pause() {
        if (!BASS_ChannelPause(sound)) {
            Logger::getInstance() << "Play error: " << BASS_ErrorGetCode() << " \n";
        }
    }

    void stop() {
        if (!BASS_ChannelStop(sound)) {
            Logger::getInstance() << "Play error: " << BASS_ErrorGetCode() << " \n";
        }
    }

    void setIfLoop(bool loop) {
        ifLoop = loop;

        if (BASS_ChannelFlags(sound, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP) == -1) {
            Logger::getInstance() << "Play error: " << BASS_ErrorGetCode() << " \n";
        }
    }

    void setVolumn(float vol) {
        volume = vol;
        BASS_ChannelSetAttribute(sound, BASS_ATTRIB_VOL, volume);
    }

    void cleanUp() {
        BASS_StreamFree(sound);

    }
};