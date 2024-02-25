#include <stdlib.h>
#include <stdio.h>

enum MethodsIds {
    PLAY_VIDEO = 1,
    PLAYLIST_BEGIN_INIT, 
    PLAYLIST_FINISH_INIT,
    IS_VIDEO_PLAYING,
    PLAYLIST_PLAY,
    PLAYLIST_PAUSE,
    PLAYLIST_PLAYING,
    PLAYLIST_HAS_SONGS,
    GOOGLE_PLAYLIST_AVAILABLE,
    GO_TO_URL,
    SHOW_LOADING_SCREEN,
    HIDE_LOADING_SCREEN,
    SHOW_LOADING_BAR,
    SHOW_LOADING_BAR_PROGRESS_HACK,
    GET_DEVICE_LANGUAGE,
    QUIT_APP,
    REMOVE_FRAME_RATE_LOCK,
    SET_KEEP_SCREEN_ON,
    SET_IN_UI,
    GET_VERSION_NAME,
    SHOW_PROMPT,
    CHECK_GATE,
    SHOW_GATE,
    SHOW_GATE_BEFORE_LOAD,
    SIGN_IN,
    SIGN_OUT,
    SET_LOCALE_PRIORITY,
    UPDATE_ROCKSTAR_ID,
    GAME_LOAD
};

extern "C"
{
    #include "FalsoJNI/FalsoJNI_Impl.h"
    
    jobject GetDeviceLanguage(jmethodID id, va_list args) {
        return (jobject)"en";
    }

    void ShowLoadingBar(jmethodID id, va_list args) {
        jfloat progress = va_arg(args, jfloat);
        printf("[JNI] - ShowLoadingBar: %f !\n", progress);
    }

    void ShowLoadingScreen(jmethodID id, va_list args) {
        printf("[JNI] - ShowLoadingScreen !\n");
    }

    void setKeepScreenOn(jmethodID id, va_list args) {
        jboolean arg = va_arg(args, jboolean);
        printf("[JNI] - setKeepScreenOn: %d !\n", arg);
    }
     
    void setInUI(jmethodID id, va_list args) {
        jboolean arg = va_arg(args, jboolean);
        printf("[JNI] - setInUI: %d !\n", arg);
    }

    NameToMethodID nameToMethodId[] = {
        { PLAY_VIDEO, "PlayVideo", METHOD_TYPE_VOID },
        { PLAYLIST_BEGIN_INIT, "PlaylistBeginInit", METHOD_TYPE_VOID },
        { PLAYLIST_FINISH_INIT, "PlaylistFinishInit", METHOD_TYPE_VOID },
        { IS_VIDEO_PLAYING, "isVideoPlaying", METHOD_TYPE_BOOLEAN },
        { PLAYLIST_PLAY, "PlaylistPlay", METHOD_TYPE_VOID },
        { PLAYLIST_PAUSE, "PlaylistPause", METHOD_TYPE_VOID },
        { PLAYLIST_PLAYING, "PlaylistPlaying", METHOD_TYPE_BOOLEAN },
        { PLAYLIST_HAS_SONGS, "PlaylistHasSongs", METHOD_TYPE_BOOLEAN },
        { GOOGLE_PLAYLIST_AVAILABLE, "GooglePlaylistAvailable", METHOD_TYPE_BOOLEAN },
        { GO_TO_URL, "GoToUrl", METHOD_TYPE_VOID },
        { SHOW_LOADING_SCREEN, "ShowLoadingScreen", METHOD_TYPE_VOID },
        { HIDE_LOADING_SCREEN, "HideLoadingScreen", METHOD_TYPE_VOID },
        { SHOW_LOADING_BAR, "ShowLoadingBar", METHOD_TYPE_VOID },
        { SHOW_LOADING_BAR_PROGRESS_HACK, "ShowLoadingBarProgressHack", METHOD_TYPE_VOID },
        { GET_DEVICE_LANGUAGE, "GetDeviceLanguage", METHOD_TYPE_OBJECT },
        { QUIT_APP, "QuitApp", METHOD_TYPE_VOID },
        { REMOVE_FRAME_RATE_LOCK, "RemoveFrameRateLock", METHOD_TYPE_VOID },
        { SET_KEEP_SCREEN_ON, "setKeepScreenOn", METHOD_TYPE_VOID },
        { SET_IN_UI, "setInUI", METHOD_TYPE_VOID },
        { GET_VERSION_NAME, "GetVersionName", METHOD_TYPE_OBJECT },
        { SHOW_PROMPT, "ShowPrompt", METHOD_TYPE_VOID },
        { CHECK_GATE, "CheckGate", METHOD_TYPE_BOOLEAN },
        { SHOW_GATE, "ShowGate", METHOD_TYPE_VOID },
        { SHOW_GATE_BEFORE_LOAD, "ShowGateBeforeLoad", METHOD_TYPE_VOID },
        { SIGN_IN, "SignIn", METHOD_TYPE_VOID },
        { SIGN_OUT, "SignOut", METHOD_TYPE_VOID },
        { SET_LOCALE_PRIORITY, "SetLocalePriority", METHOD_TYPE_VOID },
        { UPDATE_ROCKSTAR_ID, "UpdateRockstarID", METHOD_TYPE_VOID },
        { GAME_LOAD, "GameLoad", METHOD_TYPE_VOID },
    };

    MethodsBoolean methodsBoolean[] = {};
    MethodsByte methodsByte[] = {};
    MethodsChar methodsChar[] = {};
    MethodsDouble methodsDouble[] = {};
    MethodsFloat methodsFloat[] = {};
    MethodsInt methodsInt[] = {};
    MethodsLong methodsLong[] = {};
    MethodsObject methodsObject[] = {
        { GET_DEVICE_LANGUAGE,  GetDeviceLanguage },
    };
    MethodsShort methodsShort[] = {};
    MethodsVoid methodsVoid[] = {
        { SHOW_LOADING_SCREEN, ShowLoadingScreen },
        { SET_KEEP_SCREEN_ON, setKeepScreenOn },
        { SET_IN_UI, setInUI },
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