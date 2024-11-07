#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <json-c/json.h>
#include <omp.h>

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
    slog("parsed simulation file with %d bodies", sim->currentState->nBodies);

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

    DeleteSimState(this->currentState);
    DeleteSimState(this->nextState);

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

    this->currentState = NewSimStateFromJson(initialStateObject);
    this->currentState->frame = 0;

    // Decrements the reference counter to json_object to free it.
    json_object_put(simulationJson);
    return true;
}

// Swaps the simstates so that the nextstate becomes the current state
// and the current state becomes the next state.
void SwapSimStates(Simulation* this) {
    SimState* holder = this->currentState;
    this->currentState = this->nextState;
    this->nextState = holder;
}

// Runs the simulation.
void Simulate(Simulation* this) {
    // Prior to the simulation, we initialize another SimState that can hold the same number of
    // bodies that were provided with the initial state. This allows us to toggle between the
    // current/next states without having to allocate entire new simstates and copy a ton of
    // information each frame.
    this->nextState = NewSimStateByDeepCopy(this->currentState);

    // write headers to output file
    FILE* output = fopen(this->inputArguments->outputFilePath, "w");
    if (output == NULL) {
        slog("Cannot write output file '%s'. Aborting.", this->inputArguments->outputFilePath);
	return;
    }
    for (biguint i = 0; i < this->currentState->nBodies; i++) {
        if (i != 0) fprintf(output, ",");
        char* name = this->currentState->bodies[i]->name;
        fprintf(output, "%s.x,%s.y,%s.z", name, name, name);
        fflush(output);
    }
    fprintf(output, "\n");
    fflush(output);

    slog("beginning simulation");

    struct timespec simStartTime, simEndTime;
    clock_gettime(CLOCK_MONOTONIC, &simStartTime);

    struct timespec frameStartTime, frameEndTime;
    do {
        clock_gettime(CLOCK_MONOTONIC, &frameStartTime);

        biguint frame = this->currentState->frame + 1;
        seconds time = this->currentState->time + this->inputArguments->timestep;
        this->nextState->frame = frame;
        this->nextState->time = time;

        // loop over every body in the current state.
        if (this->inputArguments->gpu) {
            slog("GPU is not currently supported. Quitting.");
	    return;
            //for (biguint i = 0; i < this->currentState->nBodies; i++) {
            //    AdvanceFromStateGPU(this->inputArguments->timestep, this->nextState->bodies[i], i, this->currentState, this->nextState);
            //}
        } else {
            if (this->inputArguments->parallel) {
                # pragma omp parallel for
                for (biguint i = 0; i < this->currentState->nBodies; i++) {
                    AdvanceFromStateCPU(this->inputArguments->timestep, this->nextState->bodies[i], i, this->currentState, this->nextState);
                }
            } else {
                for (biguint i = 0; i < this->currentState->nBodies; i++) {
                    AdvanceFromStateCPU(this->inputArguments->timestep, this->nextState->bodies[i], i, this->currentState, this->nextState);
                }
            }
        }

        SwapSimStates(this);

        // write position values for the computed state
        for(biguint i = 0; i < this->currentState->nBodies; i++) {
            if (i != 0) fprintf(output, ",");
            Vector3* pos = &(this->currentState->bodyPositions[i]);
            fprintf(output, "%f,%f,%f", pos->values[0], pos->values[1], pos->values[2]);
        }
        fprintf(output, "\n");
        fflush(output);

        clock_gettime(CLOCK_MONOTONIC, &frameEndTime);
        double percent = 100.0 * (double)this->currentState->frame / (double)this->inputArguments->steps;
        double elapsed = (frameEndTime.tv_sec - frameStartTime.tv_sec) + (frameEndTime.tv_nsec - frameStartTime.tv_nsec) / 1e9;
        double totalEstRem = elapsed * (this->inputArguments->steps - this->currentState->frame);
        double estHoursRem = floor(totalEstRem / 3600.0);
        totalEstRem -= (estHoursRem * 3600.0);
        double estMinutesRem = floor(totalEstRem / 60.0);
        totalEstRem -= (estMinutesRem * 60.0);
        slogc("%.3f%% | fr %d / %d | ft %.5fs | %.0f:%02.0f:%02.0f est rem",
            percent, this->currentState->frame, this->inputArguments->steps, elapsed, estHoursRem, estMinutesRem, totalEstRem);

    } while (this->currentState->frame + 1 <= this->inputArguments->steps);

    fclose(output);

    clock_gettime(CLOCK_MONOTONIC, &simEndTime);
    double elapsed = (simEndTime.tv_sec - simStartTime.tv_sec) + (simEndTime.tv_nsec - simStartTime.tv_nsec) / 1e9;
    slog("\nSimulation completed in %.2f seconds", elapsed);
}
