extern "C" {
    #include "gpumath.h"
}

#include "../math/units.h"
#include "../math/vector.h"

__global__ void gpuVecSub_kernel(Vector3* out, Vector3* a, Vector3* b, biguint n) {
    int id = blockDim.x * blockIdx.x + threadIdx.x;
    if (id > n) return;

    out[id].values[0] = a[id].values[0] - b[id].values[0];
    out[id].values[1] = a[id].values[1] - b[id].values[1];
    out[id].values[2] = a[id].values[2] - b[id].values[2];
}

extern "C"
void gpuVecSub(Vector3* out, Vector3* a, Vector3* b, biguint n) {
    Vector3* gpuOut;
    Vector3* gpuA;
    Vector3* gpuB;
    cudaMalloc(&gpuOut, sizeof(Vector3) * n);
    cudaMalloc(&gpuA, sizeof(Vector3) * n);
    cudaMalloc(&gpuB, sizeof(Vector3) * n);

    cudaMemcpy(gpuA, a, sizeof(Vector3) * n, cudaMemcpyHostToDevice);
    cudaMemcpy(gpuB, b, sizeof(Vector3) * n, cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksInGrid = ceil(float(n) / threadsPerBlock);
    gpuVecSub_kernel<<<blocksInGrid, threadsPerBlock>>>(gpuOut, gpuA, gpuB, n);

    cudaMemcpy(out, gpuOut, sizeof(Vector3) * n, cudaMemcpyDeviceToHost);
    cudaFree(gpuOut);
    cudaFree(gpuA);
    cudaFree(gpuB);
}