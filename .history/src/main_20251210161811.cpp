#include <GL/glut.h>
#include <vector>
#include <string>
#include <cstring>
#include "../include/Player.h"
#include "../include/TextureUtils.h" // Only needed for the stb_image define below
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
    STATE_INTRO, // Intro animation state
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
bool hurryUpSoundPlayed = false; // Track if hurry-up sound has been played

// Intro animation variables
bool isIntroActive = false;
float introProgress = 0.0f;  // 0.0 to 1.0
float introDuration = 6.0f;  // 6.0 seconds for intro
float zoomScale = 1.0f;      // No zoom - constant 1.0x
float marioStartX = -100.0f; // Off-screen left
float luigiStartX = 0.0f;    // Will be set to off-screen right
float marioTargetX = 100.0f; // Final position
float luigiTargetX = 200.0f; // Final position

// HUD animation variables
float marioHUDStartX = -200.0f;
float marioHUDTargetX = 20.0f;
float luigiHUDStartX = 0.0f;  // Will be set based on screen width
float luigiHUDTargetX = 0.0f; // Will be set based on screen width

// ------------------------------
// GLUT FUNCTIONS
// ------------------------------

// Helper function to draw text
void drawText(float x, float y, const std::string &text, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for (char c : text)
    {
        glutBitmapCharacter(font, c);
    }
}

// Function to play hurry-up sound
void playHurryUpSound()
{
    system("gst-launch-1.0 filesrc location=./assets/Sounds/hurry-up.mp3 ! decodebin ! autoaudiosink &");
}

// Function to play pause sound
void playPauseSound()
{
    system("gst-launch-1.0 filesrc location=./assets/Sounds/pause.mp3 ! decodebin ! autoaudiosink &");
}

// Function to draw pause menu
void drawPauseMenu()
{
    // Get screen dimensions
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Disable texturing
    glDisable(GL_TEXTURE_2D);

    // Draw semi-transparent overlay
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f); // Semi-transparent black
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0, screenHeight);
    glEnd();

    // Draw "PAUSED" text in center
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    std::string pausedText = "PAUSED";
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;

    // Calculate text position (centered)
    int charWidth = 15;
    float textWidth = pausedText.length() * charWidth;
    float x = (screenWidth - textWidth) / 2.0f;
    float y = screenHeight / 2.0f;

    drawText(x, y, pausedText, font);

    // Draw instruction text
    std::string instructionText = "Press P to Resume";
    float instructionWidth = instructionText.length() * 10;
    float instructionX = (screenWidth - instructionWidth) / 2.0f;
    float instructionY = y - 30;

    drawText(instructionX, instructionY, instructionText, GLUT_BITMAP_HELVETICA_18);
}

// Function to draw centralized timer
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

    // Get screen dimensions
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Calculate text width (approximate)
    int textLength = strlen(timerText);
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;
    int charWidth = 15; // Approximate width per character for this font
    float textWidth = textLength * charWidth;

    // Center position
    float x = (screenWidth - textWidth) / 2.0f;
    float y = screenHeight - 40; // 40 pixels from top

    // Disable texturing for text
    glDisable(GL_TEXTURE_2D);

    // Draw background box for better visibility
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Semi-transparent black
    float padding = 15.0f;
    glBegin(GL_QUADS);
    glVertex2f(x - padding, y - 5);
    glVertex2f(x + textWidth + padding, y - 5);
    glVertex2f(x + textWidth + padding, y + 25);
    glVertex2f(x - padding, y + 25);
    glEnd();

    // Draw text in white
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(x, y, timerText, font);
}

