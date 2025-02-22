#include "BranchAndBound.h"

/* ==========================================================
        Constructors
 ========================================================= */

 /*! \brief Default constructor of a branch-and-bound
  *
  * \param instance string. The path to the file that contains the instance solved by this branch and bound
  */
BranchAndBound::BranchAndBound(std::string instance) : lp(instance), boxModel(), queue(0), U(lp), T(), P(), stat(), scoreVariable(2, std::vector<double>(lp.get_n(), 1)), nbFeasibleBranching(2, std::vector<double>(lp.get_n(), 0)), acm() { // , sp() , L(&lp, &P)

    inst = instance;
    boxModel.build(lp, &P);
    //inst.erase(0, 19);
    //inst.erase(0, 86);
    //inst.erase(0, 10);
    //std::cout << inst << "\n";

    //lp.printConstraints();

    /*std::stringstream ss(instance);
    std::vector<std::string> result;
    while (ss.good())
    {
        std::string substr;
        getline(ss, substr, '/');
        result.push_back(substr);
    }
    inst = result[result.size() - 1];*/
}

 /* ==========================================================
         Regular Methods
  ========================================================= */

  /*! \brief Runs the branch-and-bound algorithm with the given parameters.
   *
   * \param lb int. The identifier of the lower bound set.
   * \param nodeSel int. The identifier of the node selection strategy.
   * \param varSel int. The identifier of the variable selection strategy.
   * \param ob int. The identifier of the objective branching strategy.
   */
