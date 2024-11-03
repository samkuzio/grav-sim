#include "simstate.h"

#include <memory.h>

#include "../log/log.h"

#define INVALID_JSON_RETURN(msg) slog("failed to parse SimState json: %s", msg); DeleteSimState(state); return NULL;

// Create a blank SimState.
SimState* NewSimState(biguint frame, seconds time, biguint nbodies) {
    SimState* state = calloc(1, sizeof(SimState));
    state->frame = frame;
    state->time = time;
    state->nBodies = nbodies;
    state->bodies = calloc(nbodies, sizeof(SimBody*));
    for (biguint i = 0; i < state->nBodies; i++) {
        state->bodies[i] = NewSimBody();
    }
    return state;
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
        free(this->bodies);
        free(this);
    }
}