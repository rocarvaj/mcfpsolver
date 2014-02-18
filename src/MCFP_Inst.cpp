/*
 * =====================================================================================
 *
 *       Filename:  MCFP_Inst.cpp
 *
 *    Description:  Source file for MCFP_InstRepresentation of multicommodity flow data problem instance
 *    
*         Author:  Rodolfo Carvajal, rocarvaj@gatech.edu
 *        Company:  ISyE, Georgia Tech
 *
 * =====================================================================================
 */

#include "MCFP_Inst.h"


MCFP_Inst::MCFP_Inst()
{

}

MCFP_Inst::~MCFP_Inst()
{

}

MCFP_Inst::MCFP_Inst(string inputFileName, int type)
{
    instanceFileName = inputFileName;
    ifstream inFile(inputFileName.c_str());

    if(!inFile.is_open())
    { 
        cout << "Error opening file: " << instanceFileName.c_str() << endl;
        exit(1);
    }
    string line;
    vector<string> tokens;

    if(type == 0)
    {
        // First line ignored
        getline(inFile, line);

        getline(inFile, line);
        split(tokens, line, ",");

        numberNodes = atoi(tokens[1].c_str());

        for(int i = 0; i < numberNodes; ++i)
            getline(inFile, line);

        getline(inFile, line);
        tokens.clear();
        split(tokens, line, ",");

        numberArcs = atoi(tokens[1].c_str());

        for(int arc = 0; arc < numberArcs; ++arc)
        {
            Arc tmpArc;
            tokens.clear();

            getline(inFile, line);
            split(tokens, line, ",");

            tmpArc.origin = atoi(tokens[0].c_str());
            tmpArc.destin = atoi(tokens[1].c_str());
            tmpArc.varCost = atof(tokens[2].c_str());
            tmpArc.capac = atof(tokens[4].c_str());
            tmpArc.fixCost = atof(tokens[3].c_str());

            tmpArc.id = arc;

            arcs.push_back(tmpArc);
        }

        getline(inFile, line);
        tokens.clear();
        split(tokens, line, ",");

        numberComms = atoi(tokens[1].c_str());

        for(int comm = 0; comm < numberComms; ++comm)
        {
            Comm tmpComm;
            tokens.clear();

            getline(inFile, line);
            split(tokens, line, ",");

            tmpComm.origin = std::atoi(tokens[0].c_str());
            tmpComm.destin = atoi(tokens[1].c_str());
            tmpComm.demand = atof(tokens[2].c_str());

            tmpComm.id = comm;

            comms.push_back(tmpComm);
        }
    }
    else if(type == 1)
    {
        // First line not useful
        getline(inFile, line);

        getline(inFile, line);
        split(tokens, line, " ");

        numberNodes = atoi(tokens[0].c_str());
        numberArcs = atoi(tokens[1].c_str());
        numberComms = atoi(tokens[2].c_str());

        for(int arc = 0; arc < numberArcs; ++arc)
        {
            Arc tmpArc;
            tokens.clear();

            getline(inFile, line);
            split(tokens, line, " ");

            tmpArc.origin = atoi(tokens[0].c_str())-1;
            tmpArc.destin = atoi(tokens[1].c_str())-1;
            tmpArc.varCost = atof(tokens[2].c_str());
            tmpArc.capac = atof(tokens[3].c_str());
            tmpArc.fixCost = atof(tokens[4].c_str());

            tmpArc.id = arc;

            arcs.push_back(tmpArc);
        }

        for(int comm = 0; comm < numberComms; ++comm)
        {
            Comm tmpComm;
            tokens.clear();

            getline(inFile, line);
            split(tokens, line, " ");

            tmpComm.origin = atoi(tokens[0].c_str())-1;
            tmpComm.destin = atoi(tokens[1].c_str())-1;
            tmpComm.demand = atof(tokens[2].c_str());

            tmpComm.id = comm;

            comms.push_back(tmpComm);
        }
    }
    else
    {
        cout << "Wrong file type." << endl;
        exit(1);
    }

    inFile.close();

}

void MCFP_Inst::printToStream(ostream &outStream)
{
    outStream << "numberArcs: " << numberArcs << endl;
    outStream << "numberNodes: " << numberNodes << endl;
    outStream << "numberComms: " << numberComms << endl;

    outStream << "Arcs:" << endl;
    for(int i = 0; i < numberArcs; ++i)
    { 
        outStream << "(" << arcs[i].origin << ", " << arcs[i].destin << ") / "
            << arcs[i].varCost << ", " << arcs[i].capac << ", " << arcs[i].fixCost << endl;
    }

    outStream << "Comms:" << endl;
    for(int i = 0; i < numberComms; ++i)
    { 
        outStream << "(" << comms[i].origin << ", " << comms[i].destin << ") / "
            << comms[i].demand << endl;
    }

}

template <typename Container>
Container& split(
        Container&                            result,
        const typename Container::value_type& s,
        const typename Container::value_type& delimiters,
        int  empties = 0)
{
    result.clear();
    size_t current;
    size_t next = -1;
    do
    {
        if (empties == 0)
        {
            next = s.find_first_not_of( delimiters, next + 1 );
            if (next == Container::value_type::npos) break;
            next -= 1;
        }
        current = next + 1;
        next = s.find_first_of( delimiters, current );
        result.push_back( s.substr( current, next - current ) );
    }
    while (next != Container::value_type::npos);
    return result;
}
