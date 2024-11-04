#include "simstate.h"

#include <memory.h>

#include "../log/log.h"

#define INVALID_JSON_RETURN(msg) slog("failed to parse SimState json: %s", msg); DeleteSimState(state); return NULL;

// Create a blank SimState by performing a deep copy of the other.
SimState* NewSimStateByDeepCopy(SimState* other) {
    SimState* this = malloc(sizeof(SimState));

    this->time = other->time;
    this->frame = other->frame;
    this->nBodies = other->nBodies;
    this->bodies = malloc(this->nBodies * sizeof(SimBody*));
    this->bodyPositions = (Vector3*)malloc(this->nBodies * sizeof(Vector3));
    this->bodyVelocities = (Vector3*)malloc(this->nBodies * sizeof(Vector3));

    for (biguint i = 0; i < this->nBodies; i++) {
        this->bodies[i] = NewSimBodyByDeepCopy(other->bodies[i]);
    }

    memcpy(this->bodyPositions, other->bodyPositions, this->nBodies * sizeof(Vector3));
    memcpy(this->bodyVelocities, other->bodyVelocities, this->nBodies * sizeof(Vector3));

    return this;
}

// Create a SimState from json
SimState* NewSimStateFromJson(struct json_object* json) {
    SimState* state = calloc(1, sizeof(SimState));

    // Extract time
    struct json_object* timeObject;
    if(!json_object_object_get_ex(json, "time", &timeObject)) {
        INVALID_JSON_RETURN("time field is required");
    }
    if (!json_object_is_type(timeObject, json_type_string)) {
        INVALID_JSON_RETURN("time field is not a string");
    }
    const char* timeString = json_object_get_string(timeObject);
    char* endptr;
    real time = strtold(timeString, &endptr);
    if (*endptr != '\0') {
        INVALID_JSON_RETURN("time string could not be converted to numeric type");
    }
    state->time = time;

    // Extract sim bodies
    struct json_object* bodiesArray;
    if(!json_object_object_get_ex(json, "bodies", &bodiesArray)) {
        INVALID_JSON_RETURN("bodies field is required");
    }
    if (!json_object_is_type(bodiesArray, json_type_array)) {
        INVALID_JSON_RETURN("bodies field must be an array");
    }
    state->nBodies = json_object_array_length(bodiesArray);
    state->bodies = calloc(state->nBodies, sizeof(SimBody*));
    
    for (biguint i = 0; i < state->nBodies; i++) {
        struct json_object* bodyObject = json_object_array_get_idx(bodiesArray, i);
        if (bodyObject == NULL) {
            INVALID_JSON_RETURN("could not get sim body object");
        }
        SimBody* body = NewSimBodyFromJson(bodyObject);
        if (body == NULL) {
            INVALID_JSON_RETURN("invalid sim body object");
        }
        state->bodies[i] = body;
    }

    //build continuguous arrays of body positions and velocities.
    state->bodyPositions = (Vector3*)malloc(state->nBodies * sizeof(Vector3));
    state->bodyVelocities = (Vector3*)malloc(state->nBodies * sizeof(Vector3));

    for (biguint i = 0; i < state->nBodies; i++) {
        memcpy(&state->bodyPositions[i], state->bodies[i]->initialPosition, sizeof(Vector3));
        memcpy(&state->bodyVelocities[i], state->bodies[i]->initialVelocity, sizeof(Vector3));
    }

    return state;
}

// Delete a simstate, freeing it and all the memory it owns.
void DeleteSimState(SimState* this) {
    if (this != NULL) {
        for (biguint i = 0; i < this->nBodies; i++) {
            SimBody* body = this->bodies[i];
            if (body != NULL) {
                DeleteSimBody(body);
            }
        }
        if (this->bodies != NULL)  free(this->bodies);
        if (this->bodyPositions != NULL) free(this->bodyPositions);
        if (this->bodyVelocities != NULL) free(this->bodyVelocities);

        free(this);
    }
}