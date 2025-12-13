#ifndef STAR_H
#define STAR_H

#include <GL/glut.h>
#include "Player.h"
#include "TextureUtils.h"

class Star {
private:
    float x, y;
    float width, height;
    float velocityX;
    GLuint textureID;
    bool active;
    
    // bobbing animation
    float originalY;
    float bobTimer;

public:
    Star(float startX, float startY, float speed);
    ~Star();

    void update(float deltaTime);
    void draw();
    bool checkCollision(Player* p);
    bool isActive() const { return active; }
    void deactivate() { active = false; }
};

#endif
