#include <fcntl.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>

__kernel void VectorAdd( __global float* a, __global float* b, __global float* c, __global float* d, __global float* f, int iNumElements)
{
	int iGID = get_global_id(0);
	if (iGID < iNumElements)
		f[iGID] = a[iGID] + b[iGID] + c[iGID] + d[iGID];
	
}