#include <GL/glut.h>
#include <vector>
#include <string>
#include <cstring>
#include "../include/Player.h"
#include "../include/TextureUtils.h" 
#include "../include/Mario.h"
#include "../include/Luigi.h"
#include "../include/Map.h"
#include "../include/HUD.h"
#include "../include/Menu.h"

// The STB_IMAGE_IMPLEMENTATION must be defined in exactly ONE .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

// ------------------------------
// GLOBAL OBJECTS
// ------------------------------
Mario *mario;
Luigi *luigi;
Map *gameMap;
Menu *mainMenu;

enum GameState
{
    STATE_MENU,
    STATE_INTRO, 
    STATE_GAME,
    STATE_PAUSED
};

GameState currentState = STATE_MENU;
GameState previousState = STATE_MENU;
int gameMode = 0; // 0 = 1VS1, 1 = COOP

// Timing variables for the timer
int lastTime = 0;
float deltaTime = 0.0f;

// Centralized match timer for 1VS1 mode
float matchTime = 60.0f; // 1 minute countdown
float matchElapsed = 0.0f;
bool matchTimerActive = false;
bool hurryUpSoundPlayed = false;

// Intro animation variables
bool isIntroActive = false;
float introProgress = 0.0f;  
float introDuration = 6.0f;  
float zoomScale = 1.0f;      
float marioStartX = -100.0f; 
float luigiStartX = 0.0f;    
float marioTargetX = 100.0f; 
float luigiTargetX = 200.0f; 

// HUD animation variables
float marioHUDStartX = -200.0f;
float marioHUDTargetX = 20.0f;
float luigiHUDStartX = 0.0f;  
float luigiHUDTargetX = 0.0f; 

// ------------------------------
// GLUT FUNCTIONS
// ------------------------------

void drawText(float x, float y, const std::string &text, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for (char c : text)
    {
        glutBitmapCharacter(font, c);
    }
}

void playHurryUpSound()
{
    system("gst-launch-1.0 filesrc location=./assets/Sounds/hurry-up.mp3 ! decodebin ! autoaudiosink &");
}

void playPauseSound()
{
    system("gst-launch-1.0 filesrc location=./assets/Sounds/pause.mp3 ! decodebin ! autoaudiosink &");
}

void drawPauseMenu()
{
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_TEXTURE_2D);

    glColor4f(0.0f, 0.0f, 0.0f, 0.6f); 
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0, screenHeight);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); 
    std::string pausedText = "PAUSED";
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;

    int charWidth = 15;
    float textWidth = pausedText.length() * charWidth;
    float x = (screenWidth - textWidth) / 2.0f;
    float y = screenHeight / 2.0f;

    drawText(x, y, pausedText, font);

    std::string instructionText = "Press P to Resume";
    float instructionWidth = instructionText.length() * 10;
    float instructionX = (screenWidth - instructionWidth) / 2.0f;
    float instructionY = y - 30;

    drawText(instructionX, instructionY, instructionText, GLUT_BITMAP_HELVETICA_18);
}

void drawCentralTimer()
{
    if (!matchTimerActive)
        return;

    float remainingTime = matchTime - matchElapsed;
    if (remainingTime < 0)
        remainingTime = 0;

    int minutes = static_cast<int>(remainingTime) / 60;
    int seconds = static_cast<int>(remainingTime) % 60;

    char timerText[32];
    snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    int textLength = strlen(timerText);
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;
    int charWidth = 15; 
    float textWidth = textLength * charWidth;

    float x = (screenWidth - textWidth) / 2.0f;
    float y = screenHeight - 40; 

    glDisable(GL_TEXTURE_2D);

    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); 
    float padding = 15.0f;
    glBegin(GL_QUADS);
    glVertex2f(x - padding, y - 5);
    glVertex2f(x + textWidth + padding, y - 5);
    glVertex2f(x + textWidth + padding, y + 25);
    glVertex2f(x - padding, y + 25);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(x, y, timerText, font);
}

