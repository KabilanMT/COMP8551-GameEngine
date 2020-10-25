#pragma once

struct Position {
    Position(
        float v0x = -50.0f, float v0y = -50.0f, float v0t1 = 0.0f, float v0t2 = 0.0f,
        float v1x =  50.0f, float v1y = -50.0f, float v1t1 = 1.0f, float v1t2 = 0.0f,
        float v2x =  50.0f, float v2y =  50.0f, float v2t1 = 1.0f, float v2t2 = 1.0f,
        float v3x = -50.0f, float v3y =  50.0f, float v3t1 = 0.0f, float v3t2 = 1.0f,
        unsigned int v0 = 0, unsigned int v1 = 1, unsigned int v2 = 2,
        unsigned int v3 = 2, unsigned int v4 = 3, unsigned int v5 = 0) : 
        v0x(v0x), v0y(v0y), v0t1(v0t1), v0t2(v0t2),
        v1x(v1x), v1y(v1y), v1t1(v1t1), v1t2(v1t2),
        v2x(v2x), v2y(v2y), v2t1(v2t1), v2t2(v2t2),
        v3x(v3x), v3y(v3y), v3t1(v3t1), v3t2(v3t2),
        v0(v0), v1(v1), v2(v2),
        v3(v3),v4(v4), v5(v5){}

    float v0x;
    float v0y;  
    float v0t1;
    float v0t2;

    float v1x;  
    float v1y;  
    float v1t1;
    float v1t2; 

    float v2x; 
    float v2y;
    float v2t1;
    float v2t2; 

    float v3x;  
    float v3y;  
    float v3t1;
    float v3t2;

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
    unsigned int v4;
    unsigned int v5;
};