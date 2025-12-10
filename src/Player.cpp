#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/TextureUtils.h" // Needed to call TextureUtils::loadTexture

// ---------------------------------------------------------
// PLAYER BASE CLASS IMPLEMENTATION
// ---------------------------------------------------------

// Constructor Definition
Player::Player(float startX, float startY, float w, float h, const char *imagePath)
{
  x = startX;
  y = startY;
  width = w;
  height = h;
  speed = 5.0f;
  hud = nullptr; // Initialize HUD pointer to null

  textureID = TextureUtils::loadTexture(imagePath);
  
  // Initialize physics
  velocityY = 0.0f;
  gravity = 0.5f;
  jumpStrength = 15.0f; // Reduced from 20.0f for variable jump
  maxJumpTime = 12.0f;  // Max frames to sustain jump
  jumpTimer = 0.0f;
  isJumping = false;
  isOnGround = true; // Assume starting on ground
  isSprinting = false;
  
  // Initialize key states
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
      // Sustain the jump by counteracting gravity/adding force
      // Gravity is 0.5, so adding 0.5 keeps velocity constant.
      // We'll add a bit more to give a slight boost.
      velocityY += 0.6f;
      jumpTimer += 1.0f;
  }
}

void Player::setKeyState(unsigned char key, bool pressed)
{
    keyStates[key] = pressed;
    // Handle case-insensitivity
    if (key >= 'a' && key <= 'z') keyStates[key - 32] = pressed;
    if (key >= 'A' && key <= 'Z') keyStates[key + 32] = pressed;
}

void Player::processInput()
{
    float oldX, oldY;
    
    // Check for sprint key (Z)
    isSprinting = (keyStates['z'] || keyStates['Z']) && isOnGround;
    
    float currentSpeed = speed;
    if (isSprinting)
    {
        currentSpeed *= 1.8f; // 80% speed boost
    }

    if (keyStates['a'] || keyStates['A'])
    {
        move(-currentSpeed, 0, oldX, oldY);
    }
    if (keyStates['d'] || keyStates['D'])
    {
        move(currentSpeed, 0, oldX, oldY);
    }
    if (keyStates['w'] || keyStates['W'])
    {
        jump();
    }
}

void Player::update()
{
  // Process Input first
  processInput();

  // Apply gravity
  // Always apply gravity to ensure we fall if we walk off a platform
  velocityY -= gravity;
  
  // Update Y position
  float oldX = x;
  float oldY = y;
  move(0, velocityY, oldX, oldY);
  
  // Reset ground state - we'll set it to true if we find a collision
  isOnGround = false;
  
  // Check Platform Collisions
  Map* map = Map::getInstance();
  if (map != nullptr)
  {
      const std::vector<Platform*>& platforms = map->getPlatforms();
      for (Platform* platform : platforms)
      {
          // Only check collision if we are falling or stationary vertically
          // This allows jumping up through platforms
          // Also check if 'S' is NOT pressed to allow dropping down
          bool droppingDown = keyStates['s'] || keyStates['S'];
          
          if (velocityY <= 0 && !droppingDown)
          {
              float platformTop = platform->getTopY();
              float platformLeft = platform->getX();
              float platformRight = platform->getX() + platform->getWidth();
              
              // Check horizontal overlap
              if (x + width > platformLeft && x < platformRight)
              {
                  // Check if we crossed the platform top (Swept Collision)
                  // We were above (or at) it before, and are below (or at) it now
                  // Or if we are currently within the small tolerance range (fallback)
                  if ((oldY >= platformTop && y <= platformTop) || 
                      (y >= platformTop - 5.0f && y <= platformTop + 5.0f))
                  {
                      y = platformTop;
                      velocityY = 0.0f;
                      isOnGround = true;
                      isJumping = false;
                      break; // Found a support, stop checking
                  }
              }
          }
      }
  }
  
  // Check Ground Collision
  // Assuming ground is at y = 145 (where Mario starts)
  float groundLevel = 145.0f;
  
  // Only check ground if we haven't found a platform yet
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
  if (hud != nullptr) hud->loseLife();
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

// Draw Method Definition
void Player::draw()
{
  // Draw the player sprite
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1); // Flipped Y for correct orientation
  glVertex2f(x, y);
  glTexCoord2f(1, 1);
  glVertex2f(x + width, y);
  glTexCoord2f(1, 0);
  glVertex2f(x + width, y + height);
  glTexCoord2f(0, 0);
  glVertex2f(x, y + height);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  
  // Draw HUD on top
  drawHUD();
}

// Move Method Definition
void Player::move(float dx, float dy, float &oldX, float &oldY)
{
  oldX = x;
  oldY = y;
  
  float newX = x + dx;
  float newY = y + dy;
  
  // Get screen dimensions
  int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
  int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
  
  // Clamp X
  if (newX < 0) newX = 0;
  if (newX + width > screenWidth) newX = screenWidth - width;
  
  // Clamp Y
  if (newY < 0) newY = 0;
  if (newY + height > screenHeight) newY = screenHeight - height;
  
  x = newX;
  y = newY;
}

// Getters and Setters Definitions
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
    if (hud != nullptr)
    {
        hud->setPos(x, y);
    }
}