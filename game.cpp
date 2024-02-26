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
#include <unistd.h>

GLFWwindow* window = NULL;
extern void* base;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

typedef int64_t(*InitGame_t)();
typedef void (*GTAJNIlib_markInitialized_t)();
typedef void (*GTAJNIlib_viewOnDrawFrame_t)(float deltaTime);

typedef int64_t(*Test_t)(int w, int h);
typedef int64_t(*Test_t2)();
typedef float(*Test_t3)(float w, float h);

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

void* game_run(void*) {

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(800, 600, "Testing window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLES2(glfwGetProcAddress);
    glfwSwapInterval(1);

    InitGame_t InitGame = (InitGame_t)MOJOELF_dlsym(game_library, "_Z8InitGamev");
    InitGame_t StartGameBeforeLoad = (InitGame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_RockstarJNIlib_StartGameBeforeLoad");

    GTAJNIlib_markInitialized_t markInitialized = (GTAJNIlib_markInitialized_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_markInitialized");
    GTAJNIlib_viewOnDrawFrame_t viewOnDrawFrame = (GTAJNIlib_viewOnDrawFrame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_viewOnDrawFrame");

    using AssetsType = std::vector<std::tuple<std::string,std::string>>;
    AssetsType& g_assetPacks = *(AssetsType*)MOJOELF_dlsym(game_library, "g_assetPacks");
    g_assetPacks.emplace_back(std::make_tuple("data_main", "assets"));

    char* g_AppSupportPath = (char*)MOJOELF_dlsym(game_library, "g_AppSupportPath");
    strcpy(g_AppSupportPath, "/home/david/lcs/build/here");

    // auto& g_BundleSearchPaths = *(std::vector<std::string>*)(MOJOELF_dlsym(game_library, "g_BundleSearchPaths"));
    // //g_BundleSearchPaths.
    // printf("size: %d\n", g_BundleSearchPaths.size());
    // g_BundleSearchPaths.push_back("");
    // g_BundleSearchPaths.push_back("");
    // g_BundleSearchPaths.push_back("");

    int& Width = *(int*)MOJOELF_dlsym(game_library, "Width");
    int& Height = *(int*)MOJOELF_dlsym(game_library, "Height");
    eGameState& gameState = *(eGameState*)MOJOELF_dlsym(game_library, "gGameState");//*(uint64_t*)(((uint64_t)base + 0x0000000000AF2A08));

    Width = 800;
    Height= 600;
    
    *(bool*)MOJOELF_dlsym(game_library, "gameCanRender") = true;
    
    InitGame();
    printf("InitGame!\n");    

    markInitialized();
    printf("Start game before load !\n");

    *(bool*)(((uint64_t)base + 0x0000000000AF2A80)) = true;

    double last_ticke_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        //int _width, _height;
        //glfwGetFramebufferSize(window, &_width, &_height);

        double delta_time = glfwGetTime() - last_ticke_time;
        last_ticke_time = glfwGetTime();

        // if(_width != Width || _height != Height) {
        //     Width = _width;
        //     Height = _height;
        // }

        if(gameState == GS_LOGO_MPEG) {
            gameState = GS_INIT_ONCE;
        }

        if(gameState == GS_FRONTEND) {
            StartGameBeforeLoad();
        }

        viewOnDrawFrame(delta_time);
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}