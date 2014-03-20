#include "MCFP_Inst.h"

int MCFP_Inst::formulateSinglePathStrongIP(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    status = GLBaddVariables(env, problem, 'B', 'B');
    if(status)
    {
        cout << "Error in GLBaddVariables" << endl;
        return 1;
    }
    status = GLBaddFlowConservation(env, problem);
    assert(!status);

    status = GLBaddAggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddAggregatedLinking" << endl;
        return 1;
    }

    status = GLBaddDisaggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddDisaggregatedLinking" << endl;
        return 1;
    }  

    return 0;
}

int MCFP_Inst::formulateStrongIP(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    status = GLBaddVariables(env, problem, 'C', 'B');
    if(status)
    {
        cout << "Error in GLBaddVariables" << endl;
        return 1;
    }
    status = GLBaddFlowConservation(env, problem);
    if(status)
    {
        cout << "Error in GLBaddFlowConservation" << endl;
        return 1;
    }

    status = GLBaddAggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddAggregatedLinking" << endl;
        return 1;
    }

    status = GLBaddDisaggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddDisaggregatedLinking" << endl;
        return 1;
    }

    status = addSingleNodeCutSetIneqs(env, problem);
    assert(!status);

    return 0;
}

int MCFP_Inst::formulateStrongLP(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    status = GLBaddVariables(env, problem, 'C', 'C');
    if(status)
    {
        cout << "Error in GLBaddVariables" << endl;
        return 1;
    }

    status = CPXchgprobtype(env, problem, CPXPROB_LP);
    assert(!status);

    status = GLBaddFlowConservation(env, problem);
    if(status)
    {
        cout << "Error in GLBaddFlowConservation" << endl;
        return 1;
    }

    status = GLBaddAggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddAggregatedLinking" << endl;
        return 1;
    }

    status = GLBaddDisaggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddDisaggregatedLinking" << endl;
        return 1;
    }  

    status = addSingleNodeCutSetIneqs(env, problem);
    assert(!status);

    return 0;
}

int MCFP_Inst::formulateWeakLP(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    status = GLBaddVariables(env, problem, 'C', 'C');
    if(status)
    {
        cout << "Error in GLBaddVariables" << endl;
        return 1;
    }

    status = CPXchgprobtype(env, problem, CPXPROB_LP);
    assert(!status);

    status = GLBaddFlowConservation(env, problem);
    if(status)
    {
        cout << "Error in GLBaddFlowConservation" << endl;
        return 1;
    }

    status = GLBaddAggregatedLinking(env, problem);
    if(status)
    {
        cout << "Error in GLBaddAggregatedLinking" << endl;
        return 1;
    }

    return 0;
}


//=============================================================================
// Variables
//=============================================================================


int MCFP_Inst::GLBaddVariables(CPXENVptr env,
        CPXLPptr &problem,
        char flowType,
        char designType)
{
    int status;

    stringstream ss;
    vector<char*> names;
    char dummy[256];

    vector<double> obj;
    vector<double> lb;
    vector<double> ub;
    vector<char> xtype;

    // Flow variables
    for(int a = 0; a < numberArcs; ++a)
    {
        for(int comm = 0; comm < numberComms; ++comm)
        {
            obj.push_back(comms[comm].demand * arcs[a].varCost);
            lb.push_back(0.0);
            ub.push_back(1.0);

            // Single path formulation
            xtype.push_back(flowType);

            sprintf(dummy, "x[%d,%d,%d]", arcs[a].origin, arcs[a].destin, comm);

            names.push_back(strdup(dummy));
        }

    }

    // Design variables
    for(int a = 0; a < numberArcs; ++a)
    {
        obj.push_back(arcs[a].fixCost);
        lb.push_back(0.0);
        ub.push_back(1.0);

        xtype.push_back(designType);

        sprintf(dummy, "y[%d,%d]", arcs[a].origin, arcs[a].destin);

        names.push_back(strdup(dummy));
    }

    status = CPXnewcols(env,
            problem,
            numberArcs*numberComms + numberArcs,
            &(obj[0]),
            &(lb[0]),
            &(ub[0]),
            &(xtype[0]),
            &(names[0]));
    if(status)
    {
        cout << "Error adding columns" << endl;
        return 1;
    }

    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);


    return 0;


}

//================================================================================
// Constraints
//================================================================================

