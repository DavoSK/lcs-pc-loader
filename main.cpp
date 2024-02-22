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
void* cxx_library = NULL;
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

static so_default_dynlib default_dynlib[] = {
    {"__android_log_assert", (uintptr_t)&__android_log_assert},
    {"__android_log_print", (uintptr_t)&__android_log_print},
    {"__android_log_vprint", (uintptr_t)&__android_log_vprint},
    {"__android_log_write", (uintptr_t)&__android_log_write},
    {"android_set_abort_message", (uintptr_t)&android_set_abort_message},

    { "__sF", (uintptr_t)&__sF_fake },
    { "__errno", (uintptr_t)&errno },
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


    {"eglGetDisplay", (uintptr_t)&eglGetDisplay_faker},
    {"eglChooseConfig", (uintptr_t)&eglChooseConfig_faker},
    {"eglGetConfigAttrib", (uintptr_t)&eglGetConfigAttrib_faker},
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

void *my_resolver(void *handle, const char *sym)
{
    for (size_t i = 0; i < sizeof(default_dynlib) / sizeof(so_default_dynlib); ++i)
    {
        so_default_dynlib reloc = default_dynlib[i];
        if (strcmp(reloc.symbol, sym) == 0)
        {
            return (void *)reloc.func;
        }
    }
    
    //NOTE: ndk mapping to standard linux std
    for (size_t i = 0; i < sizeof(ndk_map) / sizeof(so_ndk_map); ++i)
    {
        so_ndk_map reloc = ndk_map[i];
        if (strcmp(reloc.symbol, sym) == 0)
        {   
            void *symbol = dlsym(cxx_library, reloc.symbol_src);
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
    dlclose(cxx_library);
    dlclose(self_library);
}

int main(void)
{
    cxx_library = dlopen("/home/david/lcs/libc++.so", RTLD_LAZY);
    self_library = dlopen(NULL, RTLD_LAZY);

    mpg123_init();
    printf("LCS Windows loader kek !\n");
    MOJOELF_Callbacks callbacks = {
        .loader = my_loader,
        .resolver = my_resolver,
        .unloader = my_unloader,
        .beforeinit = before_init    
    };

    game_library = MOJOELF_dlopen_file("/home/david/lcs/libGame.so", &callbacks);
    if (!game_library)
    {
        printf("[!] unable to load lib: %s\n", MOJOELF_dlerror());
        return 1;
    }

    printf("[*] lib loaded: %p\n", game_library);
    //MOJOELF_dlclose(game_library);

    void* InitGame = MOJOELF_dlsym(game_library, "_Z8InitGamev");
    printf("InitGame: %p\n", InitGame);


    jni_load();
    game_run();

    printf("jni loaded ? !\n");
    return 0;
}