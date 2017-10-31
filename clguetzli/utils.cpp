/*****************************************************************************
 * Copyright (c) 2013-2016 Intel Corporation
 * All rights reserved.
 *
 * WARRANTY DISCLAIMER
 *
 * THESE MATERIALS ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THESE
 * MATERIALS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Intel Corporation is the author of the Materials, and requests that all
 * problem reports or change requests be submitted to it directly
 *****************************************************************************/
#ifdef __USE_OPENCL__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include "CL/cl.h"
#include "CL/cl_ext.h"
#include "utils.h"
#include <assert.h>

#if defined(_WIN32) || defined(WIN32)        /**Windows*/
#define WINDOWS_IMPL
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(BSD)    /**Linux*/
#define LINUX_IMPL
#include <sys/time.h>        //gettimeofday()
#endif

//we want to use POSIX functions
#pragma warning( push )
#pragma warning( disable : 4996 )


void LogInfo(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stdout, str, args);

        va_end(args);
    }
}

void LogError(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stderr, str, args);

        va_end(args);
    }
}

// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize)
{
    int errorCode = CL_SUCCESS;

    FILE* fp = NULL;
#ifdef __linux__
    fp = fopen(fileName, "rb");
#else
    fopen_s(&fp, fileName, "rb");
#endif
    if (fp == NULL)
    {
        LogError("Error: Couldn't find program source file '%s'.\n", fileName);
        errorCode = CL_INVALID_VALUE;
    }
    else {
        fseek(fp, 0, SEEK_END);
        *sourceSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *source = new char[*sourceSize];
        if (*source == NULL)
        {
            LogError("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
            errorCode = CL_OUT_OF_HOST_MEMORY;
        }
        else {
            fread(*source, 1, *sourceSize, fp);
        }
    }
    return errorCode;
}
#pragma warning( pop )

timeCounter::timeCounter(const char *tag) {
	this->tag = tag;
#ifdef WINDOWS_IMPL
	LARGE_INTEGER frequency;
	LARGE_INTEGER timeStart;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&timeStart);
	this->timeStart = timeStart.QuadPart;
	this->frequency = frequency.QuadPart;
#elif  defined(LINUX_IMPL)
	struct timeval tpstart;
	gettimeofday(&tpstart, NULL);
	tv_sec = tpstart.tv_sec;
	tv_usec = tpstart.tv_usec;
#endif
}
timeCounter::~timeCounter() {
#ifdef WINDOWS_IMPL
	LARGE_INTEGER timeEnd;
	QueryPerformanceCounter(&timeEnd);
	double elapsed = (timeEnd.QuadPart - timeStart) / frequency;
	LogError("%s %.3fms\n", tag, elapsed * 1000);
#elif  defined(LINUX_IMPL)
	struct timeval tpend;
	gettimeofday(&tpend, NULL);
	double timeuse = 1000000 * (tpend.tv_sec - tv_sec) + tpend.tv_usec - tv_usec;
	LogError("%s %.3fms\n", tag, timeuse / 1000);
#endif
}

#endif