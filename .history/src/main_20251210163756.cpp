#include <GL/glut.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
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
    STATE_PAUSED,
    STATE_GAME_OVER // ADDED: New state for win screen
};

GameState currentState = STATE_MENU;
GameState previousState = STATE_MENU;
int gameMode = 0; // 0 = 1VS1, 1 = COOP

// ADDED: Track Winner
int winnerID = 0; // 0=None, 1=Mario, 2=Luigi

// Timing variables
int lastTime = 0;
float deltaTime = 0.0f;

// Centralized match timer
float matchTime = 60.0f; 
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
// HELPER FUNCTIONS
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

void playWinSound()
{
    // Use the castle clear sound or any other win sound
    system("gst-launch-1.0 filesrc location=./assets/Sounds/stage_clear.wav ! decodebin ! autoaudiosink &");
}

// ADDED: Start/Restart Logic
void startPvPMatch()
{
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // 1. Reset Instances (Restores Health/Lives)
    Mario::destroyInstance();
    Luigi::destroyInstance();

    // 2. Load/Reload Map
    if (gameMap == nullptr) {
        gameMap = Map::getInstance("./assets/Maps/Original Map/original.jpg", screenWidth, screenHeight);
    }

    // 3. Create Players
    mario = Mario::getInstance(100, 145);
    luigi = Luigi::getInstance(200, 145);

    // 4. Link Enemies
    mario->setEnemy(luigi);
    luigi->setEnemy(mario);

    // 5. Reset Timers/Global Vars
    matchElapsed = 0.0f;
    matchTime = 60.0f;          
    matchTimerActive = false;   
    hurryUpSoundPlayed = false; 
    winnerID = 0;

    // 6. Setup Intro
    isIntroActive = true;
    introProgress = 0.0f;

    // Positioning logic
    int currentWidth = glutGet(GLUT_WINDOW_WIDTH);
    if (currentWidth == 0) currentWidth = 800; 

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

    // 7. Play Intro Sound
    system("gst-launch-1.0 filesrc location=./assets/Sounds/castle-clear.mp3 ! decodebin ! autoaudiosink &");

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    currentState = STATE_INTRO;
}

// ADDED: Draw Game Over Screen
void drawGameOverScreen()
{
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Dark Overlay
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f); 
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0, screenHeight);
    glEnd();

    // Determine Text
    std::string winText = "GAME OVER";
    if (winnerID == 1) winText = "MARIO WINS!";
    if (winnerID == 2) winText = "LUIGI WINS!";

    // Draw Winner Text (Yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;
    
    // Center calculations (approx)
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    drawText(centerX - 60, centerY + 30, winText, font);

    // Draw Instructions (White)
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(centerX - 80, centerY - 20, "Press ENTER to Try Again", GLUT_BITMAP_HELVETICA_18);
    drawText(centerX - 60, centerY - 50, "Press ESC to Quit", GLUT_BITMAP_HELVETICA_18);
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

    float x = (screenWidth - 70) / 2.0f; // Approx width
    float y = screenHeight / 2.0f;

    drawText(x, y, pausedText, font);
    drawText(x - 30, y - 30, "Press P to Resume", GLUT_BITMAP_HELVETICA_18);
}

void drawCentralTimer()
{
    if (!matchTimerActive) return;

    float remainingTime = matchTime - matchElapsed;
    if (remainingTime < 0) remainingTime = 0;

    int minutes = static_cast<int>(remainingTime) / 60;
    int seconds = static_cast<int>(remainingTime) % 60;

    char timerText[32];
    snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    float x = (screenWidth - 50) / 2.0f;
    float y = screenHeight - 40; 

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); 
    glBegin(GL_QUADS);
    glVertex2f(x - 15, y - 5);
    glVertex2f(x + 65, y - 5);
    glVertex2f(x + 65, y + 25);
    glVertex2f(x - 15, y + 25);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(x, y, timerText, GLUT_BITMAP_TIMES_ROMAN_24);
}

