#include "cplex.h"

//extern "C"
//{
//#include "gurobi_c.h"
//}

#include "MCFP_Inst.h"
#include "callbacks.h"

#include <cstdlib>
#include <vector>
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

void usage()
{
    cout << "./solver fileName fielType formulation timeLimit solver threads\n" << endl;
    cout << "fileType: 0 if csv, 1 if dow" << endl;
    cout << "formulation: 0 IP, 1 Rlx Flow, 2 Strong LP, 3 Weak LP" << endl;
    cout << "solver: 'c' CPLEX, 'g' Gurobi"<< endl;
}

int main(int argc, char** argv)
{

    if(argc < 6)
    {
        usage();
        return 0;
    }

    int status;
    string fileName = argv[1];
    int type = atoi(argv[2]);

    // 0: IP
    // 1: Rlx Flow
    // 2: Strong LP
    // 3: Weak LP
    int formulation = atoi(argv[3]);
    double timeLimit = atof(argv[4]);

    string tmpSolver = argv[5];
    char solver = tmpSolver[0];

    int threads = atoi(argv[6]);

    double bestSolValue = 1.0E+75;
    double bestBound = -1.0E+75;

    double startTime;
    double finalTime;

    cout << "Hello world!!!" << endl;
    cout << "Solving: " << fileName << endl;
    cout << "Formulation: " << formulation << endl;
    cout << "Time limit: " << timeLimit << endl;


    Log log;
    log.isFeas = 0;

    MCFP_Inst inst(fileName, type);

    CPXENVptr env = CPXopenCPLEX(&status);
    CPXLPptr problem = CPXcreateprob(env, &status, fileName.c_str());
    assert(!status);

    status = CPXsetdblparam(env, CPX_PARAM_TILIM, timeLimit);
    assert(!status);

    if(formulation == 2 || formulation == 3)
    {
        status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);
        assert(!status);
    }

    status = CPXsetintparam(env, CPX_PARAM_THREADS, threads);
    assert(!status);

    status = CPXsetinfocallbackfunc(env, infoCallback, &log);
    assert(!status);

    if(formulation == 0)
    {
        inst.formulateSinglePathStrongIP(env, problem);
    }
    else if(formulation == 1)
    {
        inst.formulateStrongIP(env, problem);
    }
    else if(formulation == 2)
    {
        inst.formulateStrongLP(env, problem);
        //status = CPXwriteprob(env, problem, "strongLp.lp", NULL);
    }
    else if(formulation == 3)
        inst.formulateWeakLP(env, problem);

    if(solver == 'c')
    {

        status = CPXgettime(env, &startTime);

        log.startTime = startTime;

        status = CPXmipopt(env, problem);
        assert(!status);

        status = CPXgettime(env, &finalTime);

        int optStatus = CPXgetstat(env, problem);

        cout << "Status: (" << optStatus << ") ";
        if(optStatus == CPXMIP_OPTIMAL || optStatus == CPXMIP_OPTIMAL_TOL)
            cout << "Optimal" << endl;
        else if(optStatus == CPXMIP_TIME_LIM_FEAS)
            cout << "Time limit, feasible" << endl;
        else if(optStatus == CPXMIP_TIME_LIM_INFEAS)
            cout << "Time limit, infeasible" << endl;
        else
            cout << "See optim.cplex.solutionstatus in CPLEX manual\n" << endl;
 
        cout << "Total time: " << finalTime - startTime << endl;
        
        cout << fixed << setprecision(2) << endl;
        status = CPXgetbestobjval(env, problem, &bestBound);
        assert(!status);
        cout << "*** Best bound: " << bestBound << endl;

        if(optStatus == CPXMIP_OPTIMAL || optStatus == CPXMIP_OPTIMAL_TOL || optStatus == CPXMIP_TIME_LIM_FEAS)
        {
            status = CPXgetobjval(env, problem, &bestSolValue);
            assert(!status);

            cout << "*** Objective value: " << bestSolValue << endl;
        }
    }
    else if(solver == 'g')
    {
        int optStatus;
        int solsFound;

#if 0
        stringstream ss;
        ss << fileName << ".lp";
        status = CPXwriteprob(env, problem, ss.str().c_str(), NULL);
        assert(!status);

        GRBenv *grb_env;
        GRBmodel *model;
        
        status = GRBloadenv(&grb_env, NULL);
        assert(!status);

        status = GRBreadmodel(grb_env, ss.str().c_str(), &model);
        assert(!status);

        status = GRBsetdblparam(grb_env, GRB_DBL_PAR_TIMELIMIT, timeLimit);
        assert(!status);

        status = CPXgettime(env, &startTime);
        assert(!status);
        
        status = GRBoptimize(model);
        assert(!status);

        status = CPXgettime(env, &finalTime);
        assert(!status);
         
        cout << "Total time: " << finalTime - startTime << endl;

        status = GRBgetintattr(model, GRB_INT_ATTR_SOLCOUNT, &solsFound);
        assert(!status);
        status = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optStatus);
        assert(!status);

        cout << "Status: (" << optStatus << ") ";

        if(optStatus == GRB_OPTIMAL)
            cout << "Optimal" << endl;
        else if(optStatus == GRB_TIME_LIMIT && solsFound > 0)
            cout << "Time limit, feasible" << endl;
        else if(optStatus == GRB_TIME_LIMIT && solsFound == 0)
            cout << "Time limit, infeasibel" << endl;
        else
            cout << "See http://www.gurobi.com/documentation/5.6/reference-manual/optimization_status_codes#sec:StatusCodes" << endl;


        status = GRBgetdblattr(model, GRB_DBL_ATTR_OBJBOUND, &bestBound);
        assert(!status);

        cout << "*** Best bound: " << bestBound << endl;

        if(solsFound > 0)
        {
            status = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &bestSolValue);
            assert(!status);

            cout << "*** Objective value: " << bestSolValue;
         }

        status = GRBfreemodel(model);
        assert(!status);

        GRBfreeenv(grb_env);
#endif
    }

    status = CPXfreeprob(env, &problem);
    assert(!status);

    status = CPXcloseCPLEX(&env);
    assert(!status);

    return 0;
}
