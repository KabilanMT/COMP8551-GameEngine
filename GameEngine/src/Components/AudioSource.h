#pragma once

#include <Bass\bass.h>
#include "Sound.h"
#include <string>
#include "../logger.h"

using namespace std;

struct AudioSource {
	AudioSource(string name, bool playOnStart = true, bool ifLoop = false, string tag = "") : ifLoop(ifLoop), name(name), tag(tag), playOnStart(playOnStart) {
		initSound();
	}
	~AudioSource() {
		cleanUp();
		tag == "";
	}

	Sound* sound;
	string name;
	float volume = 1;
	string tag;
	bool playOnStart;
	bool ifLoop;

	void setVolume(float vol) {
		volume = vol;
	}

	bool getIfLoop() {
		return sound->ifLoop;
	}

	void setifLoop(bool loop) {
		sound->setIfLoop(loop);
	}

	string getTag() {
		return tag;
	}

	void setTag(string t) {
		tag = t;
	}

	void cleanUp() {
		sound->cleanUp();
		delete sound;
	}

	void initSound() {
		sound = new Sound(name, ifLoop);
		sound->setUpSound();
	}
};