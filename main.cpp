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

void *my_loader(const char *soname, const char *rpath, const char *runpath) {
    void *handle = dlopen(soname, RTLD_LAZY);
    if (handle) return handle;

    printf("[!] unable to open: %s\n", soname);
    return NULL;
}

void android_set_abort_message(const char* msg) {
}

void my_unloader(void* handle)  {
    if(handle) {
        dlclose(handle);
    }
}

int __android_log_assert(const char *cond, const char *tag, const char *fmt, ...) {
    return 0;
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...) {
    va_list list;
    char string[512];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);

    printf("[LOG] %s: %s\n", tag, string);
    return 0;
}

extern "C" int __android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap) {
    printf("[%s] [%s]\n", tag, fmt);
    return 0;
}

extern "C" int __android_log_write(int prio, const char *tag, const char *text) {
    printf("[%s] - %s\n", tag, text);
    return 0;
}

int ret0(void) {
    return 0;
}

static char *__ctype_ = (char *)&__ctype_;
static FILE __sF_fake[0x100][3];

extern "C" void* eglGetProcAddress_faker(const char* procname) {
    GLFWglproc result = glfwGetProcAddress(procname);
    if(!result) {
        printf("[!] glGenVertexArraysOES: %s\n", procname);
    }

    return (void*)result;
}

EGLBoolean eglMakeCurrent_faker(EGLDisplay display,
 	EGLSurface draw,
 	EGLSurface read,
 	EGLContext context) {
    return true;
}

EGLBoolean eglSwapBuffers_faker(EGLDisplay display,
 	EGLSurface surface) {
    return true;
}

FILE* fopen_hook(const char* path, const char* modes) {
    FILE* file = fopen(path, modes);
    if(!file) {
        printf("[!] fopen(%s, %s);\n", path, modes);
    }
    return file;
}

int* __errno_hook() {
    return &errno;
}

uint64_t glGenVertexArrays_c = 0x0;
uint64_t glBindVertexArray_c = 0x0;
uint64_t glDeleteVertexArrays_c = 0x0;

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
    {"AAssetManager_open", (uintptr_t)&ret0},

    {"eglGetProcAddress", (uintptr_t)&eglGetProcAddress_faker},
    {"eglMakeCurrent", (uintptr_t)&eglMakeCurrent_faker}, 
    {"eglSwapBuffers", (uintptr_t)&eglSwapBuffers_faker},

    //{"fopen", (uintptr_t)&fopen_hook},
    {"glGenVertexArrays", (uintptr_t)&glGenVertexArrays_c},
    {"glBindVertexArray", (uintptr_t)&glBindVertexArray_c},
    {"glDeleteVertexArrays", (uintptr_t)&glDeleteVertexArrays_c},
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

subhook::Hook foo_hook;
subhook::Hook foo_hook2;
subhook::Hook foo_hook3;

typedef void (*cWorldStreamEx__SetModelMatrix_t)(void* _this, void* mat);
typedef bool (*SetModelMatrix_t)(void* mat);
SetModelMatrix_t Culling_SetModelMatrix = NULL;
cWorldStreamEx__SetModelMatrix_t cWorldStreamEx__SetModelMatrix = NULL;

void cWorldStreamEx__SetModelMatrix_hook(void* _this, void* mat) {
    
    //printf("cWorldStreamEx__SetModelMatrix\n");
    //subhook::ScopedHookRemove remove(&foo_hook2);
    //printf("yeS: %p, %p\n", _this, mat);

//    uint64_t smth =  *(uint64_t*)((uint64_t)_this +  0x5C);
//    *(uint64_t*)((uint64_t)_this +  0x5C) = NULL;
   
//    cWorldStreamEx__SetModelMatrix(_this, mat);

//    *(uint64_t*)((uint64_t)_this +  0x5C) = smth;

    //if(mat) {

        Culling_SetModelMatrix(mat);
    //}
}

void* hal__Main__update_hook(void* _this, float stuff) {
    return 0;
}

typedef void (*Render2dStuff_t)();
Render2dStuff_t Render2dStuff = NULL;

Render2dStuff_t DrawFPS = NULL;

void Render2dStuff_hook(void)
{
    subhook::ScopedHookRemove remove(&foo_hook3);
    Render2dStuff();
    DrawFPS();
}

int main(void) {
    self_library = dlopen(NULL, RTLD_LAZY);
    //mpg123_init();
    
    printf("LCS Windows loader kek !\n");
    MOJOELF_Callbacks callbacks = {
        .loader     = my_loader,
        .resolver   = my_resolver,
        .unloader   = my_unloader,    
    };

    game_library = MOJOELF_dlopen_file("../libGame.so", &callbacks);
    if (!game_library) {
        printf("[!] unable to load lib: %s\n", MOJOELF_dlerror());
        return 1;
    }

    printf("[*] lib loaded: %p\n", game_library);

    void* hal__Main__update = MOJOELF_dlsym(game_library, "_ZN3hal4Main6updateEf");
    foo_hook.Install(hal__Main__update, (void *)&hal__Main__update_hook, subhook::HookFlags::HookFlag64BitOffset);

    Culling_SetModelMatrix = (SetModelMatrix_t)MOJOELF_dlsym(game_library, "_ZN7Display9C_Culling14SetModelMatrixERKN5Maths6MatrixE");

    cWorldStreamEx__SetModelMatrix = (cWorldStreamEx__SetModelMatrix_t)MOJOELF_dlsym(game_library, "_ZN14cWorldStreamEx14SetModelMatrixERK7CMatrix");
    foo_hook2.Install((void*)cWorldStreamEx__SetModelMatrix, (void *)&cWorldStreamEx__SetModelMatrix_hook, subhook::HookFlags::HookFlag64BitOffset);

    DrawFPS = (Render2dStuff_t)MOJOELF_dlsym(game_library, "_Z7DrawFPSv");
    Render2dStuff = (Render2dStuff_t)MOJOELF_dlsym(game_library, "_Z13Render2dStuffv");
    foo_hook3.Install((void*)Render2dStuff, (void *)&Render2dStuff_hook, subhook::HookFlags::HookFlag64BitOffset);
    jni_load(); 
    game_run(NULL);

    MOJOELF_dlclose(game_library);
    dlclose(self_library);
    return 0;
}