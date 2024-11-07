#include <string.h>

#include "simstate.h"
#include "../log/log.h"
#include "../gpumath/gpumath.h"
#include "../math/constants.h"

// Creates a blank SimBody.
SimBody* NewSimBody() {
    SimBody* body = calloc(1, sizeof(SimBody));
    body->initialPosition = NewVector3(0, 0, 0);
    body->initialVelocity = NewVector3(0, 0, 0);
    return body;
}

// Perform a deep copy of the other sim body.
SimBody* NewSimBodyByDeepCopy(SimBody* other) {
    SimBody* this = malloc(sizeof(SimBody));
    this->name = strdup(other->name);
    this->mass = other->mass;
    this->initialPosition = NewVector3ByDeepCopy(other->initialPosition);
    this->initialVelocity = NewVector3ByDeepCopy(other->initialVelocity);
    return this;
}

#define INVALID_JSON_RETURN(field) slog("failed to parse SimBody json: %s", field); DeleteSimBody(body); return NULL;

// Creates a new SimBody from a json object.
SimBody* NewSimBodyFromJson(struct json_object* json) {
    SimBody* body = calloc(1, sizeof(SimBody));

    // Extract name from json and allocate it as a new object.
    struct json_object* nameObject;
    if (!json_object_object_get_ex(json, "name", &nameObject)) {
        INVALID_JSON_RETURN("name is missing");
    }
    if (!json_object_is_type(nameObject, json_type_string)) {
        INVALID_JSON_RETURN("name is not a string");
    }
    body->name = strdup(json_object_get_string(nameObject));

    // Extract the mass from json and parse it as a long double.
    struct json_object* massObject;
    if(!json_object_object_get_ex(json, "mass", &massObject)) {
        INVALID_JSON_RETURN("mass is missing");
    }
    if (!json_object_is_type(massObject, json_type_string)) {
        INVALID_JSON_RETURN("mass is not a string");
    }
    const char* massString = json_object_get_string(massObject);
    char* endptr;
    kilograms mass = strtold(massString, &endptr);
    if (*endptr != '\0') {
        INVALID_JSON_RETURN("mass string could not be converted to numeric type");
    }
    body->mass = mass;

    // Extract position
    struct json_object* positionObject;
    if (!json_object_object_get_ex(json, "position", &positionObject)) {
        INVALID_JSON_RETURN("position is not defined or is not an object");
    }
    body->initialPosition = NewVector3FromJson(positionObject);
    if (body->initialPosition == NULL) {
        INVALID_JSON_RETURN("position could not be inflated");
    }

    // Extract velocity
    struct json_object* velocityObject;
    if (!json_object_object_get_ex(json, "velocity", &velocityObject)) {
        INVALID_JSON_RETURN("velocity is not defined or is not an object");
    }
    body->initialVelocity = NewVector3FromJson(velocityObject);
    if (body->initialVelocity == NULL) {
        INVALID_JSON_RETURN("velocity could not be inflated");
    }

    return body;
}

// Delete a SimBody, freeing its memory
void DeleteSimBody(SimBody* this) {
    if (this != NULL) {
        free(this->name);
        DeleteVector3(this->initialPosition);
        DeleteVector3(this->initialVelocity);
        free(this);
    }
}

/* GPU support is disabled for now
void AdvanceFromStateGPU(seconds detltaT, SimBody* this, biguint bodyIndex, SimState* currentState, SimState* nextState) {
    // Compute the normalized direction vectors from all other bodies to this body.
    // Initialize our output vector.
    Vector3* normalDirectionVectors = (Vector3*)malloc(sizeof(Vector3) * currentState->nBodies);

    // Define a vector of the position of this body, repeated for every body in the sim.
    // When we do vector subtraction A-B, this will be B.
    Vector3* thisPositionArray = (Vector3*)malloc(sizeof(Vector3) * currentState->nBodies);
    for (biguint i = 0; i < currentState->nBodies; i++) {
        memcpy(&(thisPositionArray[i]), &(currentState->bodyPositions[bodyIndex]), sizeof(Vector3));
    }

    gpuVecSub(normalDirectionVectors, currentState->bodyPositions, thisPositionArray, currentState->nBodies);

    // Inefficient implemenation. Ideally the simstate would store all positions contiguously,
    // so that duplicating and reshaping what is in memory already is not necessary.
    free(thisPositionArray);
    free(normalDirectionVectors);
}*/

// Given the previous simulation state and a reference to a body in that state,
// updates the SimBody this by applying simulation logic over the bodies in the previous state.
void AdvanceFromStateCPU(seconds deltaT, SimBody* this, biguint bodyIndex, SimState* currentState, SimState* nextState) {
    // First determine the net force on the object from all others.
    Vector3 netForce = {{0.0, 0.0, 0.0}};
    for (biguint i = 0; i < currentState->nBodies; i++) {
        // This body does not act on itself.
        if (i == bodyIndex) continue;

        SimBody* other = currentState->bodies[i];

        Vector3 vec = {{0.0, 0.0, 0.0}};
        Vector3Sub(&vec, &(currentState->bodyPositions[i]), &(currentState->bodyPositions[bodyIndex]));
        real distance = Vector3Magnitude(&vec);
        Vector3Normalize(&vec);

        real scale = (G * this->mass * other->mass) / (distance * distance);
        Vector3Scale(&vec, scale);

        Vector3Add(&netForce, &netForce, &vec);
    }

    // Calculate acceleration during this frame.
    Vector3 acceleration = {{0.0, 0.0, 0.0}};
    Vector3Add(&acceleration, &acceleration, &netForce);
    Vector3Scale(&acceleration, 1 / this->mass);

    // Initial velocity
    Vector3 initialVelocity = {{0.0, 0.0, 0.0}};
    Vector3Add(&initialVelocity, &initialVelocity, &(currentState->bodyVelocities[bodyIndex]));

    // Velocity change due to acceleration
    Vector3 deltaVelocity = {{0.0, 0.0, 0.0}};
    Vector3Add(&deltaVelocity, &deltaVelocity, &acceleration);
    Vector3Scale(&deltaVelocity, deltaT);

    // This is the velocity of the body after the current frame.
    Vector3 nextVelocity = {{0.0, 0.0, 0.0}};
    Vector3Add(&nextVelocity, &initialVelocity, &deltaVelocity);

    // Compute all 3 components of displacement:
    // - initial displacement
    // - displacement from the original velocity of the body
    // - displacement from the acceleration of the body during this frame.
    Vector3 initialDisplacement = {{0.0, 0.0, 0.0}};
    Vector3Add(&initialDisplacement, &initialDisplacement, &(currentState->bodyPositions[bodyIndex]));
    Vector3 velocityDisplacement = initialVelocity;
    Vector3Scale(&velocityDisplacement, deltaT);
    Vector3 accelerationDisplacement = acceleration;
    Vector3Scale(&accelerationDisplacement, 0.5);
    Vector3Scale(&accelerationDisplacement, deltaT*deltaT);

    // This is the position of the body after the current frame.
    Vector3 nextDisplacement = {{0.0, 0.0, 0.0}};
    Vector3Add(&nextDisplacement, &initialDisplacement, &velocityDisplacement);
    Vector3Add(&nextDisplacement, &nextDisplacement, &accelerationDisplacement);

    // Now set the properties of the body in the next simulation state.
    memcpy(&(nextState->bodyPositions[bodyIndex]), &nextDisplacement, sizeof(Vector3));
    memcpy(&(nextState->bodyVelocities[bodyIndex]), &nextVelocity, sizeof(Vector3));
}
