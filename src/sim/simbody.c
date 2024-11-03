#include <string.h>

#include "simbody.h"
#include "../log/log.h"

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
    body->position = NewVector3FromJson(positionObject);
    if (body->position == NULL) {
        INVALID_JSON_RETURN("position could not be inflated");
    }

    // Extract velocity
    struct json_object* velocityObject;
    if (!json_object_object_get_ex(json, "velocity", &velocityObject)) {
        INVALID_JSON_RETURN("velocity is not defined or is not an object");
    }
    body->velocity = NewVector3FromJson(positionObject);
    if (body->velocity == NULL) {
        INVALID_JSON_RETURN("velocity could not be inflated");
    }

    return body;
}

// Delete a SimBody, freeing its memory
void DeleteSimBody(SimBody* this) {
    if (this != NULL) {
        free(this->name);
        DeleteVector3(this->position);
        DeleteVector3(this->velocity);
        free(this);
    }
}