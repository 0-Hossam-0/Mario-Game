#include "../include/Map.h"
#include <iostream>

// initialize static instance pointer to nullptr
Map* Map::instance = nullptr;

// private constructor
Map::Map(const char* imagePath, int screenWidth, int screenHeight)
{
    width = screenWidth;
    height = screenHeight;
    
    // load the map texture
    textureID = TextureUtils::loadTexture(imagePath);
    
    if (textureID == 0)
    {
        std::cout << "Warning: Map texture failed to load from: " << imagePath << std::endl;
    }
    else
    {
        std::cout << "Map loaded successfully from: " << imagePath << std::endl;
    }
    
    // initialize default platforms
    addPlatform(new Platform(400, 350, 400, 60, "./assets/Maps/Original Map/Platform.png"));
    addPlatform(new Platform(1100, 350, 400, 60, "./assets/Maps/Original Map/Platform.png"));
}

// get the singleton instance
Map* Map::getInstance(const char* imagePath, int screenWidth, int screenHeight)
{
    if (instance == nullptr)
    {
        instance = new Map(imagePath, screenWidth, screenHeight);
    }
    return instance;
}

// destroy the singleton instance
void Map::destroyInstance()
{
    if (instance != nullptr)
    {
        // clean up texture
        if (instance->textureID != 0)
        {
            glDeleteTextures(1, &instance->textureID);
        }
        
        // clean up platforms
        for (Platform* platform : instance->platforms)
        {
            delete platform;
        }
        instance->platforms.clear();
        
        delete instance;
        instance = nullptr;
    }
}

// draw the map as background
void Map::draw()
{
    if (textureID == 0)
    {
        // if texture failed to load, draw a sky blue background
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.53f, 0.81f, 0.92f); // sky blue
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(width, 0);
        glVertex2f(width, height);
        glVertex2f(0, height);
        glEnd();
    }
    else
    {
        // enable texturing
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f); // white color to show texture as is
        
        // draw the background quad covering the entire screen
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);  // bottom left of texture
        glVertex2f(0, 0);
        glTexCoord2f(1, 1);  // bottom right of texture
        glVertex2f(width, 0);
        glTexCoord2f(1, 0);  // top right of texture
        glVertex2f(width, height);
        glTexCoord2f(0, 0);  // top left of texture
        glVertex2f(0, height);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
    }
    
    // draw platforms
    for (Platform* platform : platforms)
    {
        platform->draw();
    }
}

// draw the map with custom scale (for zoom effects)
void Map::draw(float scale)
{
    // calculate expanded dimensions to fill screen when zoomed out
    // when scale is 0.5 we need 2x the size to fill the screen
    float expandedWidth = width / scale;
    float expandedHeight = height / scale;
    
    // calculate offset to keep map centered
    float offsetX = (expandedWidth - width) / 2.0f;
    float offsetY = (expandedHeight - height) / 2.0f;
    
    if (textureID == 0)
    {
        // if texture failed to load draw sky blue background
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.53f, 0.81f, 0.92f); // sky blue
        glBegin(GL_QUADS);
        glVertex2f(-offsetX, -offsetY);
        glVertex2f(expandedWidth - offsetX, -offsetY);
        glVertex2f(expandedWidth - offsetX, expandedHeight - offsetY);
        glVertex2f(-offsetX, expandedHeight - offsetY);
        glEnd();
    }
    else
    {
        // enable texturing
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
        
        // draw the expanded background quad
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex2f(-offsetX, -offsetY);
        glTexCoord2f(1, 1);
        glVertex2f(expandedWidth - offsetX, -offsetY);
        glTexCoord2f(1, 0);
        glVertex2f(expandedWidth - offsetX, expandedHeight - offsetY);
        glTexCoord2f(0, 0);
        glVertex2f(-offsetX, expandedHeight - offsetY);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
    }
    
    // draw platforms
    for (Platform* platform : platforms)
    {
        platform->draw();
    }
}

// platform methods
void Map::addPlatform(Platform* platform)
{
    platforms.push_back(platform);
}

const std::vector<Platform*>& Map::getPlatforms() const
{
    return platforms;
}

// update dimensions when window is resized
void Map::setDimensions(int w, int h)
{
    width = w;
    height = h;
}

// static reshape handler for GLUT
void Map::handleReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);

    if (instance != nullptr)
    {
        instance->setDimensions(w, h);
    }
}
