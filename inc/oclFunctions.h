#ifndef OCL_FUNCS_H
#define OCL_FUNCS_H

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>


enum shrBOOL
{
    shrFALSE = 0,
    shrTRUE = 1
};

//#define oclCheckErrorEX(a, b, c) __oclCheckErrorEX(a, b, c, __FILE__ , __LINE__) 
//
//// Short version without Cleanup() callback pointer
//// Both Input (a) and Reference (b) are specified as args
//#define oclCheckError(a, b) oclCheckErrorEX(a, b, 0) 

cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID);

char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength);

#endif
