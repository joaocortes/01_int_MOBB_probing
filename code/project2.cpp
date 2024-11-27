// project2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <vector>
#include <filesystem>
#include "BranchAndBound.h"
#include <ilcplex/ilocplex.h>
#include <ctime>
//#include "Epsilon.h"
//#include "LowerBoundSet.h"
//#include "LinearProgram.h"
//#include "LB.h"
//#include "Model.h"
//#include "UB.h"

void tstModel() {

    IloEnv env;
    IloModel model;
    IloCplex cplex;
    IloNumVarArray x;

    model = IloModel(env);
    cplex = IloCplex(model);
    x = IloNumVarArray(env);

    x.add(IloNumVar(env, 0, 10, ILOFLOAT));
    x.add(IloNumVar(env, 0, 10, ILOFLOAT));
    x.add(IloNumVar(env, 0, 10, ILOFLOAT));

    model.add(IloMinimize(env, x[0] + 2 * x[1] + x[2]));

    IloRangeArray regCtes = IloRangeArray(env);
    regCtes.add(IloRange(env, 5, IloInfinity));
    regCtes[0].setLinearCoef(x[0], 2);
    regCtes[0].setLinearCoef(x[1], 3);
    regCtes[0].setLinearCoef(x[2], 3);
    model.add(regCtes);

    IloRangeArray addCtes = IloRangeArray(env);
    addCtes.add(IloRange(env, 8, IloInfinity));
    addCtes[0].setLinearCoef(x[0], 5);
    addCtes[0].setLinearCoef(x[1], 1);
    addCtes[0].setLinearCoef(x[2], 3);
    model.add(addCtes);

    cplex.solve();
    std::cout << cplex.getObjValue() << "\n";

    model.remove(addCtes);
    addCtes.clear();
    addCtes.add(IloRange(env, 7, IloInfinity));
    addCtes[0].setLinearCoef(x[0], 5);
    addCtes[0].setLinearCoef(x[1], 2);
    addCtes[0].setLinearCoef(x[2], 4);
    model.add(addCtes);
    cplex.solve();
    std::cout << cplex.getObjValue() << "\n";

}


