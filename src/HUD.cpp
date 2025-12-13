#include "../include/HUD.h"

// helper function to draw text
void HUD::drawText(float x, float y, const std::string& text, void* font)
{
    glRasterPos2f(x, y);
    for (char c : text)
    {
        glutBitmapCharacter(font, c);
    }
}

// helper function to draw icon
void HUD::drawIcon(float x, float y, GLuint texture)
{
    if (texture == 0) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(x, y);
    glTexCoord2f(1, 1);
    glVertex2f(x + iconSize, y);
    glTexCoord2f(1, 0);
    glVertex2f(x + iconSize, y + iconSize);
    glTexCoord2f(0, 0);
    glVertex2f(x, y + iconSize);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

// helper function to draw heart with custom size
void HUD::drawHeart(float x, float y, GLuint texture, float size)
{
    if (texture == 0) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(x, y);
    glTexCoord2f(1, 1);
    glVertex2f(x + size, y);
    glTexCoord2f(1, 0);
    glVertex2f(x + size, y + size);
    glTexCoord2f(0, 0);
    glVertex2f(x, y + size);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

// constructor
HUD::HUD(float xPos, float yPos,
    const char* iconPath,
    int initialLives, int initialScore)
{
    x = xPos;
    y = yPos;
    iconSize = 40.0f;
    heartSize = 25.0f;  // size for heart icons
    
    lives = initialLives;
    score = initialScore;
    
    // load character icon
    iconTexture = TextureUtils::loadTexture(iconPath);
    
    // load heart icon
    heartTexture = TextureUtils::loadTexture("./assets/Maps/heart.png");
    
    powerUpTimer = 0.0f;
}

// destructor
HUD::~HUD()
{
    if (iconTexture != 0)
        glDeleteTextures(1, &iconTexture);
    if (heartTexture != 0)
        glDeleteTextures(1, &heartTexture);
}

// draw the HUD
void HUD::draw()
{
    // draw icon
    drawIcon(x, y, iconTexture);
    
    // disable texturing for text
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // draw Lives as hearts (next to icon)
    float heartStartX = x + iconSize + 10;
    float heartY = y + 20;  // vertically centered with icon
    float heartSpacing = heartSize + 5;  // 5 pixels between hearts
    
    // draw hearts for each life (max 5 displayed)
    int heartsToShow = lives > 5 ? 5 : lives;
    for (int i = 0; i < heartsToShow; i++)
    {
        drawHeart(heartStartX + (i * heartSpacing), heartY, heartTexture, heartSize);
    }
    
    // if more than 5 lives, show "+X" text
    if (lives > 5)
    {
        std::string extraLives = "+" + std::to_string(lives - 5);
        drawText(heartStartX + (5 * heartSpacing), heartY + 5, extraLives, GLUT_BITMAP_HELVETICA_18);
    }
    
    // draw Score (below hearts)
    std::string scoreText = "Score: " + std::to_string(score);
    drawText(x + iconSize + 10, y + 5, scoreText, GLUT_BITMAP_HELVETICA_18);

    // draw powerup timer
    if (powerUpTimer > 0.0f) {
        glColor3f(1.0f, 1.0f, 0.0f); // yellow color
        std::string timerText = "Power: " + std::to_string((int)powerUpTimer + 1) + "s";
        drawText(x + iconSize + 10, y - 15, timerText, GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 1.0f, 1.0f); // reset color
    }
}

// update methods
void HUD::setLives(int l) { lives = l; }
void HUD::setScore(int s) { score = s; }
void HUD::addScore(int points) { score += points; }
void HUD::loseLife() { if (lives > 0) lives--; }
void HUD::gainLife() { lives++; }

// getters
int HUD::getLives() const { return lives; }
int HUD::getScore() const { return score; }
bool HUD::isAlive() const { return lives > 0; }
