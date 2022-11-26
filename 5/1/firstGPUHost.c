#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/opencl.h>

cl_int status;
cl_int ciErr
cl_device_id *devices = NULL;
cl_uint numDevices = 0;
char buffer[100000];
cl_uint buf_uint;
cl_ulong buf ulong;
size_t buf_size;
cl_int iNumElements = 512*512;

cl_float* srcA, *srcB, *scrC, *srcD, *srcF, result;

FILE* programHandle;
size_t programSize;
char *programBuffer;
cl_program cpProgram;
cl_kernel ckKernel;

size_t szGlobalWorkSize;
size_t szLocalWorkSize;

int main(int argc, char** argv) {
	
	szLocalWorkSize = 512;
	szGlobalWorkSize = iNumElements;
	srcA = (void *) malloc(sizeof(cl_float) * iNumElements);
	srcB = (void *) malloc(sizeof(cl_float) * iNumElements);
	srcC = (void *) malloc(sizeof(cl_float) * iNumElements);
	srcD = (void *) malloc(sizeof(cl_float) * iNumElements);
	srcE = (void *) malloc(sizeof(cl_float) * iNumElements);
	
	for (int i = 0; i<iNumElements; i++) {
		
		*((cl_float*)srcA + i) = 1.0;
		*((cl_float*)srcB + i) = 1.0;
		*((cl_float*)srcC + i) = 1.0;
		*((cl_float*)srcD + i) = 1.0;
		
	}
	
	status = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL,
		0, NULL, &numDevices);
	
	if (status != CL_SUCCESS) {
		
		printf("Error: Failed to create a device group\n");
		return EXIT_FAILURE;
		
	}
	
	printf("The number of devices found = %d \n", numDevices);
	
	devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
	if (status != CL_SUCCESS) {
		
		printf("Error: Failed to return a device group!2\n");
		return EXIT_FAILURE;
		
	}
	
	cl_context context = NULL;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	
	if(!context) {
		
		printf("Error failed to create a compute context!\n");
		reutrn EXIT_FAILURE;
		
	}
	
	cmdQueue = clCreateCommandQueue(context, devices[1], CL_QUEUE_PROFLINING_ENABLE, &status);
	if (!cmdQueue)
	{
		
		printf("Error: Failed to create a command commands!\n");
		reutrn EXIT_FAILURE;
		
	}
	
	programHandle = fopen(argv[1], "r");
	fseek(programHandle, 0, SEEK_END);
	programSize = ftell(programHandle);
	rewind(programHandle);
	
	printf("Program size = %lu B \n", programSize);
	
	programBuffer = (char*) malloc(programSize + 1);
	programBuffer[programSize] = '\0'
	fread(programBuffer, sizeof(char), prograSize, programHandle);
	fclose(programHandle);
	
	cpProgram = clCreateProgramWIthSource(context, 1, (const char **) &programBuffer, &programSize, &ciErr);
	if (!cpProgram){
		
		printf("Error failed to create compute program!\n");
		return EXIT_FAILURE;
		
	}
	
	free(programBuffer);
	
	ciErr = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
	
	if (ciErr != CL_SUCCESS) {
		
		size_t len;
		char buffer[2048];
		
		printf("Error Failed to build Program Executable!\n");
		clGetProgramBuildInfo(cpProgram, devices[1], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, buffer, &len);
		printf("%s\n", buffer);
		exit(1);
		
	}
	
	cl_mem bufferA;
	cl_mem bufferB;
	cl_mem bufferC;
	cl_mem bufferD;
	cl_mem bufferF;
	
	size_t datasize = sizeof(cl_float) * iNumElements;
	
	bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufferC = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufferD = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
	bufferF = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
	
	status = clEnqueueWriteBuffer(cmdQueue, bufferA, CL_FALSE, 0, datasize, srcA, 0, NULL, NULL);
	status = clEnqueueWriteBuffer(cmdQueue, bufferB, CL_FALSE, 0, datasize, srcB, 0, NULL, NULL);
	status = clEnqueueWriteBuffer(cmdQueue, bufferC, CL_FALSE, 0, datasize, srcC, 0, NULL, NULL);
	status = clEnqueueWriteBuffer(cmdQueue, bufferD, CL_FALSE, 0, datasize, srcD, 0, NULL, NULL);
	
	ckKernel = clCreateKernel(cpProgram, "VectorAdd", &ciErr);
	if (!ckKernel || ciErr != CL_SUCCESS){
		
		printf("Error failed to create compute Kernel!\n");
		exit(1);
		
	}
	
	ciErr = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*) &bufferA);
	ciErr |= clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*) &bufferB);
	ciErr |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*) &bufferC);
	ciErr |= clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void*) &bufferD);
	ciErr |= clSetKernelArg(ckKernel, 4, sizeof(cl_mem), (void*) &bufferF);
	ciErr |= clSetKernelArg(ckKernel, 5, sizeof(cl_mem), (void*) &iNumElements);
	
	ciErr = clEnqueueNDRangeKernel(cmdQueue, ckKernel, 1, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, NULL);
	
	if (ciErr != CL_SUCCESS){
		printf("Error launching Kernel");
		
	clFinish(cmdQueue);
	
	ciErr = clEnqueueReadBuffer(cmdQueue, bufferF, CL_TRUE, 0, datasize, srcC, 0, NULL, NULL);
	
	clFinish(cmdQueue);
	
	result = 0.0;
	for (int i = 0; i < iNumElements; i++) {
		result += srcC[i];
	}
	
	printf("Result = %f\n", result);
	
	free(srcA);
	free(srcB);
	free(srcC);
	free(srcD);
	free(srcF);
	
	if(ckKernel) clReleaseKernel(ckKernel);
	if(cpProgram) clReleaseProgram(cpProgram);
	if(cmdQueue) clReleaseCommandQueue(cmdQueue);
	if(context) clReleaseContext(context);
	if(bufferA) clReleaseMemObject(bufferA);
	if(bufferB) clReleaseMemObject(bufferB);
	if(bufferC) clReleaseMemObject(bufferC);
	if(bufferD) clReleaseMemObject(bufferD);
	if(bufferF) clReleaseMemObject(bufferF);
	
	return 0;
	
}

__kernel void VectorAdd( __global float* a, __global float* b, __global float* c,
__global float* d, __global float* f, int iNumElements)
{
	int iGID = get_global_id(0);
	if (iGID < iNumElements)
		f[iGID] = a[iGID] + b[iGID] + c[iGID] + d[iGID];
	
}