void BranchAndBound::run(int lb, int nodeSel, int varSel, int ob, int valBranch, int timeout, int versionProbing, int limitSubPb, int domiVarFix, int adjustBBWS, int cuts, int pbcc, int flp, double epsOB, int gapMeasure, int enumThreshold) {

    srand(int(inst.size()) * lp.get_p() * lp.get_n());

    try {
        //reset();
        //getYnFromFile();
        int iteration = 0;

        //lp.printObjective();
        //lp.printConstraints();

        Timer timeLimit;
        //cpuTime.StartTimer();
        stat.totalTime.StartTimer();
        U.startTimerUB();

        // set parameters
        P.LBset = lb;
        P.nodeSelection = nodeSel;
        P.variableSelection = varSel;
        P.objectiveBranching = ob;
        P.branchingValueSelection = valBranch;
        P.timeOut = timeout;
        P.GCD = std::vector<int>(lp.get_p(), 1); // compute with euclidean algorithm for each objective?
        P.versionProbing = versionProbing;
        P.limitSubPb = limitSubPb;
        P.threshold = 0.5;
        P.domiVarFix = domiVarFix;
        P.adjustBBWSbounds = adjustBBWS;
        P.generateCuts = cuts;
        P.pbcc = pbcc;
        //P.pbcc = 0;
        if (flp == 1) P.FLP = true;
        if (pbcc == 2) P.debug = true;
        P.epsOB = epsOB;
        P.gapMeasure = gapMeasure;
        P.shiftVec = std::vector<double>(lp.get_p(), 0.0);
        computeBestBoundDirection();
        P.enumThreshold = enumThreshold;

        // get box gap
        if (P.nodeSelection == BEST_BOUND_GAP) {
            P.nadir = std::vector<int>(lp.get_p(), -1000000);
            P.ideal = std::vector<int>(lp.get_p(), -1000000);

            for (int k = 0; k < lp.get_p(); k++) {
                boxModel.solve(k, &P, &lp);
            }

            std::cout << "ideal: ";
            for (int k = 0; k < lp.get_p(); k++) {
                std::cout << P.ideal[k] << " ";
            }
            std::cout << "\nnadir: ";
            for (int k = 0; k < lp.get_p(); k++) {
                std::cout << P.nadir[k] << " ";
            }
            std::cout << "\n";
        }

        acm.build(&lp, &P);

        //if (lp.get_p() == 3)
        //    P.objectiveBranching = CONE_OBJECTIVE_BRANCHING;
        //else
        //    P.objectiveBranching = NO_OBJECTIVE_BRANCHING;

        // initialization of the B&B
        //queue.push_back(new Node(&lp,&P,&stat)); // creation of the root node
        T = TreeManager(new Node(&lp, &P, &stat, &U, &acm), &P, &U);
        Node* currentNode = nullptr;

        if (P.versionProbing == SCORE_WS)
            (*queue.begin())->setUpScoreWS();

        std::string go;
        Timer cpuPerDepth = Timer();
        // explore the tree
        //while (queue.size() != 0 && timeLimit.CumulativeTime("sec") < P.timeOut) { // && iteration < 64    4 // TIME_OUT
        //std::cout << L.getGapMax() << " & " << iteration << "  ->  " << (L.getGapMax() != 0 || iteration == 0) << "\n";
        while (!T.isEmpty() && timeLimit.CumulativeTime("sec") < P.timeOut && ((P.nodeSelection != BEST_BOUND_GAP || T.getGlobalGap() > 0) || iteration <= 1)) { //  && iteration < 14 //  && (L.getGapMax() != 0 || iteration <= 1)

            cpuPerDepth.StartTimer();
            timeLimit.StartTimer();
            //if ((iteration + 1) % 500 == 0) T.updateScoreList();
            //if ((iteration + 1) % 200 == 0) L.updateMeasures(&U, &T);
            stat.timeNodeSel.StartTimer();
            currentNode = selectNode();
            //std::cout << "\n current gap : " << currentNode->getGap() << "\n";
            //T.showScoreList();
            //if (currentNode->getGap() == 0) std::cout << " AH!\n";
            if (P.nodeSelection == BEST_BOUND_GAP) T.updateGap(currentNode->getGap()); // /!\ REMOVE IF UPDATE GAPS MANUALLY
            stat.timeNodeSel.StopTimer();
            stat.nbNodes++;

            //if (P.nodeSelection == BEST_BOUND_GAP && U.wasUpdated()) { // P.nodeSelection == BEST_BOUND_GAP && 
            //    stat.cpuGapComputation.StartTimer();
            //    T.updateGap(); // currentNode->getGap()
            //    //std::cout << "   -> reset !\n\n";
            //    //std::cout << "iteration " << iteration << " -> global gap : " << T.getGlobalGap() << "\n";
            //    stat.cpuGapComputation.StopTimer();
            //}

            stat.gapNodeUpdated.push_back(T.getGlobalGap()); //  currentNode->getGap()
            stat.totalTime.StopTimer();
            stat.cpuGap.push_back(stat.totalTime.CumulativeTime("sec"));
            stat.totalTime.StartTimer();
            //T.updateGap(currentNode->getGap());
            //std::cout << " global gap : " << T.getGlobalGap() << "\n";

            currentNode->setIteration(iteration);

            if (iteration % 1000 == 0) { // iteration > 4798 && 
                //currentNode->print();
                //lp.printObjective();
                std::cout << "\niteration: " << iteration << "\n";
                std::cout << "  -> gap : " << currentNode->getGap() << "\n";
                //currentNode->showLB();
                //currentNode->showStatus();
            }

            currentNode->process(U,iteration);

            // add hpp of LP relax to GLB

            /*if (iteration == 0) {
                L.reset();
                int fgf = 0;
                std::list<Hyperplane*>* f = currentNode->getLPrelaxPolyhedron();
                for (std::list<Hyperplane*>::iterator hpp = f->begin(); hpp != f->end(); hpp++) {
                    L.addNewHyperplane(*hpp);
                    fgf++;
                }
                std::cout << fgf << " hyperplanes added to the global LB set!\n";
            }*/

            // split node

            if (!currentNode->isFathomed()) {
                //split
                //currentNode->splitOS(&queue, &U, iteration);
                currentNode->splitOS(&T, &U, iteration);
            }
            else {
                if (currentNode->getDepth() >= stat.maxDepth) stat.maxDepth = currentNode->getDepth();
                if (currentNode->getDepth() <= stat.minDepth) stat.minDepth = currentNode->getDepth();
                stat.avgDepth += currentNode->getDepth();
                //currentNode->print();
                //currentNode->showStatus();
            }
            
            //currentNode->stopSol();
            //currentNode->showStatus();
            //currentNode->showLB();
            //if (iteration > 4798) currentNode->showStatus();
            //currentNode->getStatistics(&stat);
            //if (iteration == 13) throw std::string(" stop bro.\n");
            cpuPerDepth.StopTimer();
            if (stat.cpuDepth.size() == currentNode->getDepth())
                stat.cpuDepth.push_back(cpuPerDepth.ElapsedTime("sec"));
            else
                stat.cpuDepth[currentNode->getDepth()] += cpuPerDepth.ElapsedTime("sec");
            delete currentNode;
            iteration++;
            timeLimit.StopTimer();

            //std::cout << L.getGapMax() << " & " << iteration << "  ->  " << (L.getGapMax() != 0 || iteration <= 1) << "\n";
        }

        /*stat.gapNodeUpdated.push_back(0);
        stat.totalTime.StopTimer();
        stat.cpuGap.push_back(stat.totalTime.CumulativeTime("sec"));
        stat.totalTime.StartTimer();*/

        //cpuTime.StopTimer();
        //stat.totalTime = cpuTime.ElapsedTime("sec");
        U.stopTimerUB();
        stat.totalTime.StopTimer();
        //if (queue.size() >= 1) {
        //std::cout << " we pass here\n";
        if (!T.isEmpty()) {
            //std::cout << " but not here?\n";
            stat.solved = 0;

            // compute box gap
            P.nadir = std::vector<int>(lp.get_p(), -1000000);
            P.ideal = std::vector<int>(lp.get_p(), -1000000);
            for (int k = 0; k < lp.get_p(); k++) {
                boxModel.solve(k, &P, &lp);
            }

            // compute gap
            if (P.LBset == PRECOMPUTED_WARMSTARTED_LP_RELAX) {
                T.updateGapFinal(); // Final
                stat.finalGap = T.getGlobalGap();
                std::cout << "Final gap: " << T.getGlobalGap() << "\n";
            }
        }
        else {
            std::cout << " and here\n";
            stat.solved = 1;
            //T.updateGapFinal();
        }
        stat.avgDepth /= (stat.nbFathomedDominance + stat.nbFathomedInfeasibility + stat.nbFathomedOptimality);

        std::cout << "Tree size: " << stat.nbNodes << " + " << T.size() << " = " << stat.nbNodes + T.size() << "\n";
    }
    catch (std::string msg) {
        std::cout << msg << std::endl;
        exit(1);
    }
}

/*! \brief Runs the branch-and-bound algorithm with the given parameters.
     *
     * This function runs the branch-and-bound algorithm with the given parameters, which are:
     * 1) the lower bound set used
     * 2) the node selection strategy
     * 4) the time limit
     * See GlobalConstants.h for the available identifiers.
     * \param lb int. The identifier of the lower bound set.
     * \param nodeSel int. The identifier of the node selection strategy.
     * \param ob int. The identifier of the objective branching strategy.
     * \param timeout int. The time limit.
     */
void BranchAndBound::run(int lb, int nodeSel, int valBranch, int timeout) {
    run(lb, nodeSel, MOST_OFTEN_FRACTIONAL, NO_OBJECTIVE_BRANCHING, valBranch, timeout, OLD_RULE, 950, 2, 1, 0, 0, 0, 0, -1, 0);
}

