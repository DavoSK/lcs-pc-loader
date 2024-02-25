#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <mpg123.h>

#include "mojoelf.h"
#include "game.h"

#include "android.cpp"
#include "jni_patch.h"
#include "egl.h"
#include "gles2.h"

#include <pthread.h>
#include <GLFW/glfw3.h>
#include "subhook/subhook.h"

typedef struct
{
    const char *symbol;
    uintptr_t func;
} so_default_dynlib;

typedef struct 
{
    const char *symbol;
    const char *symbol_src;
} so_ndk_map;

void* self_library = NULL;
void* game_library = NULL;

void android_set_abort_message(const char* msg) {
}

void *my_loader(const char *soname, const char *rpath, const char *runpath)
{
    void *handle = dlopen(soname, RTLD_LAZY);
    if (handle) return handle;

    printf("[!] unable to open: %s\n", soname);
    return NULL;
}

void my_unloader(void* handle) 
{
    if(handle) 
    {
        printf("unload: %p\n", handle);
        dlclose(handle);
    }
}

int __android_log_assert(const char *cond, const char *tag, const char *fmt, ...)
{
    return 0;
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...)
{
#ifdef DEBUG
    va_list list;
    char string[512];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);

    debugPrintf("[LOG] %s: %s\n", tag, string);
#endif
    return 0;
}

int __android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
{
    return 0;
}

int __android_log_write(int prio, const char *tag, const char *text)
{
    return 0;
}

int ret0(void)
{
    return 0;
}

static char *__ctype_ = (char *)&__ctype_;
static FILE __sF_fake[0x100][3];

void* eglGetDisplay_faker(EGLNativeDisplayType display_id) {
    printf("eglGetDisplay_faker !\n");
    return 0;
}


EGLBoolean eglChooseConfig_faker(EGLDisplay dpy, const EGLint * attrib_list, EGLConfig * configs, EGLint config_size, EGLint * num_config) {
    printf("eglChooseConfig_faker !\n");
    configs[0] = (EGLConfig)0x22;
    configs[1] = (EGLConfig)0x22; //fake ptr;
    *num_config = 1;
    return 0;
}

EGLBoolean eglGetConfigAttrib_faker(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint * value) {
    printf("eglGetConfigAttrib_faker !\n");
    return 0;
}

const GLubyte* glGetString_faker(unsigned int name) {
    return glGetString(name);
}

void* eglGetProcAddress_faker(const char* procname) {
    GLFWglproc result = glfwGetProcAddress(procname);
    if(!result) {
        printf("[!] eglGetProcAddress_faker: %s\n", procname);
    }
    return (void*)result;
}

EGLBoolean eglMakeCurrent_faker(EGLDisplay display,
 	EGLSurface draw,
 	EGLSurface read,
 	EGLContext context) {
    
    printf("eglMakeCurrent_faker !\n");
    return true;
}

EGLBoolean eglSwapBuffers_faker(EGLDisplay display,
 	EGLSurface surface) {
    printf("eglSwapBuffers !!!!!!!!!!!!!!!!!!!!!\n"); 
    return true;
}

FILE* fopen_hook(const char* path, const char* modes) {
    FILE* file = fopen(path, modes);
    // if(!file) {
    //     printf("fopen(%s, %s);\n", path, modes);
    //     return NULL;
    //     //return fopen("./dummy.txt", modes);      
    // }
    return file;
}

// GLuint glCreateShader_fake(GLenum shaderType) {
//     printf("glCreateShader_fake: %d\n", shaderType);
//     return glCreateShader(shaderType);          
// }

// GLuint glCreateProgram_fake(void) {
//     printf("[g] glCreateProgram_fake\n");
//     return glCreateProgram();
// }

void glClearColor_fake(	GLfloat red,
 	GLfloat green,
 	GLfloat blue,
 	GLfloat alpha) {
    printf("glClearColor !!!\n");
}

