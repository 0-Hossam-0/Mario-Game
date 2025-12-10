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

// ... (Keep Global Objects and Variables exactly the same) ...
Mario *mario;
Luigi *luigi;
Map *gameMap;
Menu *mainMenu;

enum GameState { STATE_MENU, STATE_INTRO, STATE_GAME, STATE_PAUSED };
GameState currentState = STATE_MENU;
GameState previousState = STATE_MENU;
int gameMode = 0;

int lastTime = 0;
float deltaTime = 0.0f;
float matchTime = 60.0f;
float matchElapsed = 0.0f;
bool matchTimerActive = false;
bool hurryUpSoundPlayed = false;

// ... (Keep Intro variables same) ...
bool isIntroActive = false;
float introProgress = 0.0f;  
float introDuration = 6.0f;  
float zoomScale = 1.0f;      
float marioStartX = -100.0f; 
float luigiStartX = 0.0f;    
float marioTargetX = 100.0f; 
float luigiTargetX = 200.0f; 
float marioHUDStartX = -200.0f;
float marioHUDTargetX = 20.0f;
float luigiHUDStartX = 0.0f;  
float luigiHUDTargetX = 0.0f; 

// ... (Keep helper functions like drawText, playSound, etc. same) ...
// (Omitting for brevity - assuming they exist from previous step)
void drawText(float x, float y, const std::string &text, void *font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(font, c);
}
void playHurryUpSound() { system("gst-launch-1.0 filesrc location=./assets/Sounds/hurry-up.mp3 ! decodebin ! autoaudiosink &"); }
void playPauseSound() { system("gst-launch-1.0 filesrc location=./assets/Sounds/pause.mp3 ! decodebin ! autoaudiosink &"); }

// ... (Keep drawPauseMenu, drawCentralTimer, updateIntroAnimation, display same) ...
// (Omitting these chunks because they didn't change logic, just copy previous main.cpp content here)

// I will re-paste display() just to be safe it is complete
void updateIntroAnimation(float deltaTime); // Forward decl
void drawCentralTimer();
void drawPauseMenu();

void display()
{
    // ... (Same display logic as before) ...
    // Copy the display function from the previous response entirely.
    // It calls mario->update() and luigi->update(), which now handle collision internally.
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentState == STATE_INTRO) {
        // ... (Intro logic same) ...
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        updateIntroAnimation(deltaTime);

        glMatrixMode(GL_MODELVIEW); glPushMatrix();
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
    else if (currentState == STATE_GAME) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f; 
        lastTime = currentTime;

        if (gameMode == 0 && matchTimerActive) {
            matchElapsed += deltaTime;
            float remainingTime = matchTime - matchElapsed;
            if (remainingTime <= 10.0f && !hurryUpSoundPlayed) {
                playHurryUpSound();
                hurryUpSoundPlayed = true;
            }
            if (matchElapsed >= matchTime) {
                matchElapsed = matchTime;
                matchTimerActive = false;
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
    else if (currentState == STATE_PAUSED) {
        if (gameMap) gameMap->draw();
        if (mario) mario->draw();
        if (gameMode == 0 && luigi) luigi->draw();
        if (gameMode == 0) drawCentralTimer();
        drawPauseMenu();
    }
    else if (currentState == STATE_MENU) {
        mainMenu->draw();
    }
    glutSwapBuffers();
}

// ... (Helper functions implementations need to be present) ...
// (Assuming you have them from the previous file content I sent)
// RE-INSERT updateIntroAnimation implementation here...

void updateIntroAnimation(float deltaTime) {
    if (!isIntroActive) return;
    introProgress += deltaTime / introDuration;
    if (introProgress >= 1.0f) {
        introProgress = 1.0f;
        isIntroActive = false;
        currentState = STATE_GAME;
        mario->setPos(marioTargetX, 145);
        luigi->setPos(luigiTargetX, 145);
        matchTimerActive = true;
        lastTime = glutGet(GLUT_ELAPSED_TIME);
    } else {
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

// UPDATED KEYBOARD FUNCTION
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);

    if (currentState == STATE_MENU)
    {
        if (key == 13) // ENTER key for Menu Select
        {
            gameMode = mainMenu->getSelectedOption();
            int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
            int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

            gameMap = Map::getInstance("./assets/Maps/Original Map/original.jpg", screenWidth, screenHeight);
            mario = Mario::getInstance(100, 145);

            if (gameMode == 0)
            {
                luigi = Luigi::getInstance(200, 145);
                
                // ADDED: Tell them who their enemy is!
                mario->setEnemy(luigi);
                luigi->setEnemy(mario);

                matchElapsed = 0.0f;
                matchTime = 60.0f;          
                matchTimerActive = false;   
                hurryUpSoundPlayed = false; 

                isIntroActive = true;
                introProgress = 0.0f;

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
            if (mario != nullptr) {
                mario->setKeyState(key, true);
                if (key == ' ') mario->shootFireball();
            }

            if (luigi != nullptr && gameMode == 0) {
                if (key == 'l' || key == 'L') luigi->setKeyState('z', true);
                
                // ADDED: Enter key (13) for Luigi Shoot
                if (key == 13) luigi->shootFireball();
            }
        }
    }
    else if (currentState == STATE_PAUSED)
    {
        if (key == 'p' || key == 'P')
        {
            currentState = previousState;
            playPauseSound();
            lastTime = glutGet(GLUT_ELAPSED_TIME);
        }
    }
}

// ... (Keep keyboardUp, specialKeys, specialKeysUp, cleanup, main same) ...
void keyboardUp(unsigned char key, int x, int y) {
    if (currentState == STATE_GAME) {
        if (mario) mario->setKeyState(key, false);
        if ((key == 'l' || key == 'L') && luigi && gameMode == 0) luigi->setKeyState('z', false);
    }
}
void specialKeys(int key, int x, int y) {
    if (currentState == STATE_GAME && gameMode == 0 && luigi) {
        switch (key) {
        case GLUT_KEY_LEFT: luigi->setKeyState('a', true); break;
        case GLUT_KEY_RIGHT: luigi->setKeyState('d', true); break;
        case GLUT_KEY_UP: luigi->setKeyState('w', true); break;
        case GLUT_KEY_DOWN: luigi->setKeyState('s', true); break;
        }
    }
}
void specialKeysUp(int key, int x, int y) {
    if (currentState == STATE_GAME && gameMode == 0 && luigi) {
        switch (key) {
        case GLUT_KEY_LEFT: luigi->setKeyState('a', false); break;
        case GLUT_KEY_RIGHT: luigi->setKeyState('d', false); break;
        case GLUT_KEY_UP: luigi->setKeyState('w', false); break;
        case GLUT_KEY_DOWN: luigi->setKeyState('s', false); break;
        }
    }
}
void cleanup() {
    Mario::destroyInstance();
    Luigi::destroyInstance();
    Map::destroyInstance();
    if (mainMenu) delete mainMenu;
}
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Game");
    glutFullScreen();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    mainMenu = new Menu(screenWidth, screenHeight);
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