void updateIntroAnimation(float deltaTime)
{
    if (!isIntroActive) return;

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
        if (screenHeight == 0) screenHeight = 600;

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

        if (gameMap) gameMap->draw(zoomScale);
        if (mario) mario->draw();
        if (luigi) luigi->draw();

        glPopMatrix();
        if (gameMode == 0) drawCentralTimer();
    }
    else if (currentState == STATE_GAME)
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f; 
        lastTime = currentTime;

        // CHECK WIN CONDITION
        if (gameMode == 0) { // Only in PvP
            if (mario != nullptr && mario->getLives() <= 0) {
                winnerID = 2; // Luigi Wins
                currentState = STATE_GAME_OVER;
                playWinSound();
            }
            else if (luigi != nullptr && luigi->getLives() <= 0) {
                winnerID = 1; // Mario Wins
                currentState = STATE_GAME_OVER;
                playWinSound();
            }
        }

        if (gameMode == 0 && matchTimerActive)
        {
            matchElapsed += deltaTime;
            float remainingTime = matchTime - matchElapsed;
            if (remainingTime <= 10.0f && !hurryUpSoundPlayed) {
                playHurryUpSound();
                hurryUpSoundPlayed = true;
            }
            if (matchElapsed >= matchTime) {
                matchElapsed = matchTime;
                matchTimerActive = false;
                // Optional: Time Over Logic
            }
        }

        if (gameMap) gameMap->draw();
        
        if (mario) {
            mario->update(); 
            mario->draw();
        }
        if (gameMode == 0 && luigi) {
            luigi->update();
            luigi->draw();
        }
        if (gameMode == 0) drawCentralTimer();
    }
    else if (currentState == STATE_PAUSED)
    {
        if (gameMap) gameMap->draw();
        if (mario) mario->draw();
        if (gameMode == 0 && luigi) luigi->draw();
        if (gameMode == 0) drawCentralTimer();
        drawPauseMenu();
    }
    // ADDED: Game Over State
    else if (currentState == STATE_GAME_OVER) 
    {
        // Draw background game frozen
        if (gameMap) gameMap->draw();
        if (mario) mario->draw();
        if (gameMode == 0 && luigi) luigi->draw();
        
        drawGameOverScreen();
    }
    else if (currentState == STATE_MENU)
    {
        mainMenu->draw();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0); // ESC

    if (currentState == STATE_MENU)
    {
        if (key == 13) // ENTER
        {
            gameMode = mainMenu->getSelectedOption();
            if (gameMode == 0) {
                startPvPMatch();
            } else {
                // Coop setup (basic)
                int w = glutGet(GLUT_WINDOW_WIDTH);
                int h = glutGet(GLUT_WINDOW_HEIGHT);
                gameMap = Map::getInstance("./assets/Maps/Original Map/original.jpg", w, h);
                mario = Mario::getInstance(100, 145);
                currentState = STATE_GAME;
            }
        }
        else {
            mainMenu->handleInput(key);
        }
    }
    else if (currentState == STATE_GAME)
    {
        if (key == 'p' || key == 'P') {
            previousState = currentState;
            currentState = STATE_PAUSED;
            playPauseSound();
        } else {
            if (mario) {
                mario->setKeyState(key, true);
                if (key == ' ') mario->shootFireball();
            }
            if (luigi && gameMode == 0) {
                if (key == 'l' || key == 'L') luigi->setKeyState('z', true);
                if (key == 13) luigi->shootFireball();
            }
        }
    }
    else if (currentState == STATE_PAUSED)
    {
        if (key == 'p' || key == 'P') {
            currentState = previousState;
            playPauseSound();
            lastTime = glutGet(GLUT_ELAPSED_TIME);
        }
    }
    // ADDED: Game Over Input
    else if (currentState == STATE_GAME_OVER)
    {
        if (key == 13) // ENTER to Restart
        {
            startPvPMatch();
        }
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (currentState == STATE_GAME)
    {
        if (mario) mario->setKeyState(key, false);
        if ((key == 'l' || key == 'L') && luigi && gameMode == 0) luigi->setKeyState('z', false);
    }
}

void specialKeys(int key, int x, int y)
{
    if (currentState == STATE_GAME && gameMode == 0 && luigi)
    {
        switch (key) {
        case GLUT_KEY_LEFT: luigi->setKeyState('a', true); break;
        case GLUT_KEY_RIGHT: luigi->setKeyState('d', true); break;
        case GLUT_KEY_UP: luigi->setKeyState('w', true); break;
        case GLUT_KEY_DOWN: luigi->setKeyState('s', true); break;
        }
    }
}

void specialKeysUp(int key, int x, int y)
{
    if (currentState == STATE_GAME && gameMode == 0 && luigi)
    {
        switch (key) {
        case GLUT_KEY_LEFT: luigi->setKeyState('a', false); break;
        case GLUT_KEY_RIGHT: luigi->setKeyState('d', false); break;
        case GLUT_KEY_UP: luigi->setKeyState('w', false); break;
        case GLUT_KEY_DOWN: luigi->setKeyState('s', false); break;
        }
    }
}

void cleanup()
{
    Mario::destroyInstance();
    Luigi::destroyInstance();
    Map::destroyInstance();
    if (mainMenu) delete mainMenu;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Game");
    glutFullScreen();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);

    mainMenu = new Menu(w, h);
    mario = nullptr;
    luigi = nullptr;
    gameMap = nullptr;

    glutDisplayFunc(display);
    glutIdleFunc(display); 
    glutReshapeFunc(Map::handleReshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);

    atexit(cleanup);
    glutMainLoop();
    return 0;
}