#ifndef PLATFORM_H
#define PLATFORM_H

#include <GL/glut.h>
#include "TextureUtils.h"

class Platform
{
private:
    float x, y;           // position
    float width, height;  // dimensions
    GLuint textureID;     // texture for the platform

public:
    // constructor
    Platform(float startX, float startY, float w, float h, const char* texturePath);
    
    // destructor
    ~Platform();
    
    // draw the platform
    void draw();
    
    // collision detection check if a point is on top of the platform
    bool isOnTop(float px, float py, float pw, float ph) const;
    
    // getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getTopY() const { return y + height; } // top surface y position
};

#endif