#include "system.h"
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

extern unsigned int _device_width;
extern unsigned int _device_height;
extern AAssetManager* _asset_manager;

/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static void _split_filename(const char* filename, char file[128], char ext[16])
{
    char* ext_ptr = NULL;
    strncpy(file, filename, sizeof(char)*128);
    ext_ptr = file + strlen(file);
    while(*ext_ptr != '.')
        --ext_ptr;
    *ext_ptr = '\0';
    strncpy(ext, ++ext_ptr, 16);
}

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/

#define  LOG_TAG    "RPSX"
#define  LOGW(...)  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
void CNSLog(const char* format, ...)
{
    va_list args;
    char message[1024] = {0};
    va_start(args, format);
    snprintf(message, sizeof(message), format, args);
    
    LOGI( message );

    va_end(args);
}

void CNSLogWrite( const char* message )
{
    LOGW( message );
}

void* get_asset_manager(void)
{
    return _asset_manager;
}

int system_load_file_to_memory( const char* filename, void** buffer, int* buffer_size )
{
    CNSLogWrite( filename );
 
    char str[256];
    strncpy( str, filename, 256 );
    char* stripped = strstr( str, "/" );

    CNSLogWrite( stripped+1 );

    AAsset* asset_file = AAssetManager_open( get_asset_manager(), stripped+1, AASSET_MODE_UNKNOWN );
    if( asset_file == NULL )
        return 1;

    off_t data_length = AAsset_getLength( asset_file );

    char* data_bytes = ( char* )malloc( data_length );
    AAsset_read( asset_file, data_bytes, data_length );
    AAsset_close( asset_file );

    *buffer = ( char* )malloc( data_length );
    memcpy( *buffer, data_bytes, data_length );
    *buffer_size = data_length;

    CNSLogWrite( "success" );

    return 0;
}

int system_load_file(const char* filename, void* buffer, int buffer_size)
{
    CNSLogWrite( filename );

    char str[256];
    strncpy( str, filename, 256 );
    char* stripped = strstr( str, "/" );

    CNSLogWrite( stripped+1 );

    AAsset* asset_file = AAssetManager_open( get_asset_manager(), stripped+1, AASSET_MODE_UNKNOWN );
    if( asset_file == NULL )
        return 1;

    off_t data_length = AAsset_getLength( asset_file );

    if(data_length > buffer_size)
        return 1;

    char* data_bytes = ( char* )malloc( data_length );
    AAsset_read( asset_file, data_bytes, data_length );
    AAsset_close( asset_file );

    memset(buffer, 0, buffer_size);
    memcpy(buffer, data_bytes, data_length);

    CNSLogWrite( "success" );

    return 0;
}

const char* system_get_path(const char* filename)
{
    return filename;
}

float get_device_scale(void) {
    return 1.0f;
}
float get_device_width(void) {
    return (float)_device_width;
}
float get_device_height(void) {
    return (float)_device_height;
}
