//-----------------------------------------------------------------------------
// Engine.cpp
// 10/3/08
//
// Ian Stewart
// istewart@calpoly.edu
//
//
//-----------------------------------------------------------------------------

#include "Engine.h"

int Engine::width;
int Engine::height;

bool Engine::keyboardButtonStates[MAX_KEYS];
bool Engine::mouseStates[3];
Point2D Engine::mousePosition;

const char* Engine::gameMode;

Engine* Engine::instance;


//-----------------------------------------------------------------------------
// Constructor
Engine::Engine( int w, int h, int x, int y, char * title,
                        unsigned int mode, const char* gameMode )
{
    // register this instance for static to Class callback
    instance = this;

    width = w;
    height = h;

    this->title = title;
    this->gameMode = gameMode;

    // initialize glut
    char * argv[1];
    argv[0] = "run";
    int argc = 1;
    glutInit(&argc, argv);

    // Initialize the window
    glutInitDisplayMode(mode);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x,y);

    glutCreateWindow(title);

    // register functions
    init();
}

Engine::~Engine()
{
}

void Engine::init()
{
    // ignore key repeats
    glutIgnoreKeyRepeat(1);

    //alpha blend stuff for gui
    //glEnable(GL_BLEND); //Enable alpha blending
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

    // register callbacks
    glutDisplayFunc(hDisplay);
    glutKeyboardFunc(hKeyboardDown);
    glutKeyboardUpFunc(hKeyboardUp);
    glutSpecialFunc(hSpecialDown);
    glutSpecialUpFunc(hSpecialUp);
    glutMouseFunc(hMouse);
    glutPassiveMotionFunc(hPassiveMotion);
    glutMotionFunc(hPassiveMotion);
    glutReshapeFunc(hReshape);
}


//-----------------------------------------------------------------------------
// Public Methods
void Engine::callMainLoop()
{
    glutTimerFunc(10, Engine::hTimerCallback, 0);
    glutMainLoop();
}

bool Engine::getKeyState(KeyEnum k)
{
    return keyboardButtonStates[k];
}
bool Engine::getMouseKeyState(GLenum k)
{
    switch(k)
    {
        case GLUT_LEFT_BUTTON:
            return mouseStates[0];
        case GLUT_MIDDLE_BUTTON:
            return mouseStates[1];
        case GLUT_RIGHT_BUTTON:
            return mouseStates[2];
        default:
            break;
    }
    return false;
}
Point2D Engine::getMousePosition()
{
    return mousePosition;
}


//-----------------------------------------------------------------------------
// Display handlers
void Engine::hDisplay(void)
{
    instance->display();

    // display asap
    glutPostRedisplay();
}
void Engine::hReshape(int w, int h)
{
    width = w;
    height = h;

    instance->Reshape(w, h);

    // post redisplay, RenderManager handles viewport shite
    glutPostRedisplay();
}


//-----------------------------------------------------------------------------
// Input handlers
void Engine::hKeyboardDown(unsigned char key, int x, int y)
{
    switch(key)
    {
        // KEY_W
        case 'w':
        case 'W':
            keyboardButtonStates[KEY_W] = true;
            break;
        // KEY_A
        case 'a':
        case 'A':
            keyboardButtonStates[KEY_A] = true;
            break;
        // KEY_S
        case 's':
        case 'S':
            keyboardButtonStates[KEY_S] = true;
            break;
        // KEY_D
        case 'd':
        case 'D':
            keyboardButtonStates[KEY_D] = true;
            break;
        case 'q':
        case 'Q':
            keyboardButtonStates[KEY_Q] = true;
            break;
        case 'z':
        case 'Z':
            keyboardButtonStates[KEY_Z] = true;
            break;
        case 'x':
        case 'X':
            keyboardButtonStates[KEY_X] = true;
            break;
        case 'c':
        case 'C':
            keyboardButtonStates[KEY_C] = true;
            break;
        case 'o':
        case 'O':
            keyboardButtonStates[KEY_O] = true;
            break;
        case 'p':
        case 'P':
            keyboardButtonStates[KEY_P] = true;
            break;
        case 'n':
        case 'N':
            keyboardButtonStates[KEY_N] = true;
            break;
        case 'j':
        case 'J':
            keyboardButtonStates[KEY_J] = true;
            break;
        case 'm':
        case 'M':
            keyboardButtonStates[KEY_M] = true;
            break;
        case 'k':
        case 'K':
            keyboardButtonStates[KEY_K] = true;
            break;
        // KEY_ESC
        case '27':
            keyboardButtonStates[KEY_ESC] = true;
            break;
        // KEY_SPACE
        case ' ':
            keyboardButtonStates[KEY_SPACE] = true;
            break;
        default:
            break;
   }

}
void Engine::hSpecialDown(int key, int x, int y)
{
    switch (key)
    {
        // KEY_F10
        case GLUT_KEY_F10:
            keyboardButtonStates[KEY_F10] = true;
            break;
        // KEY_F11
        case GLUT_KEY_F11:
            keyboardButtonStates[KEY_F11] = true;

            // fullscreen mode
            toggleGameMode();

            break;
        // KEY_F12
        case GLUT_KEY_F12:
            keyboardButtonStates[KEY_F12] = true;
            glutDestroyWindow(glutGetWindow());
            exit(0);
            break;
        default:
            break;
    }

}