void BranchAndBound::run(int lb, int nodeSel, int ob, int valBranch, int limitSubPb, int timeout){
    run(lb, nodeSel, MOST_OFTEN_FRACTIONAL, ob, valBranch, timeout, OLD_RULE, limitSubPb, 2, 1, 0, 0, 0, 0, -1, 0);
}

void BranchAndBound::run(int lb, int nodesel, int varsel, int OB, int timeout, int domivarfix, int wsscale, int coverCuts, int FLPvarsel, int epsOB, int gapMeasure) {
    run(lb, nodesel, varsel, OB, MOST_OFTEN_FRACTIONAL_VALUE, timeout, OLD_RULE, 9500, domivarfix, wsscale, 0, coverCuts, FLPvarsel, epsOB, gapMeasure, 0);
}

void BranchAndBound::run(int lb, int nodesel, int varsel, int OB, int timeout, int domivarfix, int wsscale, int coverCuts, int FLPvarsel, int enumThreshold) {
    run(lb, nodesel, varsel, OB, MOST_OFTEN_FRACTIONAL_VALUE, timeout, OLD_RULE, 9500, domivarfix, wsscale, 0, coverCuts, FLPvarsel, 0, HAUSDORFF, enumThreshold);
}

/*! \brief Select the next node to be explored in the tree
 *
 * \return a pointer to a Node.
 */
Node* BranchAndBound::selectNode() {

    Node* selectedNode = nullptr;

    if (P.nodeSelection == DEPTH_FIRST) {
        //selectedNode = queue.back();
        //queue.pop_back();
        selectedNode = T.extractNode();
    }
    else if (P.nodeSelection == BREADTH_FIRST) {
        //selectedNode = queue.front();
        //queue.pop_front();
        selectedNode = T.extractNode();
        //std::cout << "score: " << selectedNode->getScore() << "\n";
    }
    else if (P.nodeSelection == SCORE_BASED) {
        std::list<Node*>::iterator nd, sel;
        double bestScore = -1;
        //std::cout << "\n new it -------------------\n";
        for (nd = queue.begin(); nd != queue.end(); nd++) {
            if ((*nd)->getScore() > bestScore) {
                sel = nd;
                bestScore = (*nd)->getScore();
                //std::cout << "score = " << bestScore << "\n";
            }
        }
        selectedNode = *sel;
        queue.erase(sel);
    }
    else if (P.nodeSelection == HYBRID || P.nodeSelection == MOST_FRACTIONAL || P.nodeSelection == BEST_BOUND_WS || P.nodeSelection == BEST_BOUND_MAXMIN_GAP || P.nodeSelection == BEST_BOUND_GAP) {
        selectedNode = T.extractNode();
    }
    else {
        //std::cout << "  !!! Undefined node selection !!!\n";
        throw std::string("Error: Undefined node selection");
    }

    return selectedNode;
}

/*! \brief Update the scores of the variables
 *
 * \param Node* nd. The scores are updated with the information gained in node nd.
 */
void BranchAndBound::updateScores(Node* nd) {

    int i = nd->getLastBranchingVariable();

    //scoreVariable[]

}

/* Compute the weight vector used for BEST_BOUND strategy
 */
void BranchAndBound::computeBestBoundDirection() {

    int sumCoefObj;
    double sumCoefAll = 0;

    for (int k = 0; k < lp.get_p(); k++) {
        //sumCoefObj = 0;
        //for (int i = 0; i < lp.get_n(); i++) {
            //sumCoefObj += lp.get_objective(k, i);
        //}
        //P.searchDir.push_back(1.0 / double(sumCoefObj));
        P.searchDir.push_back(1.0 / double(lp.getRangeObjective(k)));
        sumCoefAll += P.searchDir[k];
    }

    for (int k = 0; k < lp.get_p(); k++) {
        P.searchDir[k] *= 1.0 / sumCoefAll;
        P.selDir.push_back(P.searchDir[k]);
        //std::cout << P.searchDir[k] << " ";
    }
    //std::cout << "\n";
}

/*! \brief Print out YN
 */
void BranchAndBound::printYN() {
    std::cout << "\n\n\n ================================================== \n";
    if (queue.size() >= 1) {
        std::cout << " !!! There are still some unexplored nodes, this is thus an approximation of YN !!!";
    }
    U.print();
}

/*! \brief Reset the branch-and-bound
 *
 * This function reset the branch-and-bound by deleting all its elements except the linear program.
 */
void BranchAndBound::reset() {

    std::list<Node*>::iterator nd;
    for (nd = queue.begin(); nd != queue.end(); nd++) {
        delete *nd;
    }
    queue.clear();

    U.reset();
    P = Parameters();
    stat = Statistics();
}

/*! \brief Write the statistic of this run to the stat file (results.txt / .csv)
 */
