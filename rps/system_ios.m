/*  @file system_ios.m
 *  @brief iOS interface
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "system.h"

#include <string.h>

#import <UIKit/UIKit.h>

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
int system_load_file(const char* filename, void* buffer, int buffer_size)
{
    char        file[128];
    char        ext[16];
    NSString*   file_string;
    NSString*   ext_string;
    NSString*   path_name;
    NSData*     data;
    NSUInteger  data_length;
    
    _split_filename(filename, file, ext);
    file_string = [NSString stringWithCString:file encoding:NSUTF8StringEncoding];
    ext_string = [NSString stringWithCString:ext encoding:NSUTF8StringEncoding];
    path_name = [[NSBundle mainBundle] pathForResource:file_string ofType:ext_string];
    data = [NSData dataWithContentsOfFile:path_name];
    data_length = [data length];
    if(data_length > buffer_size)
        return 1;
    memset(buffer, 0, buffer_size);
    memcpy(buffer, [data bytes], data_length);
    return 0;
}
