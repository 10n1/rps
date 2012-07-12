/*! @file system.h
 *  @brief System-specific functions
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef _system_h__
#define _system_h__

#ifdef __cplusplus
extern "C" { // Use C linkage
#endif 

int system_load_file(const char* filename, void* buffer, int buffer_size);
const char* system_get_path(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
