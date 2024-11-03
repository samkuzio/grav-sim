#include <stdlib.h>

#include "vector.h"
#include "../log/log.h"

#define INVALID_JSON_RETURN(field) slog("failed to parse Vector3 json: %s", field); DeleteVector3(vec); return NULL;

// Make a new Vector3
Vector3* NewVector3(real x, real y, real z) {
    Vector3* vec = malloc(sizeof(Vector3));
    vec->x = x;
    vec->y = y;
    vec->z = z;
    return vec;
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
    vec->x = x;

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
    vec->y = y;

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
    vec->z = z;

    return vec;
}

void DeleteVector3(Vector3* vec) {
    if (vec != NULL) {
        free(vec);
    }
}