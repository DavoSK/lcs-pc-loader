/* jni_patch.c -- Fake Java Native Interface
 *
 * Copyright (C) 2021 Andy Nguyen
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

extern "C"{
    #include "FalsoJNI/FalsoJNI.h"
}

#include "main.h"
#include "mojoelf.h"
#include "config.h"

typedef __int64_t (*JNI_OnLoad_t)(JavaVM* vm, void* reserved);

void jni_load(void)
{
    jni_init();
    JNI_OnLoad_t JNI_OnLoad = (JNI_OnLoad_t)MOJOELF_dlsym(game_library, "JNI_OnLoad");
    JNI_OnLoad(&jvm, NULL);
}