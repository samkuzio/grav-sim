#ifndef simargs_h
#define simargs_h

#include <stdbool.h>
#include <stdint.h>

#include "../math/units.h"

// Contains arguments provided to the program.
typedef struct GravSimArgs_t {
    char* inputFilePath;
    char* outputFilePath;
    int threads;

    // Length of time in seconds that should elapse between states of the simulation.
    seconds timestep;

    // Number of steps to simulate
    biguint steps;
} GravSimArgs;

// allocates a new GravSimArgs structure and initializes with the provided values.
// argc: number of strings in the provided input array
// argv: string array.
GravSimArgs* NewGravSimArgs(int argc, char** argv);

// deletes a GravSimArgs object.
void DeleteGravSimArgs(GravSimArgs* this);

// writes the args to a log.
void PrintGravSimArgs(GravSimArgs* this);

// Validate the GravSimArgs and print any error messages.
// Return true iff the grav sim args are valid for a simulation.
bool ValidateGravSimArgs(GravSimArgs* this);

#endif