int main(int argc, char** argv)
{
    //tstModel();

    //std::cout.precision(6);

    // from batch script

    //std::string instance = std::string(argv[1]);
    //BranchAndBound* B = new BranchAndBound(instance);

    //B->run(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), atoi(argv[10]), atoi(argv[11]), atoi(argv[12]));

    //B.run(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    //B.run(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
    //std::cout << "solved\n";
    //B->printStatistics();

    //B.writeStatistics();
    //std::cout << "stat written\n";
    //B.writeDepthStatistics();
    //std::cout << "depth written\n";
    //B.writeUB();
    //std::cout << "UB written\n";

    //B->writeStatProbing();

    // from inside VS

    //expe();

    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/test.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/expeData/newInstances/Kirlik14-ILP_p-4_n-20_m-10_ins-5.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-ILP_p-4_n-20_m-10_ins-5.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-PPP_7_4_1-100_1-100_1-2500_1_50_random_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_4_4_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/debug_3obj_int.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/expeData/waiting/Forget21-UFLP_4_6_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/UFLP6-3.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/checkLub4obj.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/KP10-3_int.txt"; // KP10-4Debug2
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-KP_p-3_n-10_ins-3.dat"; // KP10-4Debug2
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/counterProofHyperplanes.txt";
    //std::string inst = "C:/Users/au643334/Documents/MOrepo-Forget20/instances/raw/Forget20-UFLP_6_3_1-1000_1-100_spheredown_1_1.raw"; //Forget20-KP_20_3_1-1000_spheredown_3_1.raw
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/KP20-debug.raw";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget20-AP_7_3_1-10_sphereup_1_5.raw";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/UFLPcrash.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-PPP_10_5_1-100_1-100_1-2500_1_50_random_6_6.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_5_5_1-1000_1-100_spheredown_1_5.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_8_3_1-1000_1-100_spheredown_1_4.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_5_4_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-Kirlik14-KP_p-5_n-20_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/dbgPPP.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-ILP_p-3_n-10_m-5_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Kirlik14-ILP_p-3_n-10_m-5_ins-2.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Kirlik14-KP_p-3_n-30_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-PPP_11_4_1-100_1-100_1-2500_1_50_random_10_10.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-PPP_13_3_1-100_1-100_1-2500_1_50_random_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_6_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_6_3_1-1000_1-100_spheredown_1_3.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_7_3_1-1000_1-100_spheredown_1_7.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_4_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "Forget21-UFLP_4_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_6_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Duleab21-CFLP_5_10_3.fgt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Duleab21-CFLP_10_20_1.fgt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/Forget21-UFLP_5_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-KP_p-3_n-40_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/tstAlter.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-KP_p-3_n-70_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/TambyS20-MOKP_p-3_n-100_7.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Kirlik14-KP_p-3_n-10_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_5_3_1-1000_1-100_spheredown_1_1.txt";
    std::string inst = "instances/Forget21-UFLP_6_3_1-1000_1-100_spheredown_1_1.txt"; // C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/
    //std::string inst = "instances/Kirlik14-ILP_p-4_n-20_m-10_ins-1.dat"; // C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/
    //std::string inst = "instances/Forget21-PPP_11_3_1-100_1-100_1-2500_1_50_random_1_1.txt"; // C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/
    //std::string inst = "instances/Forget21-PPP_13_3_1-100_1-100_1-2500_1_50_random_1_1.txt"; // C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_7_3_1-1000_1-100_spheredown_1_7.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_8_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Forget21-UFLP_9_3_1-1000_1-100_spheredown_1_1.txt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Bektas18-AP_p-3_n-10_ins-1.dat";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/neos-1616732.fgt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/Duleab21-CFLP_10_20_10.fgt";
    //std::string inst = "C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/instMCDM.txt";


    BranchAndBound* B = new BranchAndBound(inst);
    //BranchAndBound* B2 = new BranchAndBound(inst);

    //srand(time(0));
    

     //run 1

    //B.getYnFromFile();
    //B2->run(PRECOMPUTED_WARMSTARTED_LP_RELAX, BEST_BOUND_WS, VARIABLE_FIXING, FULL_OBJECTIVE_BRANCHING, MOST_OFTEN_FRACTIONAL_VALUE, TIME_OUT, OLD_RULE, 2, 2, 1, 0, 0, 0); //FULL_OBJECTIVE_BRANCHING // SCORE_BASED
    //B2->run(WARMSTARTED_LP_RELAX, BREADTH_FIRST, MOST_OFTEN_FRACTIONAL_VALUE, 3600);
    //B2->run(WARMSTARTED_LP_RELAX, BREADTH_FIRST, FULL_OBJECTIVE_BRANCHING, MOST_OFTEN_FRACTIONAL_VALUE, 3600, 999);
    //B.printYN();
    //B2->printStatistics();
    //B2->writeUB();
    //B->writeStatistics();
    //B->writeDepthStatistics();

    // run 2
    
    // HYBRID + 0.5
    //B->getYnFromFile();
    //B->run(PRECOMPUTED_WARMSTARTED_LP_RELAX, BEST_BOUND_WS, VARIABLE_FIXING, FULL_OBJECTIVE_BRANCHING, MOST_OFTEN_FRACTIONAL_VALUE, TIME_OUT, OLD_RULE , 2, 2, 1, 0, 0, 0, 0); // OLD_RULE
    //B->run(PRECOMPUTED_WARMSTARTED_LP_RELAX, BREADTH_FIRST, VARIABLE_FIXING, FULL_OBJECTIVE_BRANCHING, TIME_OUT, 2, 1, 0, 0, 0, HAUSDORFF); // OLD_RULE // TIME_OUT
    B->run(PRECOMPUTED_WARMSTARTED_LP_RELAX, BEST_BOUND_GAP, VARIABLE_FIXING, FULL_OBJECTIVE_BRANCHING, TIME_OUT, 2, 1, 0, 0, 14); // OLD_RULE // TIME_OUT
    //B->run(WARMSTARTED_LP_RELAX, BREADTH_FIRST, MOST_OFTEN_FRACTIONAL, FULL_OBJECTIVE_BRANCHING, TIME_OUT, 2, 1, 0, 0, 0, HAUSDORFF); // OLD_RULE
    //B->printYN();
    B->printStatistics();
    //B->writeGap();
    //B.writeStatistics();
    //B->writeStatProbing();
    //B->writeUB();
    //B->writeUBXE();

    // run 3

    // HYBRID + 0.5
    //B2->run(PRECOMPUTED_WARMSTARTED_LP_RELAX, BEST_BOUND_WS, VARIABLE_FIXING, FULL_OBJECTIVE_BRANCHING, MOST_OFTEN_FRACTIONAL_VALUE, 8000, OLD_RULE, 2, 2, 1, 0, 0, 0); // OLD_RULE
    //B->printYN();
    //B2->printStatistics();
    //B.writeStatistics();
    //B->writeStatProbing();

    // get nadir
    /*std::list<Solution*>* U = B->getUb()->getIncumbentSet();
    std::list<Solution*>::iterator u, v;
    std::vector<int> nadir(3, -1000000);
    for (u = U->begin(); u != U->end(); u++) {
        for (int k = 0; k < 3; k++) {
            if ((*u)->get_objVector(k) > nadir[k]) {
                nadir[k] = (*u)->get_objVector(k);
            }
        }
    }
    std::cout << "actual nadir: ";
    for (int k = 0; k < 3; k++) {
        std::cout << nadir[k] << " ";
    }
    std::cout << "\n";*/

    // check UBs
    /*std::list<Solution*>* Ubase = B2->getUb()->getIncumbentSet();
    std::list<Solution*>* Uprob = B->getUb()->getIncumbentSet();
    std::list<Solution*>::iterator u, v;
    bool isFound;
    for (u = Ubase->begin(); u != Ubase->end(); u++) {
        isFound = false;
        for (v = Uprob->begin(); v != Uprob->end(); v++) {
            if ((*u)->isEqual(*v)) {
                isFound = true;
            }
        }
        if (!isFound) {
            (*u)->print();
        }
    }*/

    /*std::vector<int> v = { 2919, 3342, 1230 };
    Solution s = Solution(v);
    std::list<Solution*>* Uprob = B->getUb()->getIncumbentSet();
    for (std::list<Solution*>::iterator y = Uprob->begin(); y != Uprob->end(); y++) {
        (*y)->print();
        if ((*y)->isEqual(&s)) {
            std::cout << " IT'S HERE !!!\n\n";
        }
    }*/

    // check domi in UB
    /*std::list<Solution*>* UB = B->getUb()->getIncumbentSet();
    std::list<Solution*>::iterator u, v;
    bool dominated, domiLocal;
    for (u = UB->begin(); u != UB->end(); u++) {
        dominated = false;
        for (v = UB->begin(); v != UB->end(); v++) {
            
            if ((*v)->dominate(**u)) {
                dominated = true;
            }

        }
        if (dominated) {
            (*u)->print();
        }
    }*/


    //std::vector<int> v = { -16006, -14304, -17311 };
    //Solution s = Solution(v);
    //std::list<Solution*>* Uprob = B->getUb()->getIncumbentSet();
    //for (std::list<Solution*>::iterator y = Uprob->begin(); y != Uprob->end(); y++) {
    //    //(*y)->print();
    //    if ((*y)->dominate(s)) {
    //        std::cout << " IT'S DOMINATED by ";
    //        (*y)->print();
    //        std::cout << "\n\n";
    //    }
    //}

    // run 3

    //B->run(WARMSTARTED_LP_RELAX, BREADTH_FIRST, PROBING, NO_OBJECTIVE_BRANCHING, MOST_OFTEN_FRACTIONAL_VALUE, TIME_OUT, SCORE_WS);
    //B.printYN();
    //B->printStatistics();
    //B.writeStatistics();

    delete B;
    //delete B2;

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
