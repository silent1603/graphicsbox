#ifndef BASE_H
#define BASE_H
#include "config.h"
#define local_function static
#define local_variable static

#if DEBUG_BUILD
#define CONST_RELEASE
#else
#define CONST_RELEASE const
#endif
#endif