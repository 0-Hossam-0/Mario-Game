#ifndef MARIO_H
#define MARIO_H

#include "Player.h"

class Mario : public Player
{
private:
    // Private constructor - prevents direct instantiation
    Mario(float startX, float startY);
    
    // Static instance pointer
    static Mario* instance;
    
    // Animation textures
    GLuint moveTexture1;  // First walking frame (Right)
    GLuint moveTexture2;  // Second walking frame (Right)
    GLuint idleTexture;   // Standing still texture (Right)
    
    // New textures
    GLuint jumpTexture;       // Jumping (Falling/Apex)
    GLuint jumpTexture1;      // Jumping (Rising)
    
    // Animation state
    bool isMoving;
    bool facingRight;     // Track direction
    int animationFrame;   // 0 or 1 to switch between move1 and move2
    int frameCounter;     // Counter to control animation speed

public:
    // Delete copy constructor and assignment operator
    Mario(const Mario&) = delete;
    Mario& operator=(const Mario&) = delete;
    
    // Static method to get the single instance
    static Mario* getInstance(float startX = 100, float startY = 100);
    
    // Static method to destroy the instance
    static void destroyInstance();
    
    // Override draw to use animated textures
    void draw();
    
    // Override move to trigger animation
    void move(float dx, float dy, float &oldX, float &oldY);
    
    // Override update to manage animation state
    void update();
    
    // Override jump to play sound
    void jump();

    // Set facing direction manually
    void setFacingRight(bool facing);
};

#endif