void BranchAndBound::writeStatistics() {

    std::ofstream file;
    //file.open("C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/expeData/results.txt", std::ios_base::app); // append instead of overwrite
    file.open("C:/Users/au643334/Desktop/expeProject2/results.txt", std::ios_base::app); // append instead of overwrite
    
    // instance and parameters
    file << inst << ","; // instance
    file << lp.get_p() << ","; // p
    file << lp.get_n() << ","; // n
    if (P.LBset == LP_RELAX) file << "LP,"; // LB set
    else if (P.LBset == WARMSTARTED_LP_RELAX) file << "WLP,";
    else file << ",";
    if (P.nodeSelection == DEPTH_FIRST) file << "depth,"; // node sel
    else if (P.nodeSelection == BREADTH_FIRST) file << "breadth,";
    else file << ",";
    if (P.objectiveBranching == NO_OBJECTIVE_BRANCHING) file << "noOB,"; // OB
    else if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) file << "fullOB,";
    else if (P.objectiveBranching == CONE_OBJECTIVE_BRANCHING) file << "coneOB,";
    else file << ",";
    //if (P.branchingValueSelection == MEDIAN) file << "med,";
    if (P.branchingValueSelection == MEDIAN) file << "med2,";
    //else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) file << "mofv,";
    //else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) file << "mofvRevisited,";
    else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) file << "mofvRevisited2,";
    else if (P.branchingValueSelection == RANDOM) file << "rand,";
    else file << ",";

    // performance and YN
    if (stat.solved == 1) file << "1,"; // solved
    else if (stat.solved == 0) file << "0,";
    else file << ",";
    file << U.getNbNonDominatedPoints() << ","; // YN
    file << stat.totalTime.CumulativeTime("sec") << ","; // cpuTotal
    
    // LB performance profile
    file << stat.lpSolved << ","; // nbLpSolved
    file << stat.timeInitialization.CumulativeTime("sec") << ","; // cpuInitialization
    file << stat.timeFeasibilityCheck.CumulativeTime("sec") << ","; // cpuCplex
    file << stat.timeUpdatePolyhedron.CumulativeTime("sec") << ","; // cpuUpdatePolyhedron

    // Tree structure
    file << stat.nbNodes << ","; // nbNodes
    file << stat.minDepth << ","; // minDepth
    file << stat.maxDepth << ","; // maxDepth
    file << stat.avgDepth << ","; // avgDepth
    file << stat.nbFathomedInfeasibility << ","; // nbFathomedInfeas
    file << stat.nbFathomedOptimality << ","; // nbFathomedOptimality
    file << stat.nbFathomedDominance << ","; // nbFathomedDominance

    // BB performance profile
    file << stat.timeDominanceTest.CumulativeTime("sec") << ",";
    file << stat.timeUpdateUB.CumulativeTime("sec") << ",";
    file << stat.timeLBComputation.CumulativeTime("sec") << ",";
    file << stat.timeComputeOB.CumulativeTime("sec") << ",";
    file << stat.timeVarSel.CumulativeTime("sec") << ",";
    file << stat.timeNodeSel.CumulativeTime("sec") << ",";

    // added later
    file << stat.timeCopyLB.CumulativeTime("sec") << ",";

    // OB
    file << stat.avgDepthOB / std::max(stat.nbOccurencesOB,1) << ",";
    std::cout << stat.avgDepthOB;
    file << stat.minDepthOB << ",";
    file << stat.maxDepthOB << ",";
    file << stat.avgSubPbOB / std::max(stat.nbOccurencesOB, 1) << ",";
    file << stat.minSubPbOB << ",";
    file << stat.maxSubPbOB << ",";
    file << stat.nbOccurencesOB << ",";
    //file << stat.cpuSlubsOB.CumulativeTime("sec") << ",";
    //file << stat.cpuAdditionalDomiTestOB.CumulativeTime("sec") << ",";
    file << stat.avgNbLubs / (stat.nbNodes - stat.nbFathomedInfeasibility - stat.nbFathomedOptimality - stat.nbFathomedDominance) << ",";
    file << stat.maxNbLubs << ",";
    file << stat.nbDominanceTestLubHpp << ",";

    // variable fixing
    


    file << P.limitSubPb << "\n";
}

/*! \brief Write the statistic of this run to the stat file (results.txt / .csv)
 *
 * This function write the statistics of this BB run to the stat file.
 */
