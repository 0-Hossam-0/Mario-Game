#ifndef MAP_H
#define MAP_H

#include <GL/glut.h>
#include <vector>
#include "TextureUtils.h"
#include "Platform.h"

class Map
{
private:
    // private constructor - singleton pattern
    Map(const char* imagePath, int screenWidth, int screenHeight);
    
    // static instance pointer
    static Map* instance;
    
    // map properties
    GLuint textureID;
    int width;
    int height;
    
    // platforms
    std::vector<Platform*> platforms;

public:
    // delete copy constructor and assignment operator
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    
    // static method to get the single instance
    static Map* getInstance(const char* imagePath = "./assets/Maps/Original Map/Original.jpg", 
                           int screenWidth = 800, 
                           int screenHeight = 600);
    
    // static method to destroy the instance
    static void destroyInstance();
    
    // draw the map as background
    void draw();
    void draw(float scale);  // draw with custom scale for zoom effects
    
    // update dimensions (for window resize)
    void setDimensions(int w, int h);
    
    // static reshape handler for GLUT
    static void handleReshape(int w, int h);
    
    // platform methods
    void addPlatform(Platform* platform);
    const std::vector<Platform*>& getPlatforms() const;
    
    // getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif
