#include "../include/Platform.h"
#include <iostream>

// constructor
Platform::Platform(float startX, float startY, float w, float h, const char* texturePath)
{
    x = startX;
    y = startY;
    width = w;
    height = h;
    
    // load platform texture
    textureID = TextureUtils::loadTexture(texturePath);
    
    if (textureID == 0)
    {
        std::cout << "Warning: Platform texture failed to load from: " << texturePath << std::endl;
    }
}

// destructor
Platform::~Platform()
{
    if (textureID != 0)
    {
        glDeleteTextures(1, &textureID);
    }
}

// draw platform
void Platform::draw()
{
    if (textureID == 0)
    {
        // draw a brown rectangle if texture failed to load
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.6f, 0.4f, 0.2f); // brown color
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
        return;
    }
    
    // draw 5 platform tiles side by side
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // calculate the width of each tile
    float tileWidth = width / 5.0f;
    
    // draw 5 tiles
    for (int i = 0; i < 5; i++)
    {
        float tileX = x + (i * tileWidth);
        
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);  // bottom left
        glVertex2f(tileX, y);
        glTexCoord2f(1, 1);  // bottom right
        glVertex2f(tileX + tileWidth, y);
        glTexCoord2f(1, 0);  // top right
        glVertex2f(tileX + tileWidth, y + height);
        glTexCoord2f(0, 0);  // top left
        glVertex2f(tileX, y + height);
        glEnd();
    }
    
    glDisable(GL_TEXTURE_2D);
}

// check if object is on top of the platform
bool Platform::isOnTop(float px, float py, float pw, float ph) const
{
    // check if bottom of the object is at or near top of the platform
    float objectBottom = py;
    float platformTop = y + height;
    
    // check horizontal overlap
    bool horizontalOverlap = (px + pw > x) && (px < x + width);
    
    // check if object is standing on platform (with small tolerance)
    bool onTop = (objectBottom >= platformTop - 5) && (objectBottom <= platformTop + 5);
    
    return horizontalOverlap && onTop;
}
