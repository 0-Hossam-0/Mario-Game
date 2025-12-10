#include "../include/Fireball.h"
#include "../include/Map.h"
#include "../include/Player.h" // Needed for Player methods
#include <iostream>

Fireball::Fireball(float startX, float startY, bool directionRight) {
    x = startX;
    y = startY;
    width = 20.0f;
    height = 20.0f;
    active = true;
    facingRight = directionRight;

    velocityX = facingRight ? MOVE_SPEED : -MOVE_SPEED;
    velocityY = -2.0f; 

    // CHANGED: New path requested
    textureID = TextureUtils::loadTexture("./assets/Maps/fireball.gif"); 
}

// Helper to check AABB collision
bool Fireball::checkCollision(Player* p) {
    if (!active || p == nullptr) return false;

    // Player hitbox
    float pX = p->getX();
    float pY = p->getY();
    float pW = p->getW();
    float pH = p->getH();

    // Check overlap
    bool collisionX = x + width > pX && x < pX + pW;
    bool collisionY = y + height > pY && y < pY + pH;

    return collisionX && collisionY;
}

void Fireball::update(Player* enemy) {
    if (!active) return;

    // 1. Move
    x += velocityX;
    velocityY -= GRAVITY;
    y += velocityY;

    // 2. Map Collision (Bounce)
    float groundLevel = 145.0f;
    bool hitGround = false;

    Map* map = Map::getInstance();
    if (map != nullptr) {
        const std::vector<Platform*>& platforms = map->getPlatforms();
        for (Platform* platform : platforms) {
            if (x + width > platform->getX() && x < platform->getX() + platform->getWidth() &&
                y <= platform->getTopY() && y + height >= platform->getTopY() &&
                velocityY < 0) {
                
                y = platform->getTopY();
                hitGround = true;
                break;
            }
        }
    }

    if (!hitGround && y <= groundLevel) {
        y = groundLevel;
        hitGround = true;
    }

    if (hitGround) {
        velocityY = BOUNCE_FORCE;
    }

    // 3. Enemy Collision (Damage)
    if (enemy != nullptr && checkCollision(enemy)) {
        // Deal damage
        enemy->loseLife();
        
        // Destroy fireball
        active = false;
        
        // Play hit sound (optional)
        // system("gst-launch-1.0 filesrc location=./assets/Sounds/kick.wav ! decodebin ! autoaudiosink &");
        return; 
    }

    // 4. Screen Bounds
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    if (x < 0 || x > screenWidth) {
        active = false;
    }
}

void Fireball::draw() {
    if (!active) return;
    
    if (textureID == 0) {
        glDisable(GL_TEXTURE_2D);
        glColor3f(1, 0.5f, 0); // Orange fallback
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1, 1, 1);

        glPushMatrix();
        glTranslatef(x + width/2, y + height/2, 0); 
        static float angle = 0;
        angle += 20; 
        glRotatef(angle, 0, 0, 1); 
        glTranslatef(-width/2, -height/2, 0); 

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(0, 0);
        glTexCoord2f(1, 1); glVertex2f(width, 0);
        glTexCoord2f(1, 0); glVertex2f(width, height);
        glTexCoord2f(0, 0); glVertex2f(0, height);
        glEnd();
        
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
}