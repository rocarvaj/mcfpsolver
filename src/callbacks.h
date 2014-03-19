/// @file Informational callbacks (source file)

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "cplex.h"

#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <cassert>

using namespace std;

typedef struct Log {
    int isFeas;
    double startTime;
} Log;

int infoCallback (CPXCENVptr env,
        void *cbdata,
        int wherefrom,
        void *cbhandle);

#endif
