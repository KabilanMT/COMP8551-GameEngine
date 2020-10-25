#pragma once

struct Transform {
    Transform(float x = 0.0f, float y = 0.0f, float z = 0.0f,
        float rx = 0.0f, float ry = 0.0f, float rz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f) : x(x), y(y), z(z), rx(rx), ry(ry), rz(rz), sx(sx), sy(sy), sz(sz) {}

    float x, y, z; //position
    float rx, ry, rz; //rotation about each axis (we can say the order of rotation is x, y, z),
    // we might want to convert this into a quaternion or a single rotation about an axis

    float sx, sy, sz; //scale
};