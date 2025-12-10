#include "../include/Fireball.h"
#include "../include/Map.h"
#include <iostream>

Fireball::Fireball(float startX, float startY, bool directionRight) {
    x = startX;
    y = startY;
    width = 20.0f;  // Size of fireball
    height = 20.0f;
    active = true;
    facingRight = directionRight;

    // Set horizontal speed
    velocityX = facingRight ? MOVE_SPEED : -MOVE_SPEED;
    
    // Initial small pop up when spawned
    velocityY = -2.0f; 

    // Load texture (make sure you have this asset)
    textureID = TextureUtils::loadTexture("./assets/Items/fireball.png");
}

void Fireball::update() {
    if (!active) return;

    // 1. Move Horizontally
    x += velocityX;

    // 2. Apply Gravity
    velocityY -= GRAVITY;
    y += velocityY;

    // 3. Check for Ground/Platform Collision (The "Fixed Bounce")
    // We assume ground is at y=145 like in your Player class
    float groundLevel = 145.0f;
    bool hitGround = false;

    // Check Map Platforms
    Map* map = Map::getInstance();
    if (map != nullptr) {
        const std::vector<Platform*>& platforms = map->getPlatforms();
        for (Platform* platform : platforms) {
            // Simple AABB collision check
            if (x + width > platform->getX() && x < platform->getX() + platform->getWidth() &&
                y <= platform->getTopY() && y + height >= platform->getTopY() &&
                velocityY < 0) { // Only bounce if falling down
                
                y = platform->getTopY(); // Snap to top
                hitGround = true;
                break;
            }
        }
    }

    // Check main floor
    if (!hitGround && y <= groundLevel) {
        y = groundLevel;
        hitGround = true;
    }

    // THE MAGIC: If we hit ground, reset velocity to a FIXED positive value.
    // This guarantees the bounce height is always identical.
    if (hitGround) {
        velocityY = BOUNCE_FORCE;
    }

    // 4. Deactivate if off-screen
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    if (x < 0 || x > screenWidth) {
        active = false;
    }
}

void Fireball::draw() {
    if (!active) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1, 1, 1);

    // Rotate the fireball for visual flair (optional)
    glPushMatrix();
    glTranslatef(x + width/2, y + height/2, 0); // Move to center
    static float angle = 0;
    angle += 20; // Spin speed
    glRotatef(angle, 0, 0, 1); // Rotate around Z axis
    glTranslatef(-width/2, -height/2, 0); // Move back

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(width, 0);
    glTexCoord2f(1, 0); glVertex2f(width, height);
    glTexCoord2f(0, 0); glVertex2f(0, height);
    glEnd();
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}