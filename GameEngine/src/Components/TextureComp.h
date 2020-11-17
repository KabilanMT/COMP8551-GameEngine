#pragma once
#include "../Texture.h"

struct TextureComp {
    TextureComp(const char* filepath): filepath(filepath) {
    }

    ~TextureComp() {
        deInit();
    }

    void init() {
        texture = new Texture(filepath);
        ready = true;
    }

    void deInit() {
        if (texture != nullptr) {
            delete texture;
            texture = nullptr;
        }
        ready = false;
    }

    const char* getFilepath() { return filepath; }

    const char* filepath;
    Texture* texture;
    bool ready = false;
};