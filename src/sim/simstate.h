#ifndef simstate_h
#define simstate_h

#include <json-c/json.h>

#include "../math/units.h"
#include "simbody.h"

typedef struct SimState_t {
    seconds time;

    int nBodies;
    SimBody** bodies;
} SimState;

// Create a SimState from json
SimState* NewSimStateFromJson(struct json_object* json);

// Delete a simstate, freeing it and all the memory it owns.
void DeleteSimState(SimState* this);

#endif