int MCFP_Inst::GLBaddFlowConservation(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    int rcnt = numberNodes*numberComms;
    int nzcnt = 0;

    vector<char*> names;
    char dummy[256];

    vector<double> rhs;
    vector<char> sense;
    vector<int> rmatbeg;
    vector<int> rmatind;
    vector<double> rmatval;

    for(int i = 0; i < numberNodes; ++i)
    {
        for(int comm = 0; comm < numberComms; ++comm)
        {
            rmatbeg.push_back(nzcnt);
            sense.push_back('E');
            sprintf(dummy, "Flow[%d,%d]", i, comm);

            names.push_back(strdup(dummy));

            for(int a = 0; a < numberArcs; ++a)
            {
                if(arcs[a].origin == i)
                {
                    rmatval.push_back(1.0);
                    rmatind.push_back(numberComms*a + comm);
                    ++nzcnt;
                }
                else if(arcs[a].destin == i)
                {
                    rmatval.push_back(-1.0);
                    rmatind.push_back(numberComms*a + comm);
                    ++nzcnt;
                }
            }

            if(comms[comm].origin == i)
                rhs.push_back(1.0);
            else if (comms[comm].destin == i)
                rhs.push_back(-1.0);
            else
                rhs.push_back(0.0);
        }
    }

    status = CPXaddrows(env,
            problem,
            0,
            rcnt,
            nzcnt,
            &(rhs[0]),
            &(sense[0]),
            &(rmatbeg[0]),
            &(rmatind[0]),
            &(rmatval[0]),
            NULL,
            &(names[0]));
    if(status)
    {
        cout << "Error adding flow conservation constraints (" << status << ")" << endl;
        return 1;
    }

    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);


    return 0;   
}

int MCFP_Inst::addFlowConservation_k(CPXENVptr env,
        CPXLPptr &problem,
        int k)
{
    int status;

    int rcnt = numberNodes;
    int nzcnt = 0;

    vector<char*> names;
    char dummy[256];

    vector<double> rhs;
    vector<char> sense;
    vector<int> rmatbeg;
    vector<int> rmatind;
    vector<double> rmatval;

    for(int i = 0; i < numberNodes; ++i)
    {
        rmatbeg.push_back(nzcnt);
        sense.push_back('E');
        sprintf(dummy, "Flow[%d,%d]", i, k);

        names.push_back(strdup(dummy));

        for(int a = 0; a < numberArcs; ++a)
        {
            if(arcs[a].origin == i)
            {
                rmatval.push_back(1.0);
                rmatind.push_back(idxFlow(a, k));
                ++nzcnt;
            }
            else if(arcs[a].destin == i)
            {
                rmatval.push_back(-1.0);
                rmatind.push_back(idxFlow(a, k));
                ++nzcnt;
            }
        }

        if(comms[k].origin == i)
            rhs.push_back(1.0);
        else if (comms[k].destin == i)
            rhs.push_back(-1.0);
        else
            rhs.push_back(0.0);
    }

    status = CPXaddrows(env,
            problem,
            0,
            rcnt,
            nzcnt,
            &(rhs[0]),
            &(sense[0]),
            &(rmatbeg[0]),
            &(rmatind[0]),
            &(rmatval[0]),
            NULL,
            &(names[0]));
    assert(!status);
    
    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);

    return 0;   


}

int MCFP_Inst::addSingleNodeCutSetIneqs_i(CPXENVptr env, CPXLPptr &problem,
        int i)
{
    int status;

    int addOriginIneq = 0;
    int addDestinIneq = 0;

    int rmatbeg = 0;
    
    double originRhs = 0.0;
    double destinRhs = 0.0;

    vector<double> originVal;
    vector<double> destinVal;

    vector<int> originIdx;
    vector<int> destinIdx;

    vector<char> originSense;
    vector<char> destinSense;

    for(int c = 0; c < numberComms; ++c)
    {
        if(comms[c].origin == i)
        {
            addOriginIneq = 1;
            originRhs += comms[c].demand;
        }
        else if(comms[c].destin == i)
        {
            addDestinIneq = 1;
            destinRhs += comms[c].demand;
        }
    }

    if(!addOriginIneq && !addDestinIneq)
        return 0;

    for(int a = 0; a <numberArcs; ++a)
    {
        if(arcs[a].origin == i && addOriginIneq)
        {
           originIdx.push_back(idxDesign(a));
           originVal.push_back(arcs[a].capac);
           originSense.push_back('G');
        }
        else if(arcs[a].destin == i && addDestinIneq)
        {
           destinIdx.push_back(idxDesign(a));
           destinVal.push_back(arcs[a].capac);
           destinSense.push_back('G');
        }
    }

    if(addOriginIneq)
    {
        status = CPXaddrows(env, problem, 0, 1, originVal.size(),
                &originRhs,
                &(originSense[0]),
                &rmatbeg,
                &(originIdx[0]),
                &(originVal[0]),
                NULL,
                NULL);
        assert(!status);
    }

    if(addDestinIneq)
    {
        status = CPXaddrows(env, problem, 0, 1, destinVal.size(),
                &destinRhs,
                &(destinSense[0]),
                &rmatbeg,
                &(destinIdx[0]),
                &(destinVal[0]),
                NULL,
                NULL);
        assert(!status);
    }

    return 0;
}

