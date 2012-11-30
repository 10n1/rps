
#include <jni.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "game.h"

static game_t _game;
unsigned int _device_width = 0;
unsigned int _device_height = 0;
AAssetManager* _asset_manager = NULL;

// JNI function declarations
JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_init(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_step(JNIEnv * env, jobject obj, jfloat fElapsedTime);
JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_createAssetManager(JNIEnv* env, jobject obj, jobject assetManager);
JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_handleTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y);

// JNI function definitions

JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
	_device_width = width;
	_device_height = height;
    game_initialize( &_game, width, height );
}

JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_step(JNIEnv * env, jobject obj, jfloat fElapsedTime)
{
    // Update the app
    game_update( &_game );
    
    // Render a frame from the app
    game_render( &_game );
}

JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_createAssetManager(JNIEnv* env, jobject obj, jobject assetManager)
{
    _asset_manager = AAssetManager_fromJava( env, assetManager );
    assert( _asset_manager );
}

JNIEXPORT void JNICALL Java_com_tenone_rps_RPSXLib_handleTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y)
{
	game_handle_touch( &_game, x, y );
}
