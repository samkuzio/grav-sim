#ifndef sim_h
#define sim_h

#include "simargs.h"
#include "simstate.h"

// Represent a wholesimulation
typedef struct Simulation_t {
    GravSimArgs* inputArguments;
    char* inputFile;

    char* name;
    SimState* initialState;
} Simulation;

// Create a new simulation from the input arguments.
Simulation* NewSimulation(GravSimArgs* args);

// Free the simulation and all the resources that it owns.
void DeleteSimulation(Simulation* this);

#endif