// Function to update intro animation
void updateIntroAnimation(float deltaTime)
{
    if (!isIntroActive)
        return;

    introProgress += deltaTime / introDuration;

    if (introProgress >= 1.0f)
    {
        // Animation complete
        introProgress = 1.0f;
        isIntroActive = false;
        currentState = STATE_GAME;

        // Set final positions
        mario->setPos(marioTargetX, 145);
        luigi->setPos(luigiTargetX, 145);

        // Start the match timer
        matchTimerActive = true;
        lastTime = glutGet(GLUT_ELAPSED_TIME);
    }
    else
    {
        // Ease-out function for smooth animation
        float t = introProgress;
        float eased = 1.0f - (1.0f - t) * (1.0f - t); // Quadratic ease-out

        // Update character positions only (no zoom)
        float marioX = marioStartX + (marioTargetX - marioStartX) * eased;
        float luigiX = luigiStartX + (luigiTargetX - luigiStartX) * eased;

        mario->setPos(marioX, 145);
        luigi->setPos(luigiX, 145);

        // Update HUD positions
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
        // Calculate delta time
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Update intro animation
        updateIntroAnimation(deltaTime);

        // Save current matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // Get screen dimensions
        int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
        int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Apply zoom transformation
        // Translate to center, scale, then translate back
        glTranslatef(screenWidth / 2.0f, screenHeight / 2.0f, 0);
        glScalef(zoomScale, zoomScale, 1.0f);
        glTranslatef(-screenWidth / 2.0f, -screenHeight / 2.0f, 0);

        // Draw the map background with scale compensation
        if (gameMap != nullptr)
            gameMap->draw(zoomScale);

        // Draw Mario (no physics update during intro)
        if (mario != nullptr)
        {
            mario->draw();
        }

        // Draw Luigi
        if (luigi != nullptr)
        {
            luigi->draw();
        }

        // Restore matrix
        glPopMatrix();

        // Draw timer (not affected by zoom)
        if (gameMode == 0)
        {
            drawCentralTimer();
        }
    }
    else if (currentState == STATE_GAME)
    {
        // Calculate delta time
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (currentTime - lastTime) / 1000.0f; // Convert to seconds
        lastTime = currentTime;

        // Update centralized match timer in 1VS1 mode
        if (gameMode == 0 && matchTimerActive)
        {
            matchElapsed += deltaTime;

            // Check if timer hit 10 seconds and play hurry-up sound
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

        // Draw the map background first
        if (gameMap != nullptr)
            gameMap->draw();

        // Draw Mario on top
        if (mario != nullptr)
        {
            mario->update(); // Update physics
            mario->draw();
        }

        // Draw Luigi if in 1VS1 mode
        if (gameMode == 0 && luigi != nullptr)
        {
            luigi->update();
            luigi->draw();
        }

        // Draw centralized timer on top of everything
        if (gameMode == 0)
        {
            drawCentralTimer();
        }
    }
    else if (currentState == STATE_PAUSED)
    {
        // Draw the game in the background (frozen)
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

        // Draw centralized timer (frozen)
        if (gameMode == 0)
        {
            drawCentralTimer();
        }

        // Draw pause menu overlay on top
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
            // Get selected mode
            gameMode = mainMenu->getSelectedOption();

            // Initialize Game Assets
            int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
            int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

            // Initialize the map
            gameMap = Map::getInstance("./assets/Maps/Original Map/original.jpg", screenWidth, screenHeight);

            // Initialize Mario (Player 1)
            mario = Mario::getInstance(100, 145);

            // Initialize Luigi (Player 2) if 1VS1
            if (gameMode == 0)
            {
                luigi = Luigi::getInstance(200, 145);

                // Initialize centralized match timer for 1VS1 mode (1 minute)
                matchElapsed = 0.0f;
                matchTime = 60.0f;          // 60 seconds = 1 minute
                matchTimerActive = false;   // Don't start timer until intro finishes
                hurryUpSoundPlayed = false; // Reset sound flag

                // Initialize intro animation
                isIntroActive = true;
                introProgress = 0.0f;

                // Get current window width for positioning
                int currentWidth = glutGet(GLUT_WINDOW_WIDTH);
                if (currentWidth == 0)
                    currentWidth = 800; // Fallback

                // Set target positions (final positions)
                marioTargetX = 100.0f;                // Left side
                luigiTargetX = currentWidth - 100.0f; // Right side

                // Set starting positions (off-screen) - Face each other
                marioStartX = -100.0f;               // Off-screen left
                luigiStartX = currentWidth + 100.0f; // Off-screen right

                // Set initial positions
                mario->setPos(marioStartX, 145);
                mario->setFacingRight(true); // Make Mario look right towards Luigi
                luigi->setPos(luigiStartX, 145);
                luigi->setFacingRight(false); // Make Luigi look left towards Mario

                // Initialize HUD animation positions
                marioHUDStartX = -200.0f; // Off-screen left
                marioHUDTargetX = 20.0f;  // Final left position

                luigiHUDStartX = currentWidth + 50.0f;   // Off-screen right
                luigiHUDTargetX = currentWidth - 150.0f; // Final right position

                // Set initial HUD positions
                mario->setHUDPos(marioHUDStartX, screenHeight - 90);
                luigi->setHUDPos(luigiHUDStartX, screenHeight - 90);

                // Play intro sound
                system("gst-launch-1.0 filesrc location=./assets/Sounds/castle-clear.mp3 ! decodebin ! autoaudiosink &");

                // Initialize timing
                lastTime = glutGet(GLUT_ELAPSED_TIME);

                // Start intro state
                currentState = STATE_INTRO;
            }
            else
            {
                // For COOP mode, go directly to game
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
        // Handle pause key (P or p)
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
            }

            // Luigi Sprint (Map 'l' to 'z')
            if ((key == 'l' || key == 'L') && luigi != nullptr && gameMode == 0)
            {
                luigi->setKeyState('z', true);
            }
        }
    }
    else if (currentState == STATE_PAUSED)
    {
        // Handle resume key (P or p)
        if (key == 'p' || key == 'P')
        {
            currentState = previousState;
            playPauseSound();
            // Reset lastTime to prevent large deltaTime jump
            lastTime = glutGet(GLUT_ELAPSED_TIME);
        }
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (currentState == STATE_GAME)
    {
        if (mario != nullptr)
        {
            mario->setKeyState(key, false);
        }

        // Luigi Sprint Release
        if ((key == 'l' || key == 'L') && luigi != nullptr && gameMode == 0)
        {
            luigi->setKeyState('z', false);
        }
    }
}

// Special keys for Luigi (Arrows)
void specialKeys(int key, int x, int y)
{
    if (currentState == STATE_GAME && gameMode == 0 && luigi != nullptr)
    {
        switch (key)
        {
        case GLUT_KEY_LEFT:
            luigi->setKeyState('a', true); // Map Arrow Left to 'a' logic internally
            mario->~Player=
            break;
        case GLUT_KEY_RIGHT:
            luigi->setKeyState('d', true); // Map Arrow Right to 'd' logic internally
            break;
        case GLUT_KEY_UP:
            luigi->setKeyState('w', true); // Map Arrow Up to 'w' logic internally
            break;
        case GLUT_KEY_DOWN:
            luigi->setKeyState('s', true); // Map Arrow Down to 's' logic internally
            break;
        }
    }
}

void specialKeysUp(int key, int x, int y)
{
    if (currentState == STATE_GAME && gameMode == 0 && luigi != nullptr)
    {
        switch (key)
        {
        case GLUT_KEY_LEFT:
            luigi->setKeyState('a', false);
            break;
        case GLUT_KEY_RIGHT:
            luigi->setKeyState('d', false);
            break;
        case GLUT_KEY_UP:
            luigi->setKeyState('w', false);
            break;
        case GLUT_KEY_DOWN:
            luigi->setKeyState('s', false);
            break;
        }
    }
}

// Cleanup function
void cleanup()
{
    Mario::destroyInstance();
    Luigi::destroyInstance();
    Map::destroyInstance();
    if (mainMenu != nullptr)
        delete mainMenu;
}

// ------------------------------
// MAIN
// ------------------------------
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Game");
    glutFullScreen();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Get actual screen dimensions after fullscreen
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    // Initialize Menu
    mainMenu = new Menu(screenWidth, screenHeight);

    // Initialize pointers to null
    mario = nullptr;
    luigi = nullptr;
    gameMap = nullptr;

    glutDisplayFunc(display);
    glutIdleFunc(display); // Continuous update
    glutReshapeFunc(Map::handleReshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);

    // Register cleanup function
    atexit(cleanup);
    glutMainLoop();
    return 0;
}