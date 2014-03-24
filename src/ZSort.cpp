


#include "RadixSort.h"
#include "ZSort.h"






ZSort::ZSort(){

    //shrQAStart(argc, (char **)argv);

	//shrSetLogFileName ("oclRadixSort.txt");
	printf("starting...\n\n");

    printf("clGetPlatformID...\n"); 
    ciErrNum = oclGetPlatformID(&cpPlatform);
    //oclCheckError(ciErrNum, CL_SUCCESS);

    printf("clGetDeviceIDs...\n"); 
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &nDevice);
    //oclCheckError(ciErrNum, CL_SUCCESS);
    cdDevices = (cl_device_id *)malloc(nDevice * sizeof(cl_device_id) );
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, nDevice, cdDevices, NULL);
    //oclCheckError(ciErrNum, CL_SUCCESS);

    printf("clCreateContext...\n"); 
    cxGPUContext = clCreateContext(0, nDevice, cdDevices, NULL, NULL, &ciErrNum);
    //oclCheckError(ciErrNum, CL_SUCCESS);

    printf("Create command queue...\n\n");
    //int id_device;
    //if(shrGetCmdLineArgumenti(argc, argv, "device", &id_device)) // Set up command queue(s) for GPU specified on the command line
    //{
    //    // get & log device index # and name
    //    cl_device_id cdDevice = cdDevices[id_device];

    //    // create a command que
    //    cqCommandQueue[0] = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErrNum);
    //    oclCheckError(ciErrNum, CL_SUCCESS);
    //    oclPrintDevInfo(LOGBOTH, cdDevice);
    //    nDevice = 1;   
    //} 
    //else 
    //{ // create command queues for all available devices        
        for (cl_uint i = 0; i < nDevice; i++) 
        {
            cqCommandQueue[i] = clCreateCommandQueue(cxGPUContext, cdDevices[i], 0, &ciErrNum);
            //oclCheckError(ciErrNum, CL_SUCCESS);
        }
        //for (cl_uint i = 0; i < nDevice; i++) oclPrintDevInfo(LOGBOTH, cdDevices[i]);
    //}

	

}

void ZSort::sort(float* sortArray,int numberofValues){
    int ctaSize=128;
	/*if (!shrGetCmdLineArgumenti(argc, argv, "work-group-size", &ctaSize)) 
	{
		ctaSize = 128;
	}*/

    printf("Running Radix Sort on %d GPU(s) ...\n\n", nDevice);

	unsigned int numElements = numberofValues;

    // Alloc and init some data on the host, then alloc and init GPU buffer  
    unsigned int **h_keys       = (unsigned int**)malloc(nDevice * sizeof(unsigned int*));
    unsigned int **h_keysSorted = (unsigned int**)malloc(nDevice * sizeof(unsigned int*));
    cl_mem       *d_keys        = (cl_mem*       )malloc(nDevice * sizeof(cl_mem));
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
        h_keys[iDevice]       = (unsigned int*)malloc(numElements * sizeof(unsigned int));
	    h_keysSorted[iDevice] = (unsigned int*)malloc(numElements * sizeof(unsigned int));
        memcpy( h_keys[iDevice], sortArray, sizeof(unsigned int)*numberofValues );
        //makeRandomUintVector(h_keys[iDevice], numElements, keybits);

        d_keys[iDevice] = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, 
            sizeof(unsigned int) * numElements, NULL, &ciErrNum);
        ciErrNum |= clEnqueueWriteBuffer(cqCommandQueue[iDevice], d_keys[iDevice], CL_TRUE, 0, 
            sizeof(unsigned int) * numElements, h_keys[iDevice], 0, NULL, NULL);
        //oclCheckError(ciErrNum, CL_SUCCESS);
    }
	
    // instantiate RadixSort objects
    RadixSort **radixSort = (RadixSort**)malloc(nDevice * sizeof(RadixSort*));
    //StartCounter();
    printf("Number of devices: %i\n",nDevice);
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
	    radixSort[iDevice] = new RadixSort(cxGPUContext, cqCommandQueue[iDevice], numElements, "./", ctaSize, true);		    
    }
        for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
        {
	        radixSort[iDevice]->sort(d_keys[iDevice], 0, numElements, keybits);
        }
    // copy sorted keys to CPU 
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
	    clEnqueueReadBuffer(cqCommandQueue[iDevice], d_keys[iDevice], CL_TRUE, 0, sizeof(unsigned int) * numElements, 
            h_keysSorted[iDevice], 0, NULL, NULL);
    }
    //printf("Size %d array sorted in %.6f milliseconds.\n",numElements, GetCounter() );
	// Check results
	bool passed = true;
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
	    passed &= verifySortUint(h_keysSorted[iDevice], NULL, h_keys[iDevice], numElements);
    }
    printf((passed?"Passed":"Failed"));
    // cleanup allocs
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
        clReleaseMemObject(d_keys[iDevice]);
	    free(h_keys[iDevice]);
	    free(h_keysSorted[iDevice]);
        delete radixSort[iDevice];
    }
    free(radixSort);
    free(h_keys);
    free(h_keysSorted);
    
    // remaining cleanup and exit
	free(cdDevices);
    for (cl_uint iDevice = 0; iDevice < nDevice; iDevice++)
    {
	    clReleaseCommandQueue(cqCommandQueue[iDevice]);
    }
    clReleaseContext(cxGPUContext);

    // finish
    //shrQAFinishExit(argc, (const char **)argv, passed ? QA_PASSED : QA_FAILED);

    //shrEXIT(argc, argv);
}

void ZSort::makeRandomUintVector(unsigned int *a, unsigned int numElements, unsigned int keybits)
{
    // Fill up with some random data
    //int keyshiftmask = 0;
    //if (keybits > 16) keyshiftmask = (1 << (keybits - 16)) - 1;
    //int keymask = 0xffff;
    //if (keybits < 16) keymask = (1 << keybits) - 1;

    srand(95123);
    for(unsigned int i=0; i < numElements; ++i)   
    { 
        reinterpret_cast<float&>(a[i]) = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX+1/2000000));
        //a[i] = ((rand() & keyshiftmask)<<16) | (rand() & keymask); 
    }
}

// assumes the values were initially indices into the array, for simplicity of 
// checking correct order of values
bool ZSort::verifySortUint(unsigned int *keysSorted, 
					unsigned int *valuesSorted, 
					unsigned int *keysUnsorted, 
					unsigned int len)
{
    bool passed = true;
    for(unsigned int i=0; i<len-1; ++i)
    {
        if( reinterpret_cast<float&>(keysSorted[i])>reinterpret_cast<float&>(keysSorted[i+1]) )
		{
			printf("Unordered key[%d]: %d > key[%d]: %d\n", i, keysSorted[i], i+1, keysSorted[i+1]);
			passed = false;
			break;
		}
    }

    if (valuesSorted)
    {
        for(unsigned int i=0; i<len; ++i)
        {
            if( reinterpret_cast<float&>(keysUnsorted[valuesSorted[i]]) != reinterpret_cast<float&>(keysSorted[i]) )
            {
                printf("Incorrectly sorted value[%u] (%u): %u != %u\n", 
					i, valuesSorted[i], keysUnsorted[valuesSorted[i]], keysSorted[i]);
                passed = false;
                break;
            }
        }
    }

    return passed;
}
