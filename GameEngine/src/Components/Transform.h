#pragma once

struct Transform {
    Transform(float x = 0.0f, float y = 0.0f, float z = 0.0f,
        int angle = 0, int rx = 0, int ry = 0, int rz = 0,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f) : x(x), y(y), z(z), angle(angle), rx(rx), ry(ry), rz(rz), sx(sx), sy(sy), sz(sz) {}

    float x, y, z; //position
    int angle, rx, ry, rz; //rotation. Angle = angle in degrees. rx,ry,rz = axis to rotate on 1 to use axis 0 to not.
    // we might want to convert this into a quaternion or a single rotation about an axis

    float sx, sy, sz; //scale
};