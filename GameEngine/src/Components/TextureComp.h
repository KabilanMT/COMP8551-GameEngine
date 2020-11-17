#pragma once
#include "../Texture.h"

struct TextureComp {
    TextureComp(const char* filepath): filepath(filepath) {
        Logger::getInstance() << "TextureComp Constructor\n";
    }

    ~TextureComp() {
        Logger::getInstance() << "TextureComp destructor\n";
        deInit();
    }

    void init() {
        Logger::getInstance() << "TextureComp init\n";
        texture = new Texture(filepath);
        ready = true;
    }

    void deInit() {
        Logger::getInstance() << "TextureComp deInit\n";
        if (texture != nullptr) {
            Logger::getInstance() << "TextureComp before delete\n";
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