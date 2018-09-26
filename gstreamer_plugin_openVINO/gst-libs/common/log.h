/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __LOG_H__
#define __LOG_H__

#if (defined(ANDROID) && defined(__USE_LOGCAT__))
#include <utils/Log.h>
#define ERROR(...)
#define INFO(...)
#define WARNING(...)
#define DEBUG(...)

#ifndef PRINT_FOURCC
#define DEBUG_FOURCC(promptStr, fourcc)
#endif

#else

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#define GETTID()    syscall(__NR_gettid)

extern int oclLogFlag;
extern FILE* oclLogFn;
extern int isIni;

#ifndef OCLMESSAGE
#define oclMessage(stream, format, ...)  do {\
  fprintf(stream, format, ##__VA_ARGS__); \
}while (0)
#endif

#define OCL_DEBUG_MESSAGE(LEVEL, prefix, format, ...) \
    do {\
        if (oclLogFlag >= OCL_LOG_##LEVEL) { \
            const char* name = strrchr(__FILE__, '/'); \
            name = (name ? (name + 1) : __FILE__); \
            oclMessage(oclLogFn, "libocl %s %ld (%s, %d): " format "\n", #prefix, (long int)GETTID(), name, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)

#ifndef ERROR
#define ERROR(format, ...)  OCL_DEBUG_MESSAGE(ERROR, error, format, ##__VA_ARGS__)
#endif

#ifdef __ENABLE_DEBUG__

#ifndef WARNING
#define WARNING(format, ...)   OCL_DEBUG_MESSAGE(WARNING, warning, format, ##__VA_ARGS__)
#endif

#ifndef INFO
#define INFO(format, ...)   OCL_DEBUG_MESSAGE(INFO, info, format, ##__VA_ARGS__)
#endif

#ifndef DEBUG
#define DEBUG(format, ...)   OCL_DEBUG_MESSAGE(DEBUG, debug, format, ##__VA_ARGS__)
#endif

#ifndef DEBUG_
#define DEBUG_(format, ...)   DEBUG(format, ##__VA_ARGS__)
#endif

#ifndef DEBUG_FOURCC
#define DEBUG_FOURCC(promptStr, fourcc) do { \
  if (oclLogFlag >= OCL_LOG_DEBUG) { \
    uint32_t i_fourcc = fourcc; \
    char *ptr = (char*)(&(i_fourcc)); \
    DEBUG("%s, fourcc: 0x%x, %c%c%c%c\n", promptStr, i_fourcc, *(ptr), *(ptr+1), *(ptr+2), *(ptr+3)); \
  } \
} while(0)
#endif

#else                           //__ENABLE_DEBUG__
#ifndef INFO
#define INFO(format, ...)
#endif

#ifndef WARNING
#define WARNING(format, ...)
#endif

#ifndef DEBUG
#define DEBUG(format, ...)
#endif

#ifndef PRINT_FOURCC
#define DEBUG_FOURCC(promptStr, fourcc)
#endif

#endif                          //__ENABLE_DEBUG__
#endif                          //__ANDROID

#ifndef ASSERT
#define ASSERT(expr)               \
    do {                           \
        if (!(expr)) {             \
            ERROR("assert fails"); \
            assert(0 && (expr));   \
        }                          \
    } while (0)
#endif

#define OCL_LOG_ERROR 0x1
#define OCL_LOG_WARNING 0x2
#define OCL_LOG_INFO 0x3
#define OCL_LOG_DEBUG 0x4

void oclTraceInit();

void print_log(const char* msg, struct timespec* tmspec = NULL, struct timespec* early_time = NULL);

#endif                          //__LOG_H__