#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stats.h>
#include <OpenCL/opencl.h>

cl_int status;
cl_int ciErr;
cl_device_id *devices = NULL;
cl_uint numDevices = 0;
char buffer[100000];
cl_uint buf_uint;
cl_ulong buf_ulong;
size_t buf_sizet;
cl_int iNumElements = 512*512;

cl_float* srcA;
cl_float* srcB;
cl_float* srcC;
cl_float* srcD;
cl_float* srcE;
cl_float result;

FILE* programHandle;            // File that contains kernel functions
size_t programSize;
char *programBuffer;
cl_program cpProgram;           // OpenCL program
cl_kernel ckKernel;             // OpenCL kernel

size_t szGlobalWorkSize;        // global work size
size_t szLocalWorkSize;         // local work size

// Main function
// ******************************************************
int main(int argc, char **argv) 
{
    // Set and log Global and Local work size dimensions
    szLocalWorkSize = 512;
    szGlobalWorkSize = iNumElements;

    // Allocate host arrays
    srcA = (void *)malloc(sizeof(cl_float) * iNumElements);
    srcB = (void *)malloc(sizeof(cl_float) * iNumElements);
    srcC = (void *)malloc(sizeof(cl_float) * iNumElements);
    srcD = (void *)malloc(sizeof(cl_float) * iNumElements);
    srcE = (void *)malloc(sizeof(cl_float) * iNumElements);

    // init arrays:
    for(int i = 0; i < iNumElements; i++) {
        *((cl_float*)srcA + i) = 1.0;
        *((cl_float*)srcB + i) = 1.0;
        *((cl_float*)srcC + i) = 1.0;
        *((cl_float*)srcD + i) = 1.0;

    }
    
    // **************************************************
    // STEP 1: Discover and initialize the devices
    // **************************************************

    // Use clGetDeviceIDs() to retrieve the number of
    // devices present
    status = clGetDeviceIDs(
        NULL,
        CL_DEVICE_TYPE_ALL,
        0,
        NULL,
        &numDevices
    );

    if(status != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }

    printf("The number of devices found = %d \n", numDevices);

    // Allocate enough space for each device
    devices = (cl_device_id*) malloc(numDevices*sizeof(cl_device_id));

    // Fill in devices with clGetDeviceIDs()
    status = clGetDeviceIDs(
        NULL,
        CL_DEVICE_TYPE_ALL,
        numDevices,
        devices,
        NULL
    );

    if(status != CL_SUCCESS )
    {
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }

    // **************************************************
    // STEP 2: Create a context
    // **************************************************

    cl_context context = NULL;
    // Create a context using clCreateContext() and
    // associate it with the devices
    context = clCreateContext(
        NULL,
        numDevices,
        devices,
        NULL,
        NULL,
        &status
    );

    if(!context) {
        printf("Error: Failed to create a compute context!\n");
        return EXIT_FAILURE;
    }

    // **************************************************
    // STEP 3: Create a command queue
    // **************************************************

    cl_command_queue cmdQueue;

    // Create a command queue using clCreateCommandQueue(),
    // and associate it with the device you want to execute
    // on

    cmdQueue = clCreateCommandQueue(
        context,
        devices[1],     // GPU
        CL_QUEUE_PROFILING_ENABLE,
        &status
    );

    if(!cmdQueue) {
        printf("Error: Failed to create a command queue!\n");
        return EXIT_FAILURE;
    }

    // **************************************************
    // STEP 4: Create the program object for a context
    // **************************************************

    // 4a: Read the OpenCL kernel from the cource file and
    //     get the size of the kernel source
    programHandle = fopen("PATH_TO_SOMETHING", "r");
    fseek(programHandle, 0, SEEK_END);
    programSize = ftell(programHandle);
    rewind(programHandle);

    printf("Program size = %lu B \n", programSize);

    // 4 b: Read the OpenCL kernel from the source file and
    //      get the size of the kernel source 
    programBuffer = (char*) malloc(programSize + 1);

    programBuffer[programSize] = '\0'; // add null-termination
    fread(programBuffer, sizeof(char), programSize, programHandle);
    fclose(programHandle);

    // 4c: Create the program from the source
    cpProgram = clCreateProgramWithSource(
        context,
        1,
        (const char **)&programBuffer,
        &programSize,
        &ciErr
    );

    if(!cpProgram) {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }

    free(programBuffer);

    
    // **************************************************
    // STEP 5: Build the program
    // **************************************************

    ciErr = clBuildProgram(
        cpProgram,
        0,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if(ciErr != CL_SUCCESS) {
        size_t len;
        char bueffer[2048];

        printf("Error: Faild to build program executable!\n");
        clGetProgramBuildInfo(
            cpProgram,
            devices[1],
            CL_PROGRAM_BUILD_LOG,
            sizeof(buffer),
            buffer,
            &len
        );

        printf("%s\n", buffer);
        exit(1);
    }

    
    // **************************************************
    // STEP 6: Create device buffers
    // **************************************************

    cl_mem bufferA; // Input array on the device
    cl_mem bufferB; // Input array on the device
    cl_mem bufferC; // Input array on the device
    cl_mem bufferD; // Input array on the device
    cl_mem bufferE; // Output array on the device
    // cl_mem noElements;

    // Size of data:
    size_t datasize = sizeof(cl_float) * iNumElements;

    // Use clCreateBuffer() to create a buffer object (d_A)
    // that will contain the data from the host array A
    bufferA = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status
    );

    // Use clCreateBuffer() to create a buffer object (d_B)
    // that will contain the data from the host array B
    bufferB = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status
    );

    // Use clCreateBuffer() to create a buffer object (d_C)
    // that will contain the data from the host array C
    bufferC = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status
    );

    // Use clCreateBuffer() to create a buffer object (d_D)
    // that will contain the data from the host array D
    bufferD = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status
    );

    // Use clCreateBuffer() to create a buffer object (d_E)
    // that will contain the data from the host array E
    bufferE = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        datasize,
        NULL,
        &status
    );

    // **************************************************
    // STEP 7: Write host data to device buffers
    // **************************************************    

    // Use clEnqueueWriteBuffer() to write input array A to
    // the device buffer bufferA
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferA,
        CL_FALSE,
        0,
        datasize,
        srcA,
        0,
        NULL,
        NULL
    );

    // Use clEnqueueWriteBuffer() to write input array B to
    // the device buffer bufferB
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferB,
        CL_FALSE,
        0,
        datasize,
        srcB,
        0,
        NULL,
        NULL
    );

    // Use clEnqueueWriteBuffer() to write input array C to
    // the device buffer bufferC
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferC,
        CL_FALSE,
        0,
        datasize,
        srcC,
        0,
        NULL,
        NULL
    );

    // Use clEnqueueWriteBuffer() to write input array D to
    // the device buffer bufferD
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferD,
        CL_FALSE,
        0,
        datasize,
        srcD,
        0,
        NULL,
        NULL
    );

    
    // **************************************************
    // STEP 8: Create and compile the kernel
    // **************************************************

    // Create the kernel
    ckKernel = clCreateKernel(
        cpProgram,
        "VectorAdd",
        &ciErr
    );

    if(!ckKernel || ciErr != CL_SUCCESS) {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }

    // **************************************************
    // STEP 9: Set the kernel arguments
    // **************************************************

    // Set the argument values
    ciErr = clSetKernelArg(
        ckKernel,
        0,
        sizeof(cl_mem),
        (void*)&bufferA
    );

    ciErr |= clSetKernelArg(
        ckKernel,
        1,
        sizeof(cl_mem),
        (void*)&bufferB
    );

    ciErr |= clSetKernelArg(
        ckKernel,
        2,
        sizeof(cl_mem),
        (void*)&bufferC
    );

    ciErr |= clSetKernelArg(
        ckKernel,
        3,
        sizeof(cl_mem),
        (void*)&bufferD
    );

    ciErr |= clSetKernelArg(
        ckKernel,
        4,
        sizeof(cl_mem),
        (void*)&bufferE
    );

    ciErr |= clSetKernelArg(
        ckKernel,
        5,
        sizeof(cl_int),
        (void*)&iNumElements
    );

    // **************************************************
    // Start Core sequence... copy input data to GPU, 
    // compute, copy results back

    // **************************************************
    // STEP 10: Enqueue the kernel for execution
    // **************************************************
    // Launch kernel
    ciErr = clEnqueueNDRangeKernel(
        cmdQueue,
        ckKernel,
        1,
        NULL,
        &szGlobalWorkSize,
        &szLocalWorkSize,
        0,
        NULL,
        NULL
    );

    if(ciErr != CL_SUCCESS) {
        printf("Error launching kernel!\n");
    }

    // Wait for the command commands to get serviced before
    //  reading back results

    clFinish(cmdQueue);

    // **************************************************
    // STEP 11: Read the output buffer back to the host
    // **************************************************
    // Synchronous/blocking read of results
    ciErr = clEnqueueReadBuffer(
        cmdQueue,
        bufferE,
        CL_TRUE,
        0,
        datasize,
        srcE,
        0,
        NULL,
        NULL
    );

    // Wait for the command commands to get serviced before
    //  reading back results
    clfinish(cmdQueue);

    // check the result
    result = 0.0;
    for(int i = 0; i < iNumElements; i++) {
        result += srcE[i];
    }
    printf("Result = %f \n", result);

    // Cleanup
    free(srcA);
    free(srcB);
    free(srcC);
    free(srcD);
    free(srcE);   

    if(ckKernel) clReleaseKernel(ckKernel);
    if(cpProgram) clReleaseProgram(cpProgram);
    if(cmdQueue) clReleaseCommandQueue(cmdQueue);
    if(context) clReleaseContext(context);

    if(bufferA) clReleaseMemObject (bufferA);
    if(bufferB) clReleaseMemObject (bufferB);
    if(bufferC) clReleaseMemObject (bufferC);
    if(bufferD) clReleaseMemObject (bufferD);
    if(bufferE) clReleaseMemObject (bufferE);
}    

// OpenCl Kernel Function for element by element
// vector addition
__kernel void VectorAdd(__global float *a,
                        __global float *b,
                        __global float *c,
                        __global float *d,
                        __global float *e,
                        int iNumElements)
{
    // find my global index and handsle the data at this index
    int iGID = get_global_id(0);

    if (iGID < iNumElements)
    {
        // add adjacent elements
        e[iGID] = a[iGID] + b[iGID] + c[iGID] + d[iGID];
    }
}