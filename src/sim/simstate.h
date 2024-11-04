#ifndef simstate_h
#define simstate_h

#include <json-c/json.h>

#include "../math/units.h"
#include "../math/vector.h"

typedef struct SimBody_t {
    char* name;
    kilograms mass;
    Vector3* initialPosition;
    Vector3* initialVelocity;
} SimBody;

// Creates a blank SimBody.
SimBody* NewSimBody();

// Perform a deep copy of the other sim body.
SimBody* NewSimBodyByDeepCopy(SimBody* other);

// Creates a new SimBody from a json object.
SimBody* NewSimBodyFromJson(struct json_object* json);

// Delete a SimBody, freeing its memory
void DeleteSimBody(SimBody* body);

typedef struct SimState_t {
    seconds time;
    biguint frame;

    biguint nBodies;
    SimBody** bodies;
    Vector3* bodyPositions;
    Vector3* bodyVelocities;
} SimState;

// Create a blank SimState by performing a deep copy of the other.
SimState* NewSimStateByDeepCopy(SimState* other);

// Create a SimState from json
SimState* NewSimStateFromJson(struct json_object* json);

// Delete a simstate, freeing it and all the memory it owns.
void DeleteSimState(SimState* this);

// Given the current simulation state and a reference to a body in that state,
// updates the SimBody this by applying simulation logic over the bodies in the previous state.
void AdvanceFromStateGPU(seconds deltaT, SimBody* this, biguint bodyIndex, SimState* currentState, SimState* nextState);

// Given the current simulation state and a reference to a body in that state,
// updates the SimBody this by applying simulation logic over the bodies in the previous state.
void AdvanceFromStateCPU(seconds deltaT, SimBody* this, biguint bodyIndex, SimState* currentState, SimState* nextState);

#endif