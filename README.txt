# A C++ class for Multicommodity flow problems

[Rodolfo Carvajal](http://rocarvaj.uai.cl), 2014

## What?
This repo consists of:
* A C++ class (`MCFP_Inst`) which loads a multicommodity flow problem in cvs or dow format (see <https://bitbucket.org/rocarvaj/mcfplib>
a repository with instances and information about the formats).

* A few methods within the class to solve different formulations of the Multicommodity Fixed-charge flow problem
with CPLEX or Gurobi.

## Requirements
* IBM ILOG CPLEX (probably 12.5+, although any recent version should work).
* Gurobi (optional, the code to solve with Gurobi is commented right now, but you can easily activate it).

## How to use
* Modify `makefile` to suit your needs.
* `make`.
* Run the `solver` executable with no arguments for help.
