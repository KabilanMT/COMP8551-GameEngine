#pragma once

#include "entityx/entityx.h"

using namespace std;

namespace AudioCommandType {
    enum Type {
        PLAY,
        PAUSE,
        STOP,
        NUM_AUDIO_COMMAND_TYPES
    };
}

struct AudioCommand : entityx::Event<AudioCommand> {
  AudioCommand(entityx::Entity& e, AudioCommandType::Type t) : entityWithAudioSource(e), type(t) {}

  entityx::Entity& entityWithAudioSource;
  AudioCommandType::Type type;
};