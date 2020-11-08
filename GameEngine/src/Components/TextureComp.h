#pragma once

struct TextureComp {
    TextureComp(const char* filepath): filepath(filepath){}

    const char* getFilepath() { return filepath; }

    const char* filepath;
};