#include <stdio.h>
#include <stdlib.h>

#include "mojoelf.h"
#include "main.h"
#include "game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#define GLAD_GLES2_IMPLEMENTATION
#include "gles2.h"

static GLFWwindow* window = NULL;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


typedef int64_t(*InitGame_t)();
typedef void (*GTAJNIlib_markInitialized_t)();
typedef void (*GTAJNIlib_viewOnDrawFrame_t)();
typedef uint64_t (*GTAJNIlib_viewOnSurfaceChanged_t)(uint64_t, uint64_t, uint64_t);

void game_run()
{
    InitGame_t InitGame = (InitGame_t)MOJOELF_dlsym(game_library, "_Z8InitGamev");
    printf("InitGame: %p\n", InitGame);

    GTAJNIlib_markInitialized_t markInitialized = (GTAJNIlib_markInitialized_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_markInitialized");
    printf("markInitialized: %p\n", markInitialized);

    GTAJNIlib_viewOnDrawFrame_t viewOnDrawFrame = (GTAJNIlib_viewOnDrawFrame_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_markInitialized");
    printf("viewOnDrawFrame: %p\n", viewOnDrawFrame);

    GTAJNIlib_viewOnSurfaceChanged_t surfChanged = (GTAJNIlib_viewOnSurfaceChanged_t)MOJOELF_dlsym(game_library, "Java_com_rockstargames_gtalcs_GTAJNIlib_viewOnSurfaceChanged");
    glfwSetErrorCallback(error_callback);
    
    bool* gameCanRender = (bool*)MOJOELF_dlsym(game_library, "gameCanRender");
    printf("gameCanRender: %p\n", gameCanRender);
    

    int& Width = *(int*)MOJOELF_dlsym(game_library, "Width");
    int& Height = *(int*)MOJOELF_dlsym(game_library, "Height");

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(800, 600, "Testing window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLES2(glfwGetProcAddress);
    glfwSwapInterval(1);
    
    Width = 800;
    Height= 600;
    gameCanRender[0] = true;
    //surfChanged(0, 0, 0);
    InitGame();
    
    while (!glfwWindowShouldClose(window))
    {
        int _width, _height;
        glfwGetFramebufferSize(window, &_width, &_height);
        if(_width != Width || _height != Height) 
        {
            Width = _width;
            Height = _height;
        }


        glViewport(0, 0, Width, Height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

        viewOnDrawFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
        //_sleep(1);
    }
 
    glfwDestroyWindow(window);
    glfwTerminate();   
}