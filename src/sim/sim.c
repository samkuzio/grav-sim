#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <json-c/json.h>

#include "sim.h"
#include "simargs.h"
#include "_sim.h"
#include "../log/log.h"

// Create a new simulation from the input arguments.
Simulation* NewSimulation(GravSimArgs* args) {
    Simulation* sim = calloc(1, sizeof(Simulation));
    sim->inputArguments = args;

    slog("initializing simulation");
    PrintGravSimArgs(sim->inputArguments);

    if (!ReadSimulationFile(sim)) {
        DeleteSimulation(sim);
        return NULL;
    }

    ParseSimulationFile(sim);
    slog("parsed simulation file with %d bodies", sim->initialState->nBodies);

    return sim;
}

// Free the simulation and all the resources that it owns.
void DeleteSimulation(Simulation* this) {
    if (this == NULL) {
        return;
    }

    DeleteGravSimArgs(this->inputArguments);

    if (this->inputFile != NULL) {
        free(this->inputFile);
    }

    if (this->name != NULL) {
        free(this->name);
    }

    DeleteSimState(this->initialState);

    free(this);
}

// Initializes a simulation from the siulation's input arguments.
bool ReadSimulationFile(Simulation* this) {
    char* filepath = this->inputArguments->inputFilePath;
    if (access(filepath, F_OK) != 0) {
        slog("error: input file does not exist or it is inaccessible: %s", filepath);
        return false;
    }

    FILE* scenarioFile = fopen(filepath, "r");
    fseek(scenarioFile, 0, SEEK_END);
    long fileSize = ftell(scenarioFile);
    char* buffer = malloc(fileSize + 1);
    fseek(scenarioFile, 0, SEEK_SET);
    fread(buffer, fileSize, 1, scenarioFile);
    fclose(scenarioFile);
    buffer[fileSize] = 0;

    this->inputFile = buffer;
    return true;
}

bool ParseSimulationFile(Simulation* this) {
    struct json_object* simulationJson;
    simulationJson = json_tokener_parse(this->inputFile);

    // copy name into simulation.
    this->name = strdup(json_object_get_string(simulationJson));

    struct json_object* initialStateObject;
    json_object_object_get_ex(simulationJson, "initialState", &initialStateObject);

    this->initialState = NewSimStateFromJson(initialStateObject);

    // Decrements the reference counter to json_object to free it.
    json_object_put(simulationJson);
    return true;
}