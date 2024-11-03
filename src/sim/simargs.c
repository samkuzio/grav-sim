#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simargs.h"
#include "../log/log.h"
#include "../list/list.h"

GravSimArgs* NewGravSimArgs(int argc, char** argv) {
    GravSimArgs* args = calloc(1, sizeof(GravSimArgs));

    // Parse flagged values.
    for (int argi = 1; argi < argc; argi++) {
        // check if the arg is positional. We skip those.
        char* arg = argv[argi];
        if (arg[0] != '-') {
            slog("skipping positional argument %s", arg);
            continue;
        }

        // reference the flag and value input arguments.
        char* flag = argv[argi];
        char* value = (argi + 1) < argc ? argv[argi+1] : NULL;

        // assign named input args into the struct.
        if (value != NULL) {
            if (strcmp("-input", flag) == 0) {
                args->inputFilePath = calloc(strlen(value)+1, sizeof(char));
                strcpy(args->inputFilePath, value);
            } else if (strcmp("-output", flag) == 0) {
                args->outputFilePath = calloc(strlen(value)+1, sizeof(char));
                strcpy(args->outputFilePath, value);
            } else if (strcmp("-threads", flag) == 0) {
                char* endptr;
                long numThreads = strtol(value, &endptr, 10);
                if (*endptr != '\0' || numThreads < 0) {
                    numThreads = -1;
                }
                args->threads = numThreads;
            } else if (strcmp("-timestep", flag) == 0) {
                char* endptr;
                seconds timestep = strtold(value, &endptr);
                if (*endptr != '\0' || timestep < 0) {
                    timestep = -1;
                }
                args->timestep = timestep;
            } else if (strcmp("-steps", flag) == 0) {
                char *endptr;
                long steps = strtol(value, &endptr, 10);
                if (*endptr != '\0')  {
                    steps = -1;
                }
                args->steps = steps;
            }
        }

        argi++;
    }

    return args;
}

void DeleteGravSimArgs(GravSimArgs* this) {
    if (this != NULL) {
        free(this->inputFilePath);
        free(this->outputFilePath);
        free(this);
    }
}

void PrintGravSimArgs(GravSimArgs* this) {
    slog("\tinput file  = %s", this->inputFilePath);
    slog("\toutput file = %s", this->outputFilePath);
    slog("\tthreads     = %d", this->threads);
    slog("\ttimestep    = %.3Lf", this->timestep);
    slog("\tsteps       = %ld", this->steps);
}

void logListItems(void* value) {
    slog((char*) value);
}

bool ValidateGravSimArgs(GravSimArgs* this) {
    LinkedList* errorList = NewLinkedList();

    if (this->inputFilePath == NULL) {
        char* errorMessage = strdup("error: input file is required");
        Append(errorList, errorMessage);
    }

    if (this->outputFilePath == NULL) {
        char* errorMessage = strdup("error: output file is required");
        Append(errorList, errorMessage);
    }

    if (this->threads < 0) {
        char* errorMessage = strdup("error: threads should be zero or more");
        Append(errorList, errorMessage);
    }

    if (this->timestep <= 0) {
        char* errorMessage = strdup("error: timestep must be a positive number");
        Append(errorList, errorMessage);
    }

    if (this->steps <= 0) {
        char* errorMessage = strdup("error: steps must be a positive number");
        Append(errorList, errorMessage);
    }

    Iterate(errorList, &logListItems);

    bool valid = errorList->length == 0;
    DeleteLinkedList(errorList);

    return valid;
}