// void glShaderSource_fake(GLuint shader,
//  	GLsizei count,
//  	const GLchar **string,
//  	const GLint *length) {
    
//     printf("----------------------\n");
//     printf("glShaderSource:\n");
//     for(size_t i = 0; i < count; ++i) {
//         printf("[src] (%d):\n%s\n", i, string[i]);
//     }
//     printf("----------------------\n");
//     return glShaderSource(shader, count, string, length);
// }

void* AAssetManager_open(void* amgr, const char* filename, int mode) {   
    printf("AAssetManager_open!!!\n");
    return 0;
}

// void* alcOpenDevice_faker(void* ctx) {
//     printf("alcOpenDevice_faker !!!\n");
//     return NULL;
// }

int* __errno_hook() {
    return &errno;
}

 pthread_t  pthread_self_hook() {
    printf("pthread_self_hook !\n");
    return pthread_self();
 }

static so_default_dynlib default_dynlib[] = {
    {"__android_log_assert", (uintptr_t)&__android_log_assert},
    {"__android_log_print", (uintptr_t)&__android_log_print},
    {"__android_log_vprint", (uintptr_t)&__android_log_vprint},
    {"__android_log_write", (uintptr_t)&__android_log_write},
    {"android_set_abort_message", (uintptr_t)&android_set_abort_message},

    { "__sF", (uintptr_t)&__sF_fake },
    { "__errno", (uintptr_t)&__errno_hook },
    { "_ctype_", (uintptr_t)&__ctype_ },

    {"ANativeWindow_release", (uintptr_t)&ret0},
    {"ANativeWindow_fromSurface", (uintptr_t)&ret0},
    {"ANativeWindow_getWidth", (uintptr_t)&ret0},
    {"ANativeWindow_getHeight", (uintptr_t)&ret0},
    {"ANativeWindow_setBuffersGeometry", (uintptr_t)&ret0},

    {"AAsset_close", (uintptr_t)&ret0},
    {"AAsset_getLength", (uintptr_t)&ret0},
    {"AAsset_getRemainingLength", (uintptr_t)&ret0},
    {"AAsset_read", (uintptr_t)&ret0},
    {"AAsset_seek", (uintptr_t)&ret0},
    {"AAssetManager_fromJava", (uintptr_t)&ret0},
    {"AAssetManager_open", (uintptr_t)&AAssetManager_open},

    {"eglGetDisplay", (uintptr_t)&eglGetDisplay_faker},
    {"eglChooseConfig", (uintptr_t)&eglChooseConfig_faker},
    {"eglGetConfigAttrib", (uintptr_t)&eglGetConfigAttrib_faker},
    {"eglGetProcAddress", (uintptr_t)&eglGetProcAddress_faker},
    {"eglMakeCurrent", (uintptr_t)&eglMakeCurrent_faker}, 
    {"eglSwapBuffers", (uintptr_t)&eglSwapBuffers_faker},

    //{"alcOpenDevice", (uintptr_t)&alcOpenDevice_faker},

    {"glClearColor", (uintptr_t)&glClearColor_fake},
    // {"glCreateProgram", (uintptr_t)&glCreateProgram_fake},
    // {"glCreateShader", (uintptr_t)&glCreateShader_fake},
    // {"glShaderSource", (uintptr_t)&glShaderSource_fake},
    {"fopen", (uintptr_t)&fopen_hook},
    //{"pthread_self", (uintptr_t)&pthread_self_hook},

};

