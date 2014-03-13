/// @file Informational callbacks (source file)

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "cplex.h"

#include <iomanip>

typedef struct Log {
    int isFeas
    double startTime;
} Log;

int infoCallback (CPXCENVptr env,
        void *cbdata,
        int wherefrom,
        void *cbhandle);

#endif