int MCFP_Inst::addSingleNodeCutSetIneqs(CPXENVptr env, CPXLPptr &problem)
{
    int status;

    for(int i = 0; i < numberNodes; ++i)
    {
        status = addSingleNodeCutSetIneqs_i(env, problem, i);
        assert(!status);
    }

    return  0;
}


int MCFP_Inst::GLBaddAggregatedLinking(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;
    char dummy[256];

    int rcnt = numberArcs;
    int nzcnt = 0;
    vector<int> rmatbeg;
    vector<int> rmatind;
    vector<double> rmatval;
    vector<double> rhs(rcnt, 0.0);
    vector<char> sense(rcnt, 'L');
    vector<char*> names;

    for(int a = 0; a < numberArcs; ++a)
    {
        rmatbeg.push_back(nzcnt);

        for(int c = 0; c < numberComms; ++c)
        {
            rmatind.push_back(numberComms * a + c);
            rmatval.push_back(comms[c].demand);
            ++nzcnt;
        }

        rmatind.push_back(numberComms * numberArcs + a);
        rmatval.push_back(-arcs[a].capac);
        ++nzcnt;

        sprintf(dummy, "Capac[%d,%d]", arcs[a].origin, arcs[a].destin);
        names.push_back(strdup(dummy));
    }

    status = CPXaddrows(env,
            problem,
            0,
            rcnt,
            nzcnt,
            &(rhs[0]),
            &(sense[0]),
            &(rmatbeg[0]),
            &(rmatind[0]),
            &(rmatval[0]),
            NULL,
            &(names[0]));
    if(status)
    {
        cout << "Error adding aggregated linking constraints" << endl;
        return 1;
    }

    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);


    return 0;

}

int MCFP_Inst::GLBaddDisaggregatedLinking(CPXENVptr env,
        CPXLPptr &problem)
{
    int status;

    char dummy[256];

    int rcnt = numberArcs * numberComms;
    int nzcnt = 0;
    vector<int> rmatbeg;
    vector<int> rmatind;
    vector<double> rmatval;
    vector<double> rhs(rcnt, 0.0);
    vector<char> sense(rcnt, 'L');
    vector<char*> names;

    for(int a = 0; a < numberArcs; ++a)
    {
        for(int c = 0; c < numberComms; ++c)
        {
            rmatbeg.push_back(nzcnt);

            rmatind.push_back(numberComms * a + c);
            rmatval.push_back(1.0);
            ++nzcnt;

            rmatind.push_back(numberArcs*numberComms + a);
            rmatval.push_back(-1.0);
            ++nzcnt;

            sprintf(dummy, "Link[%d,%d,%d]", arcs[a].origin, arcs[a].destin, c);
            names.push_back(strdup(dummy));
        }
    }


    status = CPXaddrows(env,
            problem,
            0,
            rcnt,
            nzcnt,
            &(rhs[0]),
            &(sense[0]),
            &(rmatbeg[0]),
            &(rmatind[0]),
            &(rmatval[0]),
            NULL,
            &(names[0]));
    if(status)
    {
        cout << "Error adding linking constraints" << endl;
        return 1;
    }

    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);


    return 0;
}

int MCFP_Inst::addDisaggregatedLinking_k(CPXENVptr env,
        CPXLPptr &problem,
        int k)
{
    int status;

    char dummy[256];

    int rcnt = numberArcs;
    int nzcnt = 0;
    vector<int> rmatbeg;
    vector<int> rmatind;
    vector<double> rmatval;
    vector<double> rhs(rcnt, 0.0);
    vector<char> sense(rcnt, 'L');
    vector<char*> names;

    for(int a = 0; a < numberArcs; ++a)
    {
        rmatbeg.push_back(nzcnt);

        rmatind.push_back(idxFlow(a, k));
        rmatval.push_back(1.0);
        ++nzcnt;

        rmatind.push_back(idxDesign(a));
        rmatval.push_back(-1.0);
        ++nzcnt;

        sprintf(dummy, "Link[%d,%d]", a, k);
        names.push_back(strdup(dummy));
    }


    status = CPXaddrows(env,
            problem,
            0,
            rcnt,
            nzcnt,
            &(rhs[0]),
            &(sense[0]),
            &(rmatbeg[0]),
            &(rmatind[0]),
            &(rmatval[0]),
            NULL,
            &(names[0]));
    assert(!status);
    
    
    for(int i = 0; i < (int) names.size(); ++i)
        free(names[i]);


    return 0;



}
