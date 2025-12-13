#include "../include/Star.h"
#include <cstdlib>
#include <cmath>

Star::Star(float startX, float startY, float speed) {
    x = startX;
    y = startY;
    originalY = startY;
    width = 50.0f;  // increased size
    height = 50.0f; // increased size
    velocityX = speed;
    active = true;
    bobTimer = 0.0f;
    textureID = TextureUtils::loadTexture("./assets/Maps/star.png");
}

Star::~Star() {
    // Texture is managed globally or delete it
    // but usually textureutils might cache it 
    // dont delete it to avoid reloading issues if multiple stars use it
    // unless we implement a resource manager
}

void Star::update(float deltaTime) {
    if (!active) return;
    x += velocityX * deltaTime;
    
    // bobbing animation
    bobTimer += deltaTime * 5.0f; // speed of bobbing
    y = originalY + sin(bobTimer) * 20.0f; // amplitude of 20 pixel
}

void Star::draw() {
    if (!active) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(x, y, 0);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(width, 0);
    glTexCoord2f(1, 0); glVertex2f(width, height);
    glTexCoord2f(0, 0); glVertex2f(0, height);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

bool Star::checkCollision(Player* p) {
    if (!active || !p) return false;

    float pX = p->getX();
    float pY = p->getY();
    float pW = p->getW();
    float pH = p->getH();

    // AABB Collision
    bool collisionX = x + width >= pX && pX + pW >= x;
    bool collisionY = y + height >= pY && pY + pH >= y;

    return collisionX && collisionY;
}
