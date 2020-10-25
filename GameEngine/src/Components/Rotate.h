#pragma once

struct Rotate{
    Rotate(int angle = 0, int x = 0, int y = 0, int z = 0): angle(angle), x(x), y(y), z(z){}

    int angle, x, y, z;
};