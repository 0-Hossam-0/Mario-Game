#ifndef HUD_H
#define HUD_H

#include <GL/glut.h>
#include <string>
#include "TextureUtils.h"

class HUD
{
private:
    int lives;
    int score;
    GLuint iconTexture;
    GLuint heartTexture;  // Heart icon for lives
    float x, y;      // Position on screen
    float iconSize;  // Size of character icon
    float heartSize; // Size of heart icon
    
    // Helper function to draw text
    void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18);
    
    // Helper function to draw icon
    void drawIcon(float x, float y, GLuint texture);
    
    // Helper function to draw heart with custom size
    void drawHeart(float x, float y, GLuint texture, float size);

public:
    // Constructor for a single player HUD
    HUD(float xPos, float yPos,
        const char* iconPath,
        int initialLives = 3, int initialScore = 0);
    
    // Destructor
    ~HUD();
    
    // Draw the HUD
    void draw();
    
    // Update methods
    void setLives(int l);
    void setScore(int s);
    void addScore(int points);
    void loseLife();
    void gainLife();
    void setPos(float newX, float newY) { x = newX; y = newY; }
    
    // Getters
    int getLives() const;
    int getScore() const;
    bool isAlive() const;
};

#endif
