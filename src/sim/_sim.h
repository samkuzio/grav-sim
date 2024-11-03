#ifndef _sim_h
#define _sim_h

#include <stdbool.h>

#include "sim.h"

// Initializes a simulation from the siulation's input arguments.
bool ReadSimulationFile(Simulation* this);

// Parses the simulation file that was read and stored on the Simulation field.
// Return true iff the file was parsed successfully.
// ReadSimulationFile must be called with the given simulation prior to calling ParseSimulationFile.
bool ParseSimulationFile(Simulation* this);

// Swaps the simstates so that the nextstate becomes the current state
// and the current state becomes the next state.
void SwapSimStates(Simulation* this);

#endif