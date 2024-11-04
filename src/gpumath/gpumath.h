#ifndef gpumath_h
#define gpumath_h

#include <cuda_runtime_api.h>

#include "../math/units.h"
#include "../math/vector.h"

void gpuVecSub(Vector3* out, Vector3* a, Vector3* b, biguint n);

#endif