#pragma once
#include <GL/glut.h>
#include "Player.h"
#include "TextureUtils.h"
#include "Map.h"
#include <vector>

class Bomb {
public:
    enum State { FALLING, ON_GROUND, HELD, THROWN, EXPLODING, DEAD };

    Bomb(float startX, float startY);
    ~Bomb();

    void update(float deltaTime);
    void draw();
    
    bool checkCollision(Player* p);
    void pickUp(Player* p);
    void throwBomb(bool facingRight);
    
    State getState() const { return state; }
    bool isDead() const { return state == DEAD; }
    
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

private:
    float x, y;
    float width, height;
    float velocityX, velocityY;
    float gravity;
    State state;
    
    Player* holder;
    
    float explodeTimer;
    float explosionDuration;
    
    GLuint textureNormal;
    GLuint textureExploding;
    
    std::vector<Player*> hitPlayers;

public:
    bool hasHit(Player* p) {
        for(Player* hit : hitPlayers) if(hit == p) return true;
        return false;
    }
    void markHit(Player* p) { hitPlayers.push_back(p); }
};
