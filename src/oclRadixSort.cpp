#include "ZSort.h"
#include <windows.h>

double PCFreq = 0.0;
__int64 CounterStart = 0;
void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
	printf("QueryPerformanceFrequency failed!\n");

    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}
int main(int argc, const char **argv)
{
    unsigned int numElements = 1048576*4;//128*128*128*2;
    float *array = (float*)malloc(numElements * sizeof(float));
    srand(95123);
    for(unsigned int i=0; i < numElements; ++i)   
    { 
    array[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX+1/2000000));
    }
    ZSort sortObject;
    StartCounter();
    sortObject.sort(array,numElements);
    printf("\nSize %d array sorted in %.6f milliseconds.\n",numElements, GetCounter() );
    while(1);
}
