#pragma once
#include <GL/glut.h>
#include "TextureUtils.h"
#include "Platform.h"
#include <vector>

// forward declaration to avoid circular includes
class Player;

class Fireball {
private:
    float x, y;
    float width, height;
    float velocityX;
    float velocityY;
    bool active;
    bool facingRight;
    GLuint textureID;
    Player* owner; // the player who shot the fireball

    // Physics constants
    const float GRAVITY = 0.5f;
    const float BOUNCE_FORCE = 10.0f; 
    const float MOVE_SPEED = 9.0f;

public:
    Fireball(Player* owner, float startX, float startY, bool directionRight);
    
    void update(Player* enemy); // updated to take enemy
    void draw();
    
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    
    // check if fireball hits a specific player
    bool checkCollision(Player* p);

    // getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return width; }
    float getH() const { return height; }
};