void BranchAndBound::writeDepthStatistics() {

    std::ofstream file;

    // build path to output file

    std::string pathFile = inst;
    size_t s = pathFile.size();
    pathFile.erase(s - 4, s);

    pathFile = "statFiles/Depth/" + pathFile; // + "_UB_WARMSTART";
    //if (P.LBset == LP_RELAX) pathFile = pathFile + std::string("_LP");
    //else if (P.LBset == WARMSTARTED_LP_RELAX) pathFile = pathFile + std::string("_WLP");

    //if (P.branchingValueSelection == MEDIAN) pathFile = pathFile + std::string("_MED2"); //_MED
    //else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) pathFile = pathFile + std::string("_MOFVREVISITED2"); // _MOFV _MOFVREVISITED
    //else if (P.branchingValueSelection == RANDOM) pathFile = pathFile + std::string("_RAND");

    if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_FULLOB");
    else if (P.objectiveBranching == CONE_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_CONEOB");
    else if (P.objectiveBranching == LIMITED_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_LIMITEDOB_") + std::to_string(P.limitSubPb);
    else pathFile = pathFile + std::string("_NOOB");

    if (P.variableSelection == VARIABLE_FIXING) pathFile = pathFile + std::string("_VARFIX");
    else pathFile = pathFile + std::string("_MOF");

    pathFile = pathFile + std::string(".txt");

    // create & write file

    file.open(pathFile); // , std::ofstream::out
    file << "depth,nbNodes,nbInfeas,avgFacet,avgVertex,minFacet,minVertex,maxFacet,maxVertex,cpuUpdatePoly,cpuDepth,avgFeasVtx,avgNewFacets,avgVtxNoRay,avgFacetsNoRay,avgNewFacetsNoRay,avgIntegerVtx,nbNodesOB,avgNbSubPb,minNbSubPb,maxNbSubPb\n";
    
    std::cout << stat.nbNodesAtStage.size() << "vs" << stat.nbInfeasibleNodes.size() << "\n";

    while (stat.nbNodesAtStage.size() >= stat.nbInfeasibleNodes.size()){
        //std::cout << "add loop 1\n";
        stat.nbInfeasibleNodes.push_back(0);
    }
    while (stat.nbNodesAtStage.size() != stat.nodesOBPerDepth.size()) {
        std::cout << "add loop 2\n";
        stat.nodesOBPerDepth.push_back(0);
    }
    while (stat.nbNodesAtStage.size() != stat.avgSubPbOBPerDepth.size()) {
        std::cout << "add loop 3\n";
        stat.avgSubPbOBPerDepth.push_back(0);
    }
    while (stat.nbNodesAtStage.size() != stat.minSubPbOBPerDepth.size()) {
        std::cout << "add loop 4\n";
        stat.minSubPbOBPerDepth.push_back(100000);
    }
    while (stat.nbNodesAtStage.size() != stat.maxSubPbOBPerDepth.size()) {
        std::cout << "add loop 5\n";
        stat.maxSubPbOBPerDepth.push_back(0);
    }

    std::cout << "1\n";

    for (int d = 0; d < stat.nbNodesAtStage.size(); d++) {
        stat.nbNodesAtStage[d] += stat.nbInfeasibleNodes[d];
        file << d << "," << stat.nbNodesAtStage[d] << "," << stat.nbInfeasibleNodes[d] << ",";
        file << stat.avgFacets[d]/(stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << "," << stat.avgVertices[d]/(stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.minFacets[d] << "," << stat.minVertices[d] << ",";
        file << stat.maxFacets[d] << "," << stat.maxVertices[d] << ",";
        file << stat.timeUpdatePoly[d] << "," << stat.cpuDepth[d] << ",";
        file << stat.nbFeasVtx[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << "," << stat.nbNewFacets[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.vtxOnly[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.facetsNoRay[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.newFacetsNoRay[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.avgIntegerVtx[d] / (stat.nbNodesAtStage[d] - stat.nbInfeasibleNodes[d]) << ",";
        file << stat.nodesOBPerDepth[d] << ",";
        file << stat.avgSubPbOBPerDepth[d] / std::max(stat.nodesOBPerDepth[d], 1) << ",";
        file << stat.minSubPbOBPerDepth[d] << "," << stat.maxSubPbOBPerDepth[d] << "\n";
    }

    std::cout << "br\n";


    file.close();

    std::cout << "kurozu\n";
}

/*! \brief Write the statistic of this run to the stat file (results.txt / .csv)
     *
     * This function write the statistics of this BB run to the stat file.
     */
void BranchAndBound::writeUB() {

    std::ofstream file;

    // build path to output file

    std::string pathFile = inst;
    size_t s = pathFile.size();
    pathFile.erase(s - 4, s);

    pathFile = pathFile;// +"_UB_WARMSTART"; // "statFiles/UB/" + 
    //if (P.LBset == LP_RELAX) pathFile = pathFile + std::string("_LP");
    //else if (P.LBset == WARMSTARTED_LP_RELAX) pathFile = pathFile + std::string("_WLP");

    //if (P.branchingValueSelection == MEDIAN) pathFile = pathFile + std::string("_MED2");
    //else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) pathFile = pathFile + std::string("_MOFVREVISITED2"); // _MOFV
    //else if (P.branchingValueSelection == RANDOM) pathFile = pathFile + std::string("_RAND");

    //if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_FULLOB");
    //else if (P.objectiveBranching == CONE_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_CONEOB");
    //else if (P.objectiveBranching == LIMITED_OBJECTIVE_BRANCHING) pathFile = pathFile + std::string("_LIMITEDOB_") + std::to_string(P.limitSubPb);
    //else pathFile = pathFile + std::string("_NOOB");

    //if (P.variableSelection == VARIABLE_FIXING) pathFile = pathFile + std::string("_VARFIX");
    //else pathFile = pathFile + std::string("_MOF");

    pathFile = pathFile + std::string("_UB.txt");

    std::cout << " File is: " << pathFile << "\n";

    // create & write file

    file.open(pathFile); // , std::ofstream::out
    for (int k = 0; k < lp.get_p(); k++) {
        file << "obj" << k << ",";
    }
    file << "cpu\n";

    std::list<Solution*>* YN = U.getYN();
    for (std::list<Solution*>::iterator s = YN->begin(); s != YN->end(); s++) {
        for (int k = 0; k < lp.get_p(); k++) {
            file << (*s)->get_objVector(k) << ",";
        }
        file << (*s)->getCpu() << "\n";
    }

    file.close();

}

void BranchAndBound::writeUBXE() {

    std::ofstream file;
    std::string pathFile = "tstValVar.txt";

    // create & write file

    file.open(pathFile); // , std::ofstream::out
    for (int k = 0; k < lp.get_p(); k++) {
        file << "obj" << k << ",";
    }
    for (int i = 0; i < lp.get_n(); i++) {
        file << "x" << i << ",";
    }
    file << "cpu\n";

    std::list<Solution*>* YN = U.getYN();
    for (std::list<Solution*>::iterator s = YN->begin(); s != YN->end(); s++) {
        for (int k = 0; k < lp.get_p(); k++) {
            file << (*s)->get_objVector(k) << ",";
        }
        for (int i = 0; i < lp.get_n(); i++) {
            file << (*s)->get_preImage(i) << ",";
        }
        file << (*s)->getCpu() << "\n";
    }

    file.close();

}


/*! \brief Print out statistics
 *
 * This function prints various statistics about this branch-and-bound run.
 */
void BranchAndBound::printStatistics() {

    std::cout << "\n\n =====================================================================\n";
    std::cout << "       Node selection | ";
    if (P.nodeSelection == BREADTH_FIRST) std::cout << "Breadth first\n";
    else if (P.nodeSelection == DEPTH_FIRST) std::cout << "Depth first\n";
    else if (P.nodeSelection == HYBRID) std::cout << "Hybrid at " << P.threshold << "\n";
    else if (P.nodeSelection == MOST_FRACTIONAL) std::cout << "Most fractional father node\n";
    else if (P.nodeSelection == BEST_BOUND_WS) std::cout << "Best bound WS\n";
    else if (P.nodeSelection == BEST_BOUND_MAXMIN_GAP) std::cout << "Best bound max-min gap\n";
    else if (P.nodeSelection == BEST_BOUND_GAP) std::cout << "Best bound gap\n";

    std::cout << "   Variable selection | ";
    if (P.variableSelection == FIRST_INDEX) std::cout << "First free index\n";
    else if (P.variableSelection == MOST_OFTEN_FRACTIONAL) std::cout << "Most often fractional\n";
    else if (P.variableSelection == PROBING) std::cout << "Probing\n";
    else if (P.variableSelection == PROBING_PRESOLVE_LP) std::cout << "Probing with LP presolve\n";
    else if (P.variableSelection == PROBING_PRESOLVE_IP) std::cout << "Probing with IP presolve\n";
    else if (P.variableSelection == VARIABLE_FIXING) std::cout << "Variable Fixing\n";

    std::cout << "  Objective Branching | ";
    if (P.objectiveBranching == NO_OBJECTIVE_BRANCHING) std::cout << "None\n";
    else if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) std::cout << "Full\n";
    else if (P.objectiveBranching == CONE_OBJECTIVE_BRANCHING) std::cout << "Cone\n";
    else if (P.objectiveBranching == LIMITED_OBJECTIVE_BRANCHING) std::cout << "Limited\n";

    std::cout << "      Lower Bound Set | ";
    if (P.LBset == LP_RELAX) std::cout << "LP relax (no warmstart)\n";
    else if (P.LBset == WARMSTARTED_LP_RELAX) std::cout << "Warmstarted LP relax\n";
    else if (P.LBset == PRECOMPUTED_WARMSTARTED_LP_RELAX) std::cout << "Precomputed Warmstarted LP relax\n";

    std::cout << "\n Non-dominated points found: " << U.getNbNonDominatedPoints() << "\n";

    double cputOther = stat.totalTime.CumulativeTime("sec") - stat.timeLBComputation.CumulativeTime("sec") - stat.timeUpdateUB.CumulativeTime("sec") - stat.timeDominanceTest.CumulativeTime("sec");
    std::cout << "\n Elapsed time:   " << stat.totalTime.CumulativeTime("sec") << " sec\n\n";
    std::cout << "    -> LB computation: " << stat.timeLBComputation.CumulativeTime("sec") << " sec ( " << 100 * stat.timeLBComputation.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    if (P.LBset == LP_RELAX || P.LBset == WARMSTARTED_LP_RELAX) {
        std::cout << "          -> Initialization: " << stat.timeInitialization.CumulativeTime("sec") << " sec ( " << 100 * stat.timeInitialization.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
        std::cout << "          -> Feasibility checks: " << stat.timeFeasibilityCheck.CumulativeTime("sec") << " sec ( " << 100 * stat.timeFeasibilityCheck.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
        //std::cout << "          -> Find furthest feasible point: " << stat.timeFurthestFeasiblePoint << " sec ( " << 100 * stat.timeFurthestFeasiblePoint / stat.totalTime << " % )\n";
        //std::cout << "          -> Find hyperplane (dual benson): " << stat.timeDualBenson << " sec ( " << 100 * stat.timeDualBenson / stat.totalTime << " % )\n";
        std::cout << "          -> Update polyhedron: " << stat.timeUpdatePolyhedron.CumulativeTime("sec") << " sec ( " << 100 * stat.timeUpdatePolyhedron.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    }
    std::cout << "    -> Dominance test: " << stat.timeDominanceTest.CumulativeTime("sec") << " sec ( " << 100 * stat.timeDominanceTest.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "    -> Update UB: " << stat.timeUpdateUB.CumulativeTime("sec") << " sec ( " << 100 * stat.timeUpdateUB.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "    -> others: " << cputOther << " sec ( " << 100 * cputOther / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "          -> Node selection: " << stat.timeNodeSel.CumulativeTime("sec") << " sec ( " << 100 * stat.timeNodeSel.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "          -> Variable fixing: " << stat.cpuProbing.CumulativeTime("sec") << " sec ( " << 100 * stat.cpuProbing.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "          -> Gap computation: " << stat.cpuGapComputation.CumulativeTime("sec") << " sec ( " << 100 * stat.cpuGapComputation.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";


    int nbFathomed = stat.nbFathomedInfeasibility + stat.nbFathomedDominance + stat.nbFathomedOptimality;
    std::cout << "\n Node explored: " << stat.nbNodes << "\n";
    //std::cout << "   -> fathomed by dominance: " << stat.nbFathomedDominance << " ( " << 100 * double(stat.nbFathomedDominance) / double(stat.nbNodes) << " % )\n"; // stat.nbFathomedDominance + stat.nbFathomedInfeasibility + stat.nbFathomedOptimality

    //if (P.LBset == LP_RELAX || P.LBset == WARMSTARTED_LP_RELAX) {
    //    std::cout << "\n LP solved: " << stat.lpSolved << "\n";
    //}

    //std::cout << "\n Debug : " << stat.debug.CumulativeTime("sec") << "\n";
    //std::cout << "    -> # domi tests : " << stat.debugNbDomiTest << "\n";
    //std::cout << "    -> # gap computations : " << stat.debugNbComputeGap << "\n";

   /*std::cout << " ####### Probing #######\n";
    std::cout << "\n LPs probing: " << stat.lpProbing << "\n";
    std::cout << "\n nb fixing LPs: " << stat.nbFixingLp << " ( " << 100 * stat.nbFixingLp / max(stat.lpProbing, 1) << " % )\n";
    std::cout << " cpu probing: " << stat.cpuProbing.CumulativeTime("sec")<< "\n";
    std::cout << "   -> cpu cplex: " << stat.cpuProbingCplex.CumulativeTime("sec") << "\n";
    std::cout << "   -> cpu preprocessing: " << stat.cpuProbingManual.CumulativeTime("sec") << "\n";*/

    //std::cout << "\n\n   Nb OB : " << stat.nbOccurencesOB << "\n";
    //std::cout << "   Proportion nodes : " << 100 * double(stat.nbOccurencesOB) / double(stat.nbNodes) << "%\n";
    //std::cout << "   Avg subpb : " << stat.avgSubPbOB / stat.nbOccurencesOB << "\n";

    std::cout << "\n   Skipped nodes: " << stat.nbSkippedLB << " ( " << 100 * double(stat.nbSkippedLB) / double(stat.nbNodes) << " % )\n";
    std::cout << "   Cpu processing skipped: " << stat.cpuSkip.CumulativeTime("sec") << " ( " << 100 * stat.cpuSkip.CumulativeTime("sec") / stat.totalTime.CumulativeTime("sec") << " % )\n";
    std::cout << "   Nb lp solved: " << stat.lpSolved + stat.lpProbing << " ( " << stat.lpProbing << " by variable fixing)\n";
}


/*! \brief Write the statistic of this run to the stat file (results.txt / .csv)
 */
void BranchAndBound::writeStatProbing() {

    //std::cout << "do we even enter here??\n";

    std::ofstream file;
    //file.open("C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/instances/expeData/results.txt", std::ios_base::app); // append instead of overwrite
    file.open("results.txt", std::ios_base::app); // append instead of overwrite C:/Users/au643334/Desktop/testsSophie/

    //std::string pathFile = inst;
    //size_t s = pathFile.size();
    //pathFile.erase(s - 4, s);
    //pathFile.erase(0, 86); // 0 to n - 1

    //pathFile = "res/results_" + pathFile;
    //if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) pathFile = pathFile + "_FULL";
    //else if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) pathFile = pathFile + "_CONE";
    //else pathFile = pathFile + "_NOOB";
    //if (P.variableSelection == VARIABLE_FIXING) pathFile = pathFile + "_VARFIX";
    //else if (P.variableSelection == MOST_OFTEN_FRACTIONAL) pathFile = pathFile + "_MOF";
    //pathFile = pathFile + ".txt";

    //std::cout << pathFile << "\n";

    ////pathFile = "filedokodesuka.txt";

    //file.open(pathFile); //"C:/Users/au643334/Desktop/instCluster/results/" + inst + ".txt"


    //C:/Users/au643334/source/repos/LinearRelaxationBasedMultiObjectiveBranchAndBound/Code/

    // instance and parameters
    file << inst << ","; // instance
    file << lp.get_p() << ","; // p
    file << lp.get_n() << ","; // n

    // configLB
    if (P.LBset == LP_RELAX) file << "LP,"; // LB set
    else if (P.LBset == WARMSTARTED_LP_RELAX) file << "WLP,";
    else if (P.LBset == PRECOMPUTED_WARMSTARTED_LP_RELAX) file << "PWLP,";
    else file << ",";

    // configNodeSel
    if (P.nodeSelection == DEPTH_FIRST) file << "depth,"; // node sel
    else if (P.nodeSelection == BREADTH_FIRST) file << "breadth,";
    else if (P.nodeSelection == BEST_BOUND_WS && !P.adjustBBWSbounds) file << "bestBoundWs,";
    else if (P.nodeSelection == BEST_BOUND_WS && P.adjustBBWSbounds) file << "bestBoundWsAdjusted,";
    else file << ",";

    // configVarSel
    if (P.variableSelection == MOST_OFTEN_FRACTIONAL) file << "mof,";
    else if (P.variableSelection == VARIABLE_FIXING && P.domiVarFix == 0) file << "varFix,";
    else if (P.variableSelection == VARIABLE_FIXING && P.domiVarFix == 2) file << "varFixDomi,";

    // configOB
    if (P.objectiveBranching == NO_OBJECTIVE_BRANCHING) file << "noOB,"; // OB
    else if (P.objectiveBranching == FULL_OBJECTIVE_BRANCHING) file << "fullOB,";
    else if (P.objectiveBranching == CONE_OBJECTIVE_BRANCHING) file << "coneOB,";
    else file << ",";

    // coverCuts
    if (P.pbcc == 0) file << "0,";
    else if (P.pbcc == 1) file << "1,";
    else file << ",";

    // FLPvarsel
    if (P.FLP == 0) file << "0,";
    else if (P.FLP == 1) file << "1,";
    else file << ",";

    // epsOB
    file << P.epsOB << ",";


    //if (P.variableSelection == MOST_OFTEN_FRACTIONAL) file << "mof,";
    //else if (P.variableSelection == PROBING && P.versionProbing == OLD_RULE) file << "probMof,";
    ////else if (P.variableSelection == PROBING && P.versionProbing == WORST_BEST_YI) file << "probWByI,";
    //else if (P.variableSelection == PROBING && P.versionProbing == CLOSEST_DIMENSION) file << "probClosestInfeasDim,";
    //else if (P.variableSelection == PROBING && P.versionProbing == SCORE_WS) file << "probScoreBased,";
    //else if (P.variableSelection == VARIABLE_FIXING && P.domiVarFix == 0) file << "variableFixing0,";
    //else if (P.variableSelection == VARIABLE_FIXING && P.domiVarFix == 1) file << "variableFixing1,";
    //else if (P.variableSelection == VARIABLE_FIXING && P.domiVarFix == 2) file << "variableFixingDomi,";
    //else file << ",";

    //if (P.branchingValueSelection == MEDIAN) file << "med,";
    //else if (P.branchingValueSelection == MOST_OFTEN_FRACTIONAL_VALUE) file << "mofv,";
    //else if (P.branchingValueSelection == RANDOM) file << "rand,";
    //else file << ",";

    // performance and YN
    if (stat.solved == 1) file << "1,"; // solved
    else if (stat.solved == 0) file << "0,";
    else file << ",";
    file << U.getNbNonDominatedPoints() << ","; // YN
    file << stat.totalTime.CumulativeTime("sec") << ","; // cpu

    // tree struc
    file << stat.nbNodes << ","; // nbNodes

    // LP solved
    file << stat.lpSolved + stat.lpProbing << ","; // nbLpSolved
    file << stat.lpProbing << ",";

    // Tree structure
    
    //file << stat.minDepth << ","; // minDepth
    //file << stat.maxDepth << ","; // maxDepth
    //file << stat.avgDepth << ","; // avgDepth
    //file << stat.nbFathomedInfeasibility << ","; // nbFathomedInfeas
    //file << stat.nbFathomedOptimality << ","; // nbFathomedOptimality
    //file << stat.nbFathomedDominance << ","; // nbFathomedDominance

    // BB performance profile
    file << stat.timeDominanceTest.CumulativeTime("sec") << ",";
    file << stat.timeUpdateUB.CumulativeTime("sec") << ",";
    file << stat.timeLBComputation.CumulativeTime("sec") << ",";
    file << stat.timeComputeOB.CumulativeTime("sec") << ",";
    file << stat.timeVarSel.CumulativeTime("sec") << ",";
    file << stat.timeNodeSel.CumulativeTime("sec") << ",";
    file << stat.cpuProbing.CumulativeTime("sec") << ",";
    file << stat.cpuProbingCplex.CumulativeTime("sec") << ",";
    file << stat.cpuProbingManual.CumulativeTime("sec") << ",";
    file << stat.nbFixingLp << ",";

    // extr pts lb
    stat.avgExtrPts /= stat.nbNodes - stat.nbFathomedInfeasibility;
    file << stat.avgExtrPts << ",";
    //std::cout << " avg extr pts: " << stat.avgExtrPts << "\n";

    //std::cout << "eps OB : " << P.epsOB << "\n";

    // OB stat
    //file << P.epsOB << ",";
    file << stat.nbOccurencesOB << ",";
    file << 100 * double(stat.nbOccurencesOB) / double(stat.nbNodes) << ",";
    file << stat.avgSubPbOB / std::max(stat.nbOccurencesOB,1) << ",";

    file << T.size() << ",";
    
    // final gap
    file << stat.finalGap << "\n";
}

void BranchAndBound::writeGap() {

    std::ofstream file;
    std::string pathFile = "gap.txt";

    // create & write file

    file.open(pathFile); // , std::ofstream::out

    stat.gapNodeUpdated.push_back(0);
    for (int i = 0; i < stat.gapNodeUpdated.size() - 1; i++) {
        file << stat.gapNodeUpdated[i] << ",";
    }
    file << stat.gapNodeUpdated.back() << "\n";

    stat.cpuGap.push_back(stat.totalTime.CumulativeTime("sec"));
    for (int i = 0; i < stat.cpuGap.size() - 1; i++) {
        file << stat.cpuGap[i] << ",";
    }
    file << stat.cpuGap.back();

    file.close();

}

void BranchAndBound::getYnFromFile() {

    //std::string path = "UB/" + inst;
    std::cout << inst << "\n";
    std::string path = inst; // "instances/" + 
    path.erase(path.size() - 4, path.size());
    //path += "_WLP_MOFVREVISITED2.txt";
    path += "_FULLOB_VARFIX.txt";
    std::cout << path << "\n";

    std::ifstream f(path);

    if (f) {

        std::string line;
        getline(f, line);

        while (getline(f, line)) {
            if (!line.empty()) {
                // get data from current point
                std::istringstream iss(line);
                std::string lineStream;
                std::vector<int> objVec(0);
                while (getline(iss, lineStream, ',')) {
                    objVec.push_back(stoi(lineStream));
                }
                objVec.pop_back();
                // create solution & update UB
                U.addSol(new Solution(objVec));
            }
        }

        printYN();

    }
    else {
        std::cout << "Problem with UB file.\n";
    }
}


UpperBoundSet* BranchAndBound::getUb() {
    return &U;
}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


//allCplexModels::allCplexModels() {
//    weightedSum = new WeightedSumModel();
//    feasibilityCheck = new FeasibilityCheckModel();
//    dualBenson = new DualBensonModel();
//    furthestFeasiblePoint = new FurthestFeasiblePointModel();
//    prob = new ProbingModel();
//    varFix = new VariableFixingModel();
//    ws = new WeightedSumModel();
//}
//
//allCplexModels::~allCplexModels() {
//    delete weightedSum;
//    delete feasibilityCheck;
//    delete dualBenson;
//    delete furthestFeasiblePoint;
//    delete prob;
//    delete varFix;
//    delete ws;
//}
//
//void allCplexModels::build(MathematicalModel* lp, Parameters* param) {
//    weightedSum->build(*lp);
//    feasibilityCheck->build(*lp);
//    dualBenson->build(*lp);
//    furthestFeasiblePoint->build(*lp);
//    prob->build(*lp, param);
//    varFix->build(*lp, param);
//    ws->build(*lp);
//}
