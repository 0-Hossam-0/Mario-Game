#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/TextureUtils.h"
#include <windows.h> // Required for sound
#include <mmsystem.h> // Required for sound
#include <string>
#include <iostream>

// ---------------------------------------------------------
// PLAYER BASE CLASS IMPLEMENTATION
// ---------------------------------------------------------

Player::Player(float startX, float startY, float w, float h, const char *imagePath)
{
  x = startX;
  y = startY;
  width = w;
  height = h;
  speed = 5.0f;
  hud = nullptr; 

  textureID = TextureUtils::loadTexture(imagePath);
  
  velocityY = 0.0f;
  gravity = 0.5f;
  jumpStrength = 15.0f; 
  maxJumpTime = 12.0f;  
  jumpTimer = 0.0f;
  isJumping = false;
  isOnGround = true; 
  isSprinting = false;
  
  // Cooldown
  shootCooldown = 0.26f; // 0.26 second cooldown
  shootTimer = 0.0f;

  for(int i=0; i<256; i++) keyStates[i] = false;
}

Player::~Player()
{
  if (hud != nullptr)
  {
    delete hud;
    hud = nullptr;
  }
}

// CHANGED: Windows Native Sound Player
void Player::playSound(const char* path)
{
    // We use mciSendString to play mp3/wav on Windows without external libraries
    std::string cmd = "open \"" + std::string(path) + "\" type mpegvideo alias mySound";
    // Close any previous instance (simple overlap prevention)
    mciSendString("close mySound", NULL, 0, NULL);
    
    // Open the new file
    mciSendString(cmd.c_str(), NULL, 0, NULL);
    
    // Play it
    mciSendString("play mySound from 0 notify", NULL, 0, NULL);
}

void Player::jump()
{
  if (isOnGround)
  {
    velocityY = jumpStrength;
    isJumping = true;
    isOnGround = false;
    jumpTimer = 0;
  }
  else if (isJumping && jumpTimer < maxJumpTime)
  {
      velocityY += 0.6f;
      jumpTimer += 1.0f;
  }
}

void Player::setKeyState(unsigned char key, bool pressed)
{
    keyStates[key] = pressed;
    if (key >= 'a' && key <= 'z') keyStates[key - 32] = pressed;
    if (key >= 'A' && key <= 'Z') keyStates[key + 32] = pressed;
}

void Player::processInput()
{
    float oldX, oldY;
    isSprinting = (keyStates['z'] || keyStates['Z']) && isOnGround;
    float currentSpeed = speed;
    if (isSprinting) currentSpeed *= 1.8f; 

    if (keyStates['a'] || keyStates['A']) move(-currentSpeed, 0, oldX, oldY);
    if (keyStates['d'] || keyStates['D']) move(currentSpeed, 0, oldX, oldY);
    if (keyStates['w'] || keyStates['W']) jump();
}

void Player::update(float deltaTime)
{
  if (shootTimer > 0) {
    shootTimer -= deltaTime;
  }

  processInput();
  velocityY -= gravity;
  float oldX = x;
  float oldY = y;
  move(0, velocityY, oldX, oldY);
  isOnGround = false;
  
  Map* map = Map::getInstance();
  if (map != nullptr)
  {
      const std::vector<Platform*>& platforms = map->getPlatforms();
      for (Platform* platform : platforms)
      {
          bool droppingDown = keyStates['s'] || keyStates['S'];
          if (velocityY <= 0 && !droppingDown)
          {
              float platformTop = platform->getTopY();
              float platformLeft = platform->getX();
              float platformRight = platform->getX() + platform->getWidth();
              
              if (x + width > platformLeft && x < platformRight)
              {
                  if ((oldY >= platformTop && y <= platformTop) || 
                      (y >= platformTop - 5.0f && y <= platformTop + 5.0f))
                  {
                      y = platformTop;
                      velocityY = 0.0f;
                      isOnGround = true;
                      isJumping = false;
                      break; 
                  }
              }
          }
      }
  }
  
  float groundLevel = 145.0f;
  if (!isOnGround && y <= groundLevel)
  {
    y = groundLevel;
    velocityY = 0.0f;
    isOnGround = true;
    isJumping = false;
  }
}

void Player::initHUD(float xPos, float yPos, const char* iconPath, int lives, int score)
{
  if (hud != nullptr) delete hud;
  hud = new HUD(xPos, yPos, iconPath, lives, score);
}

void Player::drawHUD()
{
  if (hud != nullptr) hud->draw();
}

void Player::addScore(int points)
{
  if (hud != nullptr) hud->addScore(points);
}

void Player::loseLife()
{
  if (hud != nullptr) {
      if (hud->getLives() > 0) {
          hud->loseLife();
          // Use the new static method
          playSound("./assets/Sounds/life-lost.mp3");
      }
  }
}

void Player::gainLife()
{
  if (hud != nullptr) hud->gainLife();
}

int Player::getScore() const
{
  return (hud != nullptr) ? hud->getScore() : 0;
}

int Player::getLives() const
{
  return (hud != nullptr) ? hud->getLives() : 0;
}

void Player::draw()
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1); 
  glVertex2f(x, y);
  glTexCoord2f(1, 1);
  glVertex2f(x + width, y);
  glTexCoord2f(1, 0);
  glVertex2f(x + width, y + height);
  glTexCoord2f(0, 0);
  glVertex2f(x, y + height);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  drawHUD();
}

void Player::move(float dx, float dy, float &oldX, float &oldY)
{
  oldX = x;
  oldY = y;
  float newX = x + dx;
  float newY = y + dy;
  int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
  int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
  
  if (newX < 0) newX = 0;
  if (newX + width > screenWidth) newX = screenWidth - width;
  if (newY < 0) newY = 0;
  if (newY + height > screenHeight) newY = screenHeight - height;
  
  x = newX;
  y = newY;
}

float Player::getX() const { return x; }
float Player::getY() const { return y; }
float Player::getW() const { return width; }
float Player::getH() const { return height; }
float Player::getSpeed() const { return speed; }
void Player::setPos(float newX, float newY)
{
  x = newX;
  y = newY;
}

void Player::setHUDPos(float x, float y)
{
    if (hud != nullptr) hud->setPos(x, y);
}