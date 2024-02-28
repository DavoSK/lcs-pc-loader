#include <stdio.h>
#include <stdlib.h>

#include "mojoelf.h"
#include "main.h"
#include "game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#define GLAD_GLES2_IMPLEMENTATION
#include "gles2.h"

#include <time.h>
#include <vector>
#include <tuple>
#include <string>
#include <array>
#include <unistd.h>

enum eGameState
{
    GS_START_UP = 0,
    GS_INIT_LOGO_MPEG,
    GS_LOGO_MPEG,
    GS_INIT_INTRO_MPEG,
    GS_INTRO_MPEG,
    GS_INIT_ONCE,
    GS_INIT_FRONTEND,
    GS_FRONTEND,
    GS_INIT_PLAYING_GAME,
    GS_PLAYING_GAME,
};

GLFWwindow* window = NULL;
extern void* base;

typedef void (*GTAJNIlib_markInitialized_t)();
typedef void (*GTAJNIlib_viewOnDrawFrame_t)(float deltaTime);
typedef void (*InitGame_t)();
typedef class CPad* (*GetPad_t)(int pad);

typedef int64_t(*TouchEvent_t)(int, int, int, int);
TouchEvent_t test_touch_event = NULL;
InitGame_t StartGame = NULL;
InitGame_t StartGameBeforeLoad = NULL;
InitGame_t FinishGate = NULL;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool is_key_down = action == GLFW_PRESS;
    if (key == GLFW_KEY_ESCAPE && is_key_down) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

void show_gate() {
    FinishGate();
    StartGame();
}

void show_gate_before_load() {
    FinishGate();
    StartGame();
}

void* game_run(void*) {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    window = glfwCreateWindow(800, 600, "Testing window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLES2(glfwGetProcAddress);
    glfwSwapInterval(1);
    printf("GLES: %s\n", glGetString(GL_VERSION));

    test_touch_event = (TouchEvent_t)MOJOELF_dlsym(game_library, "_Z14AND_TouchEventiiii");
    InitGame_t InitGame = (InitGame_t)MOJOELF_dlsym(game_library, "_Z8InitGamev");
    StartGame = (InitGame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_RockstarJNIlib_StartGame");
    StartGameBeforeLoad = (InitGame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_RockstarJNIlib_StartGameBeforeLoad");
    FinishGate = (InitGame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_RockstarJNIlib_FinishGate");
    

    GTAJNIlib_markInitialized_t markInitialized = (GTAJNIlib_markInitialized_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_markInitialized");
    GTAJNIlib_viewOnDrawFrame_t viewOnDrawFrame = (GTAJNIlib_viewOnDrawFrame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_viewOnDrawFrame");

    using AssetsType = std::vector<std::tuple<std::string,std::string>>;
    AssetsType& g_assetPacks = *(AssetsType*)MOJOELF_dlsym(game_library, "g_assetPacks");
    g_assetPacks.emplace_back(std::make_tuple("data_main", "assets"));
    g_assetPacks.push_back(std::make_tuple("data_music", "assets"));

    char* g_AppSupportPath = (char*)MOJOELF_dlsym(game_library, "g_AppSupportPath");
    strcpy(g_AppSupportPath, "/home/david/lcs/build/here");

    int& Width = *(int*)MOJOELF_dlsym(game_library, "Width");
    int& Height = *(int*)MOJOELF_dlsym(game_library, "Height");
    Width = 800;
    Height= 600;

    eGameState& gameState = *(eGameState*)MOJOELF_dlsym(game_library, "gGameState");
    *(bool*)MOJOELF_dlsym(game_library, "gameCanRender") = true;
    
    InitGame();
    printf("InitGame!\n");

    markInitialized();
    printf("Start game before load !\n");

    *(bool*)(((uint64_t)base + 0x0000000000AF2A80)) = true; //bIsPlaylistInited or something like that

    double last_ticke_time = glfwGetTime();
    
    int& joypadConnected = *(int*)MOJOELF_dlsym(game_library, "JoypadsConnected");
    joypadConnected = 1;

    double* GamepadAxis = (double*)MOJOELF_dlsym(game_library, "GamepadAxis"); 
    uint8_t* joypadButtons = (uint8_t*)MOJOELF_dlsym(game_library, "joypadButtons");
    
    while (!glfwWindowShouldClose(window)) {
        double delta_time = glfwGetTime() - last_ticke_time;
        last_ticke_time = glfwGetTime();

        //NOTE: fake input axis
        GamepadAxis[0] = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ? -1.0f : GamepadAxis[0];
        GamepadAxis[0] = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ? 1.0f : GamepadAxis[0];
        if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS) {
            GamepadAxis[0] = 0.0f;
        }

        GamepadAxis[1] = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ? -1.0f : GamepadAxis[1];
        GamepadAxis[1] = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ? 1.0f : GamepadAxis[1];
         if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) {
            GamepadAxis[1] = 0.0f;
        }

        //NOTE: fake inpput buttons
        joypadButtons[GLFW_GAMEPAD_BUTTON_A] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        joypadButtons[GLFW_GAMEPAD_BUTTON_B] = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
        joypadButtons[GLFW_GAMEPAD_BUTTON_Y] = glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS;
        joypadButtons[GLFW_GAMEPAD_BUTTON_X] = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;
        //int _width, _height;
        //glfwGetFramebufferSize(window, &_width, &_height);


        // printf("joi: %d, %d, %d\n",  glfwJoystickPresent(GLFW_JOYSTICK_1), glfwJoystickPresent(GLFW_JOYSTICK_2),  glfwJoystickPresent(GLFW_JOYSTICK_3));
        // GLFWgamepadstate state;
        // if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        // {
        //     printf("something !\n");
        //     if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
        //     {
        //         printf("inpuat A !!!\n");
        //     }
        
        //     //input_speed(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
        // }
        
        if(gameState == GS_LOGO_MPEG) {
            gameState = GS_INIT_ONCE;
        }

        viewOnDrawFrame(delta_time);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}