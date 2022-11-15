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