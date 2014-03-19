CPX_PATH = /nethome/rcarvajal3/ILOG/CPLEX_Studio125/cplex
#CPX_PATH = /home/stu/rcarvajal3/ILOG/CPLEX_Studio125/cplex
#GRB_PATH = /opt/gurobi/linux64/
EXEC = solver

all: 
	g++ -g -L $(CPX_PATH)/lib/x86-64_sles10_4.1/static_pic/ -L lib/ -I $(CPX_PATH)/include/ilcplex/ -I src/ -o $(EXEC) src/main.cpp src/MCFP_Inst.cpp src/MCFP_formulations.cpp src/callbacks.cpp -lm -lpthread -lcplex
