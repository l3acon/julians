#ifndef _ZSORT_H_
#define _ZSORT_H_

#include "oclFunctions.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#define MAX_GPU_COUNT 8

class ZSort
{
public:
	ZSort();
	//~ZSort();

	void sort(float* sortArray,int numberofValues);

private:

    static const int keybits = 32; // bit size of uint 

    cl_platform_id cpPlatform;                      // OpenCL platform
    cl_uint nDevice;                                // OpenCL device count
    cl_device_id* cdDevices;                        // OpenCL device list    
	cl_context cxGPUContext;                        // OpenCL context
    cl_command_queue cqCommandQueue[MAX_GPU_COUNT]; // OpenCL command que
	cl_int ciErrNum;
    double secs;
    struct timeval start , end;

    bool verifySortUint(unsigned int *keysSorted, unsigned int *valuesSorted, unsigned int *keysUnsorted, unsigned int len);
    void makeRandomUintVector(unsigned int *a, unsigned int numElements, unsigned int keybits);
};
#endif
