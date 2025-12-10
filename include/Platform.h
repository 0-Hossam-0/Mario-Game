#ifndef PLATFORM_H
#define PLATFORM_H

#include <GL/glut.h>
#include "TextureUtils.h"

class Platform
{
private:
    float x, y;           // Position
    float width, height;  // Dimensions
    GLuint textureID;     // Texture for the platform

public:
    // Constructor
    Platform(float startX, float startY, float w, float h, const char* texturePath);
    
    // Destructor
    ~Platform();
    
    // Draw the platform
    void draw();
    
    // Collision detection - check if a point is on top of the platform
    bool isOnTop(float px, float py, float pw, float ph) const;
    
    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getTopY() const { return y + height; } // Top surface Y position
};

#endif