#pragma once
#include <GL/glut.h>
#include "TextureUtils.h"

class Fireball {
private:
    float x, y;
    float width, height;
    float velocityX;
    float velocityY;
    bool active;
    bool facingRight;
    GLuint textureID;

    // Physics constants for consistent bouncing
    const float GRAVITY = 0.4f;
    const float BOUNCE_FORCE = 7.0f; // Fixed upward force on every bounce
    const float MOVE_SPEED = 8.0f;

public:
    Fireball(float startX, float startY, bool directionRight);
    void update();
    void draw();
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    
    // Getters for collision
    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return width; }
    float getH() const { return height; }
};