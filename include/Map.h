#ifndef MAP_H
#define MAP_H

#include <GL/glut.h>
#include <vector>
#include "TextureUtils.h"
#include "Platform.h"

class Map
{
private:
    // Private constructor - Singleton pattern
    Map(const char* imagePath, int screenWidth, int screenHeight);
    
    // Static instance pointer
    static Map* instance;
    
    // Map properties
    GLuint textureID;
    int width;
    int height;
    
    // Platforms
    std::vector<Platform*> platforms;

public:
    // Delete copy constructor and assignment operator
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    
    // Static method to get the single instance
    static Map* getInstance(const char* imagePath = "./assets/Maps/Original Map/Original.jpg", 
                           int screenWidth = 800, 
                           int screenHeight = 600);
    
    // Static method to destroy the instance
    static void destroyInstance();
    
    // Draw the map as background
    void draw();
    void draw(float scale);  // Draw with custom scale for zoom effects
    
    // Update dimensions (for window resize)
    void setDimensions(int w, int h);
    
    // Static reshape handler for GLUT
    static void handleReshape(int w, int h);
    
    // Platform methods
    void addPlatform(Platform* platform);
    const std::vector<Platform*>& getPlatforms() const;
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif
