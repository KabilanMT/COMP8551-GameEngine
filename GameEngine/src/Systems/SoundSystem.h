#pragma once
 
#include "entityx/entityx.h"
#include "../Components/Sound.h"
#include "../Events/Events.h"

#include "AudioMixer.h"
#include <Windows.h>
#include "../Components/AudioSource.h"
#include <Bass\bass.h>

using namespace entityx;

class SoundSystem : public System<SoundSystem>, public Receiver<SoundSystem> {
    public:
        AudioMixer *audiomix;

        void configure(EventManager& events) override {
            events.subscribe<ScenePreLoad>(*this);
            events.subscribe<AudioCommand>(*this);
        }

        //this method will be called right before the scene loads
        void receive(const ScenePreLoad& sl) {

            if (!initialized) {
                initSystem();
            }

            newScene = true;
        }

        void receive(const AudioCommand& comm) {
            //error checking
            if (!comm.entityWithAudioSource.valid())
                return;
            if (!comm.entityWithAudioSource.has_component<AudioSource>())
                return;

            ComponentHandle<AudioSource> handle = comm.entityWithAudioSource.component<AudioSource>();
            switch(comm.type) {
                case AudioCommandType::PLAY:
                    handle->sound->play();
                    break;
                case AudioCommandType::PAUSE:
                    handle->sound->pause();
                    break;
                case AudioCommandType::STOP:
                    handle->sound->stop();
                    break;
            }
        }
 
        void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
            //update loop

            auto entities = es.entities_with_components<AudioSource>();
            if (newScene) {
                newScene = false;
                for (Entity e : entities) {
                    ComponentHandle<AudioSource> handle = e.component<AudioSource>();
                    if (handle->playOnStart)
                        handle->sound->play();

                    //audiomix->Setchorus(handle->sound->sound, true);
                    //audiomix->chorusproperties(100, 100, 99, 10, 1, 20, BASS_DX8_PHASE_90);
                    //change volume
                    //handle->sound->setVolumn(2);
                    //causes echo effect
                    //audiomix->Setecho(handle->sound->sound, true);
                    //audiomix->echoproperties(100, 0, 1, 1, TRUE);
                    //causes distortion effect
                    //audiomix->Setdistortion(handle->sound->sound, true);
                    //audiomix->distortionproperties(-18, 15, 2400, 2400, 8000);
                }
            }

            for (Entity e : entities) {
                ComponentHandle<AudioSource> handle = e.component<AudioSource>();
                handle->sound->setVolumn(handle->volume);
            }
        }

        void initSystem() {
            if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
                Logger::getInstance() << "There is no device to use" << "\n\r";
            }
            else {
                initialized = true;
            }
        }

        void cleanup(EntityManager& es) {
            auto entities = es.entities_with_components<AudioSource>();
            for (Entity e : entities) {
                ComponentHandle<AudioSource> handle = e.component<AudioSource>();
                handle->cleanUp();
            }
            BASS_Free();
        }

    private:
        bool initialized = false;
        bool newScene = true;
};
