#pragma once

#include <Bass\bass.h>
#include "Sound.h"
#include <string>
#include "../logger.h"

using namespace std;

struct AudioSource {
	AudioSource(string name, bool playOnStart = true, bool ifLoop = false, string tag = "") : sound{new Sound(name, ifLoop)}, name(name), tag(tag), playOnStart(playOnStart) {
		sound->setUpSound();
	}
	~AudioSource() {
		cleanUp();
	}

	Sound* sound;
	string name;
	float volume = 1;
	string tag;
	bool playOnStart;

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
		tag == "";
	}
};