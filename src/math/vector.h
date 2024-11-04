#ifndef vector_h
#define vector_h

#include <json-c/json.h>

#include "../math/units.h"

typedef struct Vector3_t {
    real values[3];
} Vector3;

// Make a new Vector3.
Vector3* NewVector3(real x, real y, real z);

// Make a new vector3 by copying the contents of the other.
Vector3* NewVector3ByDeepCopy(Vector3* other);

// Make a new Vector3 from a json object.
Vector3* NewVector3FromJson(struct json_object* json);

// Deletes a vector, freeing its memory.
void DeleteVector3(Vector3* vec);

// Adds a to b and stores in dest.
void Vector3Add(Vector3* dest, Vector3* a, Vector3* b);

// Stores the subtraction of a-b into dest
void Vector3Sub(Vector3* dest, Vector3* a, Vector3* b);

// Normalize the given vector in-place
void Vector3Normalize(Vector3* vec);

// Determine the magnitude of the vector.
real Vector3Magnitude(Vector3* vec);

// Scale the vector in place by the provided scale factor.
void Vector3Scale(Vector3* vec, real scale);

#endif