#pragma once

struct Camera{
    Camera(float lf = 0.0f, float rf = 960.0f, float bf=0.0f, float tf=540.0f, float dnp=-1.0f, float dfp=1.0f, float x=0, float y=0, float z=0):
    lf(lf), rf(rf), bf(bf), tf(tf), dnp(dnp), dfp(dfp), x(x), y(y), z(z){}

    float lf, rf, bf, tf, dnp, dfp, x, y, z;
};