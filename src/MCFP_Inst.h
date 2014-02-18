/**
 * \class MCFP_Inst
 *
 * \brief Main class, containing multicommodity fix-charge flow problem instance
 *
 *         Author:  Rodolfo Carvajal, rocarvaj@gatech.edu
 *        Company:  ISyE, Georgia Tech
 *
 */

#ifndef MCFP_INST_H_
#define MCFP_INST_H_

#include "cplex.h"

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cassert>
#include <cstddef>
#include <map>

using namespace std;

///
/// \brief Directed graph arc
///
struct Arc {
    int origin;
    int destin;

    double varCost;
    double fixCost;
    double capac;

    int id;
};

///
/// \brief Commodity structure
///
struct Comm {
    int origin;
    int destin;

    double demand;
    int id;
};

class MCFP_Inst {

    public:
        MCFP_Inst();
        virtual ~MCFP_Inst();

        string instanceFileName;   ///< Name of file containing instance (with path).

        int numberNodes;   ///< Number of nodes in the graph.
        int numberArcs;   ///< Number of arcs in the graph.
        int numberComms;   ///< Number of commodities.

        vector<Arc> arcs;   ///< Arcs of graph.
        vector<Comm> comms;   ///< Commodities of graph.

        ///
        /// \brief Creates an instance of MCFP_Inst
        /// 
        /// From a .csv or .dow file
        /// type 0 is .csv and type 1 is .dow
        /// In .dow file, nodes are indexed from 1 but the code will
        /// map them starting from 0.
        ///
        MCFP_Inst(string inputFileName, int type);

        ///
        /// \brief  It prints the MFCP instance to the provided stream
        /// 
        void printToStream(ostream &outStream);

        //=====================================================================
        //CPLEX related functions (formulation)
        //=====================================================================

        ///
        /// \brief Returns index corresponding to flow variable associated to commodity \f$k\f$
        /// along arc \f$a\f$.
        ///
        /// This method is meant to be overwritten by descendant classes so methods to add flow conservation
        /// constraints or linking constraints can be used by descendant classes as well.
        ///
        int idxFlow(int arc, int k);

        ///
        /// \brief Returns the index of the design variable corresponding to arc \f$a\f$.
        ///
        /// This method is meant to be overwritten by descendant classes so methods to add
        /// linking constraints can be used by descendant classes as well.
        ///
        int idxDesign(int arc);

        ///
        /// \brief Formulates the problem with binary flows (Single path) and dissagregated linking (strong)
        ///
        int formulateSinglePathStrongIP(CPXENVptr env,
                CPXLPptr &problem);

        ///
        /// \brief Formulates the problem with relaxed (continuous) flows
        ///
        ///  This means \f$x_a^k\in [0,1]\f$
        ///
        int formulateStrongIP(CPXENVptr env,
                CPXLPptr &problem);

        ///
        /// \brief Formulates LP relaxation of problem
        ///
        int formulateStrongLP(CPXENVptr env,
                CPXLPptr &problem);
        ///
        /// \brief Formulates weak LP relaxation of problem
        ///
        int formulateWeakLP(CPXENVptr env,
                CPXLPptr &problem);
        
        ///
        /// \brief Creates the flow and design variables
        ///
        /// The flowType and designType chars define the variable type
        /// 'B': binary
        /// 'C' : continuous
        /// 
        int GLBaddVariables(CPXENVptr env,
                CPXLPptr &problem,
                char flowType,
                char designType);

        int GLBaddFlowConservation(CPXENVptr env,
                CPXLPptr &problem);


        ///
        /// \brief This method adds the flow conservation constraints associated to
        /// commodity k.
        ///
        int addFlowConservation_k(CPXENVptr env,
                CPXLPptr &problem,
                int  k);

        int addSingleNodeCutSetIneqs_i(CPXENVptr env, CPXLPptr &problem,
                int i);

        int addSingleNodeCutSetIneqs(CPXENVptr env, CPXLPptr &problem);

        ///
        /// \brief Adds aggregated liking constraints
        ///
        /// Constraints are \f$\sum_{k\in K}x^k_a \leq u_ay_a,\quad \forall a\in A\f$
        ///
        int GLBaddAggregatedLinking(CPXENVptr env,
                CPXLPptr &problem);


        ///
        /// \brief Adds disaggregated liking constraints
        ///
        /// Constraints are \f$x^k_a \leq y_a,\quad \forall a\in A, \forall k\in K\f$
        ///
        int GLBaddDisaggregatedLinking(CPXENVptr env,
                CPXLPptr &problem);

        ///
        /// \brief This methods adds the dissagregated linking constraints associated to
        /// commodity k.
        ///
        int addDisaggregatedLinking_k(CPXENVptr env,
                CPXLPptr &problem,
                int k);
};

inline int MCFP_Inst::idxFlow(int arc, int k)
{
    return arc*numberComms + k;
}

inline int MCFP_Inst::idxDesign(int arc)
{
    return numberArcs*numberComms + arc;
}

///
/// \brief Splits "s" in tokens separated by "delimiters"
///
template <typename Container> Container& split(Container& result,
        const typename Container::value_type& s,
        const typename Container::value_type& delimiters,
        int empties = 0);

#endif
