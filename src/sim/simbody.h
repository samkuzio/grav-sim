#ifndef simbody_h
#define simbody_h

#include <json-c/json.h>

#include "../math/units.h"
#include "../math/vector.h"

typedef struct SimBody_t {
    char* name;
    kilograms mass;
    Vector3* position;
    Vector3* velocity;
} SimBody;

// Creates a new SimBody from a json object.
SimBody* NewSimBodyFromJson(struct json_object* json);

// Delete a SimBody, freeing its memory
void DeleteSimBody(SimBody* body);

#endif