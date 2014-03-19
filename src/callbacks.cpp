#include "callbacks.h"

int infoCallback (CPXCENVptr env,
        void *cbdata,
        int wherefrom,
        void *cbhandle)
{
    int status;
    double bestLowerBound;
    double bestUpperBound;
    int nodes;

    Log *tmplog = (Log*) cbhandle;
    Log &log = *tmplog;

    double curTime;
    status = CPXgettime(env, &curTime);
    assert(!status);

    status = CPXgetcallbackinfo(env, 
            cbdata, 
            wherefrom, 
            CPX_CALLBACK_INFO_BEST_REMAINING, 
            &bestLowerBound);
    assert(!status);

    status = CPXgetcallbackinfo(env,
            cbdata,
            wherefrom,
            CPX_CALLBACK_INFO_NODE_COUNT,
            &nodes);
    assert(!status);
    
    if(!log.isFeas)
    {
        status = CPXgetcallbackinfo(env,
                cbdata, 
                wherefrom, 
                CPX_CALLBACK_INFO_MIP_FEAS, 
                &log.isFeas);
        assert(!status);
    }

    if(log.isFeas)
    {
        status = CPXgetcallbackinfo(env, 
                cbdata, 
                wherefrom, 
                CPX_CALLBACK_INFO_BEST_INTEGER, 
                &bestUpperBound);
        assert(!status);
    }

    double time = curTime - log.startTime;
    cout << setw(9) << right << time << "\t";
    cout << setw(9) << right << nodes << "\t";

        if(bestLowerBound != -1e+75)
            cout << setw(12) << right << bestLowerBound << "\t";
        else
            cout << setw(12) << right << "--" << "\t";

    if(log.isFeas)
        cout << setw(12) << right << bestUpperBound << "\t";
    else
        cout << setw(12) << right << "--" << "\t";

    if(log.isFeas)
    {
        double gap = ((bestUpperBound - bestLowerBound)/(fabs(bestUpperBound) + 1e-10))*100;
        cout << setw(6) << right << gap << "%\t";
    }
    else
        cout << setw(6) << right << "--" << "%\t";

    cout << endl;

    return 0;
}