static so_ndk_map ndk_map[] = {
    { "_ZNKSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7compareEPKc", "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7compareEPKc" },
    { "_ZNKSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7compareEmmPKcm", "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7compareEmmPKcm" },
    { "_ZNKSt6__ndk120__vector_base_commonILb1EE20__throw_length_errorEv", "_ZNKSt3__120__vector_base_commonILb1EE20__throw_length_errorEv" },
    { "_ZNKSt6__ndk120__vector_base_commonILb1EE20__throw_out_of_rangeEv", "_ZNKSt3__120__vector_base_commonILb1EE20__throw_out_of_rangeEv" },
    { "_ZNKSt6__ndk121__basic_string_commonILb1EE20__throw_length_errorEv", "_ZNKSt3__121__basic_string_commonILb1EE20__throw_length_errorEv" },
    { "_ZNKSt6__ndk16locale9use_facetERNS0_2idE", "_ZNKSt3__16locale9use_facetERNS0_2idE" },
    { "_ZNKSt6__ndk18ios_base6getlocEv", "_ZNKSt3__18ios_base6getlocEv" },
    { "_ZNSt13runtime_errorC1ERKNSt6__ndk112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE", "_ZNSt13runtime_errorC1ERKNSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE5eraseEmm", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE5eraseEmm" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKcm", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKcm" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6insertEmPKc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6insertEmPKc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6insertEmPKcm", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6insertEmPKcm" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6resizeEmc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6resizeEmc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7replaceEmmPKc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7replaceEmmPKc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9push_backEc", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9push_backEc" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_mmRKS4_", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_mmRKS4_" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED2Ev", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev" },
    { "_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEaSERKS5_", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEaSERKS5_" },
    { "_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEE6sentryC1ERS3_b", "_ZNSt3__113basic_istreamIcNS_11char_traitsIcEEE6sentryC1ERS3_b" },
    { "_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED0Ev", "_ZNSt3__113basic_istreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED1Ev", "_ZNSt3__113basic_istreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEErsERi", "_ZNSt3__113basic_istreamIcNS_11char_traitsIcEEErsERi" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED0Ev", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED1Ev", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED2Ev", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEf", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf" },
    { "_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEi", "_ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi" },
    { "_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev", "_ZNSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev", "_ZNSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED2Ev", "_ZNSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE4syncEv", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE4syncEv" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE5imbueERKNS_6localeE", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE5imbueERKNS_6localeE" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE5uflowEv", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE5uflowEv" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6setbufEPcl", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE6setbufEPcl" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6xsgetnEPcl", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE6xsgetnEPcl" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6xsputnEPKcl", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE6xsputnEPKcl" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE9showmanycEv", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEE9showmanycEv" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEEC2Ev", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEEC1Ev" },
    { "_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEED2Ev", "_ZNSt3__115basic_streambufIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk15ctypeIcE2idE", "_ZNSt3__15ctypeIcE2idE" },
    { "_ZNSt6__ndk16localeD1Ev", "_ZNSt3__16localeD1Ev" },
    { "_ZNSt6__ndk18ios_base33__set_badbit_and_consider_rethrowEv", "_ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv" },
    { "_ZNSt6__ndk18ios_base4initEPv", "_ZNSt3__18ios_base4initEPv" },
    { "_ZNSt6__ndk18ios_base5clearEj", "_ZNSt3__18ios_base5clearEj" },
    { "_ZNSt6__ndk19basic_iosIcNS_11char_traitsIcEEED2Ev", "_ZNSt3__19basic_iosIcNS_11char_traitsIcEEED0Ev" },
    { "_ZNSt6__ndk19to_stringEi", "_ZNSt3__19to_stringEi" },
    { "_ZNSt6__ndk19to_stringEj", "_ZNSt3__19to_stringEj" },
    { "_ZNSt6__ndk1plIcNS_11char_traitsIcEENS_9allocatorIcEEEENS_12basic_stringIT_T0_T1_EEPKS6_RKS9_", "_ZNSt3__1plIcNS_11char_traitsIcEENS_9allocatorIcEEEENS_12basic_stringIT_T0_T1_EEPKS6_RKS9_" },
    { "_ZTINSt6__ndk113basic_istreamIcNS_11char_traitsIcEEEE", "_ZTINSt3__113basic_istreamIcNS_11char_traitsIcEEEE" },
    { "_ZTINSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEEE", "_ZTINSt3__113basic_ostreamIcNS_11char_traitsIcEEEE" },
    { "_ZTINSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEEE", "_ZTINSt3__114basic_iostreamIcNS_11char_traitsIcEEEE" },
    { "_ZTINSt6__ndk115basic_streambufIcNS_11char_traitsIcEEEE", "_ZTINSt3__115basic_streambufIcNS_11char_traitsIcEEEE" },
    { "_ZThn16_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev", "_ZThn16_NSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZThn16_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev", "_ZThn16_NSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED0Ev", "_ZTv0_n24_NSt3__113basic_istreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED1Ev", "_ZTv0_n24_NSt3__113basic_istreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED0Ev", "_ZTv0_n24_NSt3__113basic_ostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED1Ev", "_ZTv0_n24_NSt3__113basic_ostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev", "_ZTv0_n24_NSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
    { "_ZTv0_n24_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev", "_ZTv0_n24_NSt3__114basic_iostreamIcNS_11char_traitsIcEEED0Ev" },
};

void *my_resolver(void *handle, const char *sym) {
    for (size_t i = 0; i < sizeof(default_dynlib) / sizeof(so_default_dynlib); ++i) {
        so_default_dynlib reloc = default_dynlib[i];
        if (strcmp(reloc.symbol, sym) == 0) {
            return (void *)reloc.func;
        }
    }
    
    //NOTE: ndk mapping to standard linux std
    for (size_t i = 0; i < sizeof(ndk_map) / sizeof(so_ndk_map); ++i) {
        so_ndk_map reloc = ndk_map[i];
        if (strcmp(reloc.symbol, sym) == 0) {   
            void *symbol = dlsym(self_library, reloc.symbol_src);
            if(symbol) return (void *)symbol;
        }
    }

    void *symbol = dlsym(!handle ? self_library : handle, sym);
    if (symbol)
        return symbol;

    return NULL; /* can't help you. */
}

void before_init(void *handle)
{
    printf("\n\n");
}

void my_library_close(void) __attribute__ ((destructor));
void my_library_close(void)
{
    dlclose(self_library);
}

subhook::Hook foo_hook;
// subhook::Hook foo_hook2;
// subhook::Hook foo_hook3;
// subhook::Hook foo_hook4;

bool initVertexArrays_hook() {
    uint64_t* glGenVertexArraysAddr = (uint64_t*)MOJOELF_dlsym(game_library, "glGenVertexArrays");
    *glGenVertexArraysAddr = (uint64_t)glfwGetProcAddress("glGenVertexArraysOES");
    //printf("[x] - glGenVertexArraysAddr: %p -> %p\n", glGenVertexArraysAddr, (void*)*glGenVertexArraysAddr);

    uint64_t* glBindVertexArrayAddr = (uint64_t*)MOJOELF_dlsym(game_library, "glBindVertexArray");
    *glBindVertexArrayAddr = (uint64_t)glfwGetProcAddress("glBindVertexArray");
    //printf("[x] - glBindVertexArrayAddr: %p -> %p\n", glBindVertexArrayAddr, (void*)*glBindVertexArrayAddr);

    uint64_t* glDeleteVertexArraysAddr = (uint64_t*)MOJOELF_dlsym(game_library, "glDeleteVertexArrays");
    *glDeleteVertexArraysAddr = (uint64_t)glfwGetProcAddress("glDeleteVertexArraysOES");
    //printf("[x] - glDeleteVertexArraysOES: %p -> %p\n", glDeleteVertexArraysAddr, (void*)*glDeleteVertexArraysAddr);

    bool& vertexArraysSupported = *(bool*)MOJOELF_dlsym(game_library, "vertexArraysSupported");
    vertexArraysSupported = *glGenVertexArraysAddr != NULL && *glBindVertexArrayAddr != NULL && *glDeleteVertexArraysAddr != NULL;
    //printf("vertexArraysSupported: %d\n", vertexArraysSupported);
    return vertexArraysSupported;
}

// typedef void* (*WadArchive__OpenFile_t)(void* wad, const char *a2);
// WadArchive__OpenFile_t open_file = NULL;

// void* WadArchive__OpenFile(void* wad, const char *a2) {
//     subhook::ScopedHookRemove remove(&foo_hook2);
//     void* file = open_file(wad, a2);
//     if(!file) {
//         printf("WadArchive__OpenFile: %p, %s -> %p\n", wad, a2, file);
//     }
//     return file;
// }

// typedef void* (*lglTextureLoader__loadTextureFromPath_t)(void *loader, void *textureTask, const char *a3);
// lglTextureLoader__loadTextureFromPath_t lglTextureLoader__loadTextureFromPath = NULL;

// bool lglTextureLoader__loadTextureFromPath_Hook(void *loader, void *textureTask, const char *texturePath)
// {
//     subhook::ScopedHookRemove remove(&foo_hook3);
//     bool wasloaded = lglTextureLoader__loadTextureFromPath(loader, textureTask, texturePath);
//     if(!wasloaded) {
//         printf("lglTextureLoader__loadTextureFromPath %s\n", texturePath);
//     }
//     return wasloaded;
// }

// typedef void* (*Platform__FileOpenOSFilePath_t)(char *a1, int a2, int a3);
// Platform__FileOpenOSFilePath_t Platform__FileOpenOSFilePath = NULL;

// void* Platform__FileOpenOSFilePath_Hook(char *a1, int a2, int a3)
// {
//     subhook::ScopedHookRemove remove(&foo_hook4);
//     printf("Platform__FileOpenOSFilePath (%s, %d, %d)\n", a1, a2, a3);
//     void* ptr = Platform__FileOpenOSFilePath(a1, a2, a3);
//     return ptr;
// }

int main(void) {
    self_library = dlopen(NULL, RTLD_LAZY);
    mpg123_init();
    game_init(); //Lets load opengl before we start loading library
    printf("LCS Windows loader kek !\n");
    MOJOELF_Callbacks callbacks = {
        .loader     = my_loader,
        .resolver   = my_resolver,
        .unloader   = my_unloader,
        .beforeinit = before_init    
    };

    game_library = MOJOELF_dlopen_file("/home/david/lcs/libGame.so", &callbacks);
    if (!game_library) {
        printf("[!] unable to load lib: %s\n", MOJOELF_dlerror());
        return 1;
    }

    printf("[*] lib loaded: %p\n", game_library);
    //MOJOELF_dlclose(game_library);

    void* initVertexArrays = MOJOELF_dlsym(game_library, "_Z16initVertexArraysv");
    foo_hook.Install((void *)initVertexArrays, (void *)&initVertexArrays_hook, subhook::HookFlags::HookFlag64BitOffset);

    // open_file = (WadArchive__OpenFile_t)MOJOELF_dlsym(game_library, "_ZN10WadArchive8OpenFileEPKc8FileMode");
    // foo_hook2.Install((void *)open_file, (void *)&WadArchive__OpenFile, subhook::HookFlags::HookFlag64BitOffset);

    // lglTextureLoader__loadTextureFromPath = (lglTextureLoader__loadTextureFromPath_t)MOJOELF_dlsym(game_library, "_ZN16lglTextureLoader19loadTextureFromPathEP14lglTextureTaskPKc");
    // foo_hook3.Install((void *)lglTextureLoader__loadTextureFromPath, (void *)&lglTextureLoader__loadTextureFromPath_Hook, subhook::HookFlags::HookFlag64BitOffset);

    // Platform__FileOpenOSFilePath = (Platform__FileOpenOSFilePath_t)MOJOELF_dlsym(game_library, "_ZN8Platform18FileOpenOSFilePathEPKc8FileModei");
    // foo_hook4.Install((void *)Platform__FileOpenOSFilePath, (void *)&Platform__FileOpenOSFilePath_Hook, subhook::HookFlags::HookFlag64BitOffset);

    jni_load();
    game_run();

    printf("jni loaded ? !\n");
    return 0;
}