void Engine::hKeyboardUp(unsigned char key, int x, int y)
{
    switch(key)
    {
        // KEY_W
        case 'w':
        case 'W':
            keyboardButtonStates[KEY_W] = false;
            break;
        // KEY_A
        case 'a':
        case 'A':
            keyboardButtonStates[KEY_A] = false;
            break;
        // KEY_S
        case 's':
        case 'S':
            keyboardButtonStates[KEY_S] = false;
            break;
        // KEY_D
        case 'd':
        case 'D':
            keyboardButtonStates[KEY_D] = false;
            break;
        case 'q':
        case 'Q':
            keyboardButtonStates[KEY_Q] = false;
            break;
        case 'z':
        case 'Z':
            keyboardButtonStates[KEY_Z] = false;
            break;
        case 'x':
        case 'X':
            keyboardButtonStates[KEY_X] = false;
            break;
        case 'c':
        case 'C':
            keyboardButtonStates[KEY_C] = false;
            break;
        case 'o':
        case 'O':
            keyboardButtonStates[KEY_O] = false;
            break;
        case 'p':
        case 'P':
            keyboardButtonStates[KEY_P] = false;
            break;
        case 'n':
        case 'N':
            keyboardButtonStates[KEY_N] = false;
            break;
        case 'j':
        case 'J':
            keyboardButtonStates[KEY_J] = false;
            break;
        case 'm':
        case 'M':
            keyboardButtonStates[KEY_M] = false;
            break;
        case 'k':
        case 'K':
            keyboardButtonStates[KEY_K] = false;
            break;
        // KEY_ESC
        case '27':
            keyboardButtonStates[KEY_ESC] = false;
            break;
        // KEY_SPACE
        case ' ':
            //keyboardButtonStates[KEY_SPACE] = false; //setting false after jump is processed.
            break;

        default:
            break;
    }
}
void Engine::hSpecialUp(int key, int x, int y)
{
    switch (key)
    {
        // KEY_F10
        case GLUT_KEY_F10:
            keyboardButtonStates[KEY_F10] = false;
            break;
        // KEY_F11
        case GLUT_KEY_F11:
            keyboardButtonStates[KEY_F11] = false;
            break;
        // KEY_F12
        case GLUT_KEY_F12:
            keyboardButtonStates[KEY_F12] = false;
            break;
        default:
            break;
    }
}

void Engine::hMouse(int button, int state, int x, int y)
{
    bool bState = (state == GLUT_DOWN ? true : false);

    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseStates[0] = bState;
            break;
        case GLUT_MIDDLE_BUTTON:
            mouseStates[1] = bState;
            break;
        case GLUT_RIGHT_BUTTON:
            mouseStates[2] = bState;
            break;
        default:
            break;
    }
}
void Engine::hPassiveMotion(int x, int y)
{
    mousePosition.x = x;
    mousePosition.y = y;
}

void Engine::hTimerCallback(int val)
{
	instance->UpdateLoop();
	glutTimerFunc(10, Engine::hTimerCallback, 0);
}

void Engine::toggleGameMode()
{
    if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE))
    {
        // disable game mode
        glutLeaveGameMode();
    }
    else
    {
        // enable game mode
        glutGameModeString(gameMode);
        if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
        {
            glutEnterGameMode();
		    instance->Setup();
            // TODO: register callbacks again?
            init();
        }
    }

}

