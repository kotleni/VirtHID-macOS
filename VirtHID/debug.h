//
//  debug.h
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#ifndef debug_h
#define debug_h

#include <IOKit/IOLib.h>

#define TAG "VirtHID: "

#ifdef DEBUG
    #define LogD(fmt, ...) IOLog((TAG fmt "\n"), ##__VA_ARGS__)
#else
    #define LogD(...)
#endif

#endif /* debug_h */
