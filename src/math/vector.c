#include <stdlib.h>
#include <math.h>

#include "vector.h"
#include "../log/log.h"

#define INVALID_JSON_RETURN(field) slog("failed to parse Vector3 json: %s", field); DeleteVector3(vec); return NULL;

// Make a new Vector3
Vector3* NewVector3(real x, real y, real z) {
    Vector3* vec = malloc(sizeof(Vector3));
    vec->values[0] = x;
    vec->values[1] = y;
    vec->values[2] = z;
    return vec;
}

// Make a new vector3 by copying the contents of the other.
Vector3* NewVector3ByDeepCopy(Vector3* other) {
    Vector3* this = NewVector3(other->values[0], other->values[1], other->values[2]);
    return this;
}

// Make a new Vector3 from a json object.
Vector3* NewVector3FromJson(struct json_object* json) {
    Vector3* vec = calloc(1, sizeof(Vector3));

    if (!json_object_is_type(json, json_type_object)) {
        INVALID_JSON_RETURN("json is not an object type");
    }

    // Extract x value
    struct json_object* xStringObj;
    if(!json_object_object_get_ex(json, "x", &xStringObj)) {
        INVALID_JSON_RETURN("x is missing");
    }
    if(!json_object_is_type(xStringObj, json_type_string)) {
        INVALID_JSON_RETURN("x is not a string");
    }
    const char* xString = json_object_get_string(xStringObj);
    char* endptr;
    real x = strtold(xString, &endptr);
    if (*endptr != '\0') {
        INVALID_JSON_RETURN("x string could not be converted to numeric type");
    }
    vec->values[0] = x;

    // Extract y value
    struct json_object* yStringObj;
    if(!json_object_object_get_ex(json, "y", &yStringObj)) {
        INVALID_JSON_RETURN("y is missing");
    }
    if(!json_object_is_type(yStringObj, json_type_string)) {
        INVALID_JSON_RETURN("y is not a string");
    }
    const char* yString = json_object_get_string(yStringObj);
    real y = strtold(yString, &endptr);
    if (*endptr != '\0') {
        INVALID_JSON_RETURN("y string could not be converted to numeric type");
    }
    vec->values[1] = y;

    // Extract z value
    struct json_object* zStringObj;
    if(!json_object_object_get_ex(json, "z", &zStringObj)) {
        INVALID_JSON_RETURN("z is missing");
    }
    if(!json_object_is_type(zStringObj, json_type_string)) {
        INVALID_JSON_RETURN("z is not a string");
    }
    const char* zString = json_object_get_string(zStringObj);
    real z = strtold(zString, &endptr);
    if (*endptr != '\0') {
        INVALID_JSON_RETURN("x string could not be converted to numeric type");
    }
    vec->values[2] = z;

    return vec;
}

void DeleteVector3(Vector3* vec) {
    if (vec != NULL) {
        free(vec);
    }
}

// Adds a to b and stores in dest.
void Vector3Add(Vector3* dest, Vector3* a, Vector3* b) {
    dest->values[0] = a->values[0] + b->values[0];
    dest->values[1] = a->values[1] + b->values[1];
    dest->values[2] = a->values[2] + b->values[2];
}

// Stores the subtraction of a-b into dest
void Vector3Sub(Vector3* dest, Vector3* a, Vector3* b) {
    dest->values[0] = a->values[0] - b->values[0];
    dest->values[1] = a->values[1] - b->values[1];
    dest->values[2] = a->values[2] - b->values[2];
}

// Normalize the given vector in-place
void Vector3Normalize(Vector3* vec) {
    real mag = Vector3Magnitude(vec);
    vec->values[0] = vec->values[0] / mag;
    vec->values[1] = vec->values[1] / mag;
    vec->values[2] = vec->values[2] / mag;
}

// Determine the magnitude of the vector.
real Vector3Magnitude(Vector3* vec) {
    return sqrt(
        (vec->values[0] * vec->values[0])
        + (vec->values[1] * vec->values[1])
        + (vec->values[2] * vec->values[2])
    );
}

// Scale the vector in place by the provided scale factor.
void Vector3Scale(Vector3* vec, real scale) {
    vec->values[0] *= scale;
    vec->values[1] *= scale;
    vec->values[2] *= scale;
}