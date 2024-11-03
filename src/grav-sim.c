#include <stdlib.h>

#include "sim/sim.h"
#include "sim/simargs.h"

int main(int argc, char** argv) {
    GravSimArgs* args = NewGravSimArgs(argc, argv);
    if (!ValidateGravSimArgs(args)) {
        DeleteGravSimArgs(args);
        return 1;
    }

    Simulation* simulation = NewSimulation(args);
    if (simulation == NULL) {
        DeleteSimulation(simulation);
        return 2;
    }

    Simulate(simulation);

    DeleteSimulation(simulation);
    return 0;
}
