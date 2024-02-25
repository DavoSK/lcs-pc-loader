#include <stdlib.h>
#include <stdio.h>

extern "C"
{
    #include "FalsoJNI/FalsoJNI_Impl.h"
    
    jobject GetDeviceLanguage(jmethodID id, va_list args) {
        return (jobject)"en";
    }

    void GameInited(jmethodID id, va_list args) {
        printf("[JNI] - Game inited !\n");
    }

    NameToMethodID nameToMethodId[] = {
        { 0, "GetDeviceLanguage", METHOD_TYPE_OBJECT },
        { 0, "GameInited",  METHOD_TYPE_VOID },
    };

    MethodsBoolean methodsBoolean[] = {};
    MethodsByte methodsByte[] = {};
    MethodsChar methodsChar[] = {};
    MethodsDouble methodsDouble[] = {};
    MethodsFloat methodsFloat[] = {};
    MethodsInt methodsInt[] = {};
    MethodsLong methodsLong[] = {};
    MethodsObject methodsObject[] = {
         { 0,  GetDeviceLanguage },
    };
    MethodsShort methodsShort[] = {};
    MethodsVoid methodsVoid[] = {
        { 0, GameInited },
    };

    NameToFieldID nameToFieldId[] = {};

    FieldsBoolean fieldsBoolean[] = {};
    FieldsByte fieldsByte[] = {};
    FieldsChar fieldsChar[] = {};
    FieldsDouble fieldsDouble[] = {};
    FieldsFloat fieldsFloat[] = {};
    FieldsInt fieldsInt[] = {};
    FieldsObject fieldsObject[] = {};
    FieldsLong fieldsLong[] = {};
    FieldsShort fieldsShort[] = {};

    __FALSOJNI_IMPL_CONTAINER_SIZES
}