#ifndef simstate_h
#define simstate_h

#include <json-c/json.h>

#include "../math/units.h"
#include "../math/vector.h"

typedef struct SimBody_t {
    char* name;
    kilograms mass;
    Vector3* position;
    Vector3* velocity;
} SimBody;

// Creates a blank SimBody.
SimBody* NewSimBody();

// Creates a new SimBody from a json object.
SimBody* NewSimBodyFromJson(struct json_object* json);

// Delete a SimBody, freeing its memory
void DeleteSimBody(SimBody* body);

typedef struct SimState_t {
    seconds time;
    biguint frame;

    biguint nBodies;
    SimBody** bodies;
} SimState;

// Create a blank SimState.
SimState* NewSimState(biguint frame, seconds time, biguint nbodies);

// Create a SimState from json
SimState* NewSimStateFromJson(struct json_object* json);

// Delete a simstate, freeing it and all the memory it owns.
void DeleteSimState(SimState* this);

// Given the previous simulation state and a reference to a body in that state,
// updates the SimBody this by applying simulation logic over the bodies in the previous state.
void AdvanceFromState(SimBody* this, SimState* previousState, SimBody* previousBody);

#endif