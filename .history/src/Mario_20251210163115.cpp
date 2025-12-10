#include "../include/Mario.h"
#include "../include/TextureUtils.h"
#include <cstdlib>
#include <iostream>

Mario* Mario::instance = nullptr;

Mario::Mario(float startX, float startY)
    : Player(startX, startY, 80, 80, "./assets/Mario/mario.png")
{
  idleTexture = textureID; 
  moveTexture1 = TextureUtils::loadTexture("./assets/Mario/mario_move1.gif");
  moveTexture2 = TextureUtils::loadTexture("./assets/Mario/mario_move2.gif");
  
  speed = 6.0f;
  jumpStrength = 16.0f;
  
  jumpTexture = TextureUtils::loadTexture("./assets/Mario/mario_jump.png");
  jumpTexture1 = TextureUtils::loadTexture("./assets/Mario/mario_jump_1.png");
  
  isMoving = false;
  facingRight = true;
  animationFrame = 0;
  frameCounter = 0;
  enemy = nullptr; // Initialize enemy to null
  
  int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
  if (screenHeight == 0) screenHeight = 600; 
  
  initHUD(20, screenHeight - 90, "./assets/Mario/mario.png", 3, 0);
}

Mario* Mario::getInstance(float startX, float startY)
{
  if (instance == nullptr) instance = new Mario(startX, startY);
  return instance;
}

void Mario::destroyInstance()
{
  if (instance != nullptr)
  {
    if (instance->moveTexture1 != 0) glDeleteTextures(1, &instance->moveTexture1);
    if (instance->moveTexture2 != 0) glDeleteTextures(1, &instance->moveTexture2);
    if (instance->jumpTexture != 0) glDeleteTextures(1, &instance->jumpTexture);
    if (instance->jumpTexture1 != 0) glDeleteTextures(1, &instance->jumpTexture1);
    
    for (Fireball* fb : instance->fireballs) delete fb;
    instance->fireballs.clear();

    delete instance;
    instance = nullptr;
  }
}

void Mario::draw()
{
  GLuint currentTexture;
  bool flipHorizontal = false;
  
  if (isJumping)
  {
      if (velocityY > 16.5f) currentTexture = jumpTexture1;
      else currentTexture = jumpTexture;
      if (!facingRight) flipHorizontal = true;
  }
  else if (isMoving)
  {
    currentTexture = (animationFrame == 0) ? moveTexture1 : moveTexture2;
    if (!facingRight) flipHorizontal = true;
  }
  else
  {
    currentTexture = idleTexture;
    if (!facingRight) flipHorizontal = true;
  }
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, currentTexture);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);
  if (flipHorizontal)
  {
      glTexCoord2f(1, 1); glVertex2f(x, y);
      glTexCoord2f(0, 1); glVertex2f(x + width, y);
      glTexCoord2f(0, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(1, 0); glVertex2f(x, y + height);
  }
  else
  {
      glTexCoord2f(0, 1); glVertex2f(x, y);
      glTexCoord2f(1, 1); glVertex2f(x + width, y);
      glTexCoord2f(1, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(0, 0); glVertex2f(x, y + height);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  
  drawHUD();

  for (Fireball* fb : fireballs) {
      fb->draw();
  }
}

void Mario::update()
{
    isMoving = false;
    Player::update();

    // UPDATE FIREBALLS
    for (int i = 0; i < fireballs.size(); i++) {
        // Pass the enemy (Luigi) to the fireball to check for collision
        fireballs[i]->update(enemy);
        
        if (!fireballs[i]->isActive()) {
            delete fireballs[i];
            fireballs.erase(fireballs.begin() + i);
            i--;
        }
    }
}

void Mario::move(float dx, float dy, float &oldX, float &oldY)
{
  if (dx > 0) facingRight = true;
  if (dx < 0) facingRight = false;

  Player::move(dx, dy, oldX, oldY);
  
  if (dx != 0)
  {
      if (x != oldX)
      {
        isMoving = true;
        int threshold = isSprinting ? 5 : 10;
        frameCounter++;
        if (frameCounter >= threshold)
        {
          animationFrame = (animationFrame == 0) ? 1 : 0;
          frameCounter = 0;
          // Footstep sounds (omitted for brevity)
        }
      }
      else
      {
        isMoving = false;
        frameCounter = 0;
        animationFrame = 0;
      }
  }
}

void Mario::jump()
{
    bool wasOnGround = isOnGround;
    Player::jump();
    if (wasOnGround && isJumping) {
        system("gst-launch-1.0 filesrc location=./assets/Sounds/small-jump.mp3 ! decodebin ! autoaudiosink &");
    }
}

void Mario::setFacingRight(bool facing)
{
    facingRight = facing;
}

void Mario::shootFireball()
{
    float spawnX = facingRight ? x + width : x - 20; 
    float spawnY = y + height / 2;
    fireballs.push_back(new Fireball(spawnX, spawnY, facingRight));
    system("gst-launch-1.0 filesrc location=./assets/Sounds/fireball.wav ! decodebin ! autoaudiosink &");
}