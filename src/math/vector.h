#ifndef vector_h
#define vector_h

#include <json-c/json.h>

#include "../math/units.h"

typedef struct Vector3_t {
    real x;
    real y;
    real z;
} Vector3;

// Make a new Vector3.
Vector3* NewVector3(real x, real y, real z);

// Make a new Vector3 from a json object.
Vector3* NewVector3FromJson(struct json_object* json);

// Deletes a vector, freeing its memory.
void DeleteVector3(Vector3* vec);

#endif