void updateIntroAnimation(float deltaTime)
{
    if (!isIntroActive)
        return;

    introProgress += deltaTime / introDuration;

    if (introProgress >= 1.0f)
    {
        introProgress = 1.0f;
        isIntroActive = false;
        currentState = STATE_GAME;

        mario->setPos(marioTargetX, 145);
        luigi->setPos(luigiTargetX, 145);

        matchTimerActive = true;
        lastTime = glutGet(GLUT_ELAPSED_TIME);
    }
    else
    {
        float t = introProgress;
        float eased = 1.0f - (1.0f - t) * (1.0f - t); 

        float marioX = marioStartX + (marioTargetX - marioStartX) * eased;
        float luigiX = luigiStartX + (luigiTargetX - luigiStartX) * eased;

        mario->setPos(marioX, 145);
        luigi->setPos(luigiX, 145);

        float marioHUDX = marioHUDStartX + (marioHUDTargetX - marioHUDStartX) * eased;
        float luigiHUDX = luigiHUDStartX + (luigiHUDTargetX - luigiHUDStartX) * eased;

        int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
        if (screenHeight == 0)
            screenHeight = 600;

        mario->setHUDPos(marioHUDX, screenHeight - 90);
        luigi->setHUDPos(luigiHUDX, screenHeight - 90);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentState == STATE_INTRO)
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        updateIntroAnimation(deltaTime);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
        int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

        glTranslatef(screenWidth / 2.0f, screenHeight / 2.0f, 0);
        glScalef(zoomScale, zoomScale, 1.0f);
        glTranslatef(-screenWidth / 2.0f, -screenHeight / 2.0f, 0);

        if (gameMap != nullptr)
            gameMap->draw(zoomScale);

        if (mario != nullptr)
        {
            mario->draw();
        }

        if (luigi != nullptr)
        {
            luigi->draw();
        }

        glPopMatrix();

        if (gameMode == 0)
        {
            drawCentralTimer();
        }
    }
    else if (currentState == STATE_GAME)
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f; 
        lastTime = currentTime;

        if (gameMode == 0 && matchTimerActive)
        {
            matchElapsed += deltaTime;

            float remainingTime = matchTime - matchElapsed;
            if (remainingTime <= 10.0f && !hurryUpSoundPlayed)
            {
                playHurryUpSound();
                hurryUpSoundPlayed = true;
            }

            if (matchElapsed >= matchTime)
            {
                matchElapsed = matchTime;
                matchTimerActive = false;
            }
        }

        if (gameMap != nullptr)
            gameMap->draw();

        if (mario != nullptr)
        {
            mario->update(); 
            mario->draw();
        }

        if (gameMode == 0 && luigi != nullptr)
        {
            luigi->update();
            luigi->draw();
        }

        if (gameMode == 0)
        {
            drawCentralTimer();
        }
    }
    else if (currentState == STATE_PAUSED)
    {
        if (gameMap != nullptr)
            gameMap->draw();

        if (mario != nullptr)
        {
            mario->draw();
        }

        if (gameMode == 0 && luigi != nullptr)
        {
            luigi->draw();
        }

        if (gameMode == 0)
        {
            drawCentralTimer();
        }

        drawPauseMenu();
    }
    else if (currentState == STATE_MENU)
    {
        mainMenu->draw();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) // ESC key
        exit(0);

    if (currentState == STATE_MENU)
    {
        if (key == 13) // ENTER key
        {
            gameMode = mainMenu->getSelectedOption();

            int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
            int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

            gameMap = Map::getInstance("./assets/Maps/Original Map/original.jpg", screenWidth, screenHeight);
            mario = Mario::getInstance(100, 145);

            if (gameMode == 0)
            {
                luigi = Luigi::getInstance(200, 145);

                matchElapsed = 0.0f;
                matchTime = 60.0f;          
                matchTimerActive = false;   
                hurryUpSoundPlayed = false; 

                isIntroActive = true;
                introProgress = 0.0f;

                int currentWidth = glutGet(GLUT_WINDOW_WIDTH);
                if (currentWidth == 0)
                    currentWidth = 800; 

                marioTargetX = 100.0f;                
                luigiTargetX = currentWidth - 100.0f; 

                marioStartX = -100.0f;               
                luigiStartX = currentWidth + 100.0f; 

                mario->setPos(marioStartX, 145);
                mario->setFacingRight(true); 
                luigi->setPos(luigiStartX, 145);
                luigi->setFacingRight(false); 

                marioHUDStartX = -200.0f; 
                marioHUDTargetX = 20.0f;  

                luigiHUDStartX = currentWidth + 50.0f;   
                luigiHUDTargetX = currentWidth - 150.0f; 

                mario->setHUDPos(marioHUDStartX, screenHeight - 90);
                luigi->setHUDPos(luigiHUDStartX, screenHeight - 90);

                system("gst-launch-1.0 filesrc location=./assets/Sounds/castle-clear.mp3 ! decodebin ! autoaudiosink &");

                lastTime = glutGet(GLUT_ELAPSED_TIME);
                currentState = STATE_INTRO;
            }
            else
            {
                currentState = STATE_GAME;
            }
        }
        else
        {
            mainMenu->handleInput(key);
        }
    }
    else if (currentState == STATE_GAME)
    {
        if (key == 'p' || key == 'P')
        {
            previousState = currentState;
            currentState = STATE_PAUSED;
            playPauseSound();
        }
        else
        {
            if (mario != nullptr)
            {
                mario->setKeyState(key, true);
                
                // ADDED: Fireball shooting (Spacebar)
                if (key == ' ') {
                    mario->shootFireball();
                }
            }

            if ((key == 'l' || key == 'L') && luigi