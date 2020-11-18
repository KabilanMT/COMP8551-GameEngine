#pragma once

struct TextComp {
    TextComp(const char* text, float x, float y, float scale, float colourR, float colourG, float colourB) : text(text), x(x), y(y), scale(scale), colourR(colourR), colourG(colourG), colourB(colourB) {}

    const char* text;
    float x, y, scale, colourR, colourG, colourB;
};