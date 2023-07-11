#include "Tree.h"

TreeManager::TreeManager() : P(NULL), U(NULL), threshold(-2), L(0), nbLayers(0), ruleLayer(0), currentNodeRule(BREADTH_FIRST), globalGap(100000), lastShuffle(0) {
}

/* Constructor of the Tree, initialized with the root node.
 *
 * \param Node* nd. A pointer to the root node.
 * \param Parameters* P. Parameters of the algorithm, that in particular tells how the tree should be explored.
 */
TreeManager::TreeManager(Node* nd, Parameters* param, UpperBoundSet* UB) : P(param), U(UB), L(0), heapL(), nbLayers(1), ruleLayer(0), currentNodeRule(BREADTH_FIRST), globalGap(100000), lastShuffle(0) {

    srand(time(0));

    // set the threshold parameter
    if (P->nodeSelection == BREADTH_FIRST) threshold = -1;
    else if (P->nodeSelection == DEPTH_FIRST) threshold = 2;
    else threshold = P->threshold;

    // create a new layer and initialize it with the root node.
    L.push_back(std::list<Node*>(0));
    L.front().push_back(nd);
    heapL.push(nd);
    if (P->nodeSelection == BREADTH_FIRST) ruleLayer.push_back(BREADTH_FIRST);
    else if (P->nodeSelection == DEPTH_FIRST) ruleLayer.push_back(DEPTH_FIRST);
    else if (P->nodeSelection == HYBRID) ruleLayer.push_back(BREADTH_FIRST);
    else if (P->nodeSelection == MOST_FRACTIONAL) ruleLayer.push_back(MOST_FRACTIONAL);
    else if (P->nodeSelection == BEST_BOUND_WS) ruleLayer.push_back(BEST_BOUND_WS);
    else if (P->nodeSelection == BEST_BOUND_MAXMIN_GAP) ruleLayer.push_back(BEST_BOUND_MAXMIN_GAP);
    else if (P->nodeSelection == BEST_BOUND_GAP) ruleLayer.push_back(BEST_BOUND_GAP);

    //if (P->nodeSelection == BEST_BOUND_WS) make_heap(L.back().begin(), L.back().end(), &nodeComparator::operator()); // min heap
    //if (P->nodeSelection == BEST_BOUND_WS) ; // min heap

}

/* Extract the next node to explore. Set currentNode to the extracted node.
 *
 * \return a pointer to the next node explored.
 */
Node* TreeManager::extractNode() {

    Node* nd = NULL;

    if (ruleLayer.back() == DEPTH_FIRST) {
        nd = L.back().back();
        L.back().pop_back();
    }
    else if (ruleLayer.back() == BREADTH_FIRST) {
        nd = L.back().front();
        L.back().pop_front();
    }
    else if (ruleLayer.back() == MOST_FRACTIONAL) {
        std::list<Node*>::iterator ndd, nd2;
        double minProp = 2, nodeProp;
        for (ndd = L.back().begin(); ndd != L.back().end(); ndd++) {
            nodeProp = (*ndd)->getScore(); // PercentageIntegralityLB
            if (nodeProp <= minProp) {
                minProp = nodeProp;
                nd2 = ndd;
            }
        }
        nd = *nd2;
        L.back().erase(nd2);
    }
    else if (ruleLayer.back() == BEST_BOUND_WS || ruleLayer.back() == BEST_BOUND_GAP) { //  || ruleLayer.back() == BEST_BOUND_MAXMIN_GAP

        if (ruleLayer.back() == BEST_BOUND_GAP) { //  && U->wasUpdated()
            double gapSnapshot = -1;
            do {
                nd = heapL.top();
                heapL.pop();
                gapSnapshot = nd->getScore();

                if (P->gapMeasure == LB_SHIFT) nd->computeGap();
                else if (P->gapMeasure == HAUSDORFF) nd->computeGap2();

                //std::cout << gapSnapshot << " vs " << nd->getScore() << "\n";
                heapL.push(nd);
            } while (abs(gapSnapshot - nd->getScore()) >= 0.0001);
        }


        nd = heapL.top();
        //std::cout << "score -> " << nd->getScore() << "\n";
        //if (nd->getScore() >= -0.5) showScoreList();
        heapL.pop();
        lastShuffle++;

        //std::cout << "\n score = " << nd->getScore() << "\n";
        //std::cout << "      -> size : " << heapL.size() + 1 << "";
    }
    else if (ruleLayer.back() == BEST_BOUND_MAXMIN_GAP) {

        std::list<Node*>::iterator n1, n2;
        double maxScore = -1000000, currentScore;

        // recompute scores
        //std::cout << "\n updating node costs: \n";
        for (n1 = L.back().begin(); n1 != L.back().end(); n1++) {
            //std::cout << (*n1)->getScore() << " --> ";
            (*n1)->computeMaxMinGapScore();
            //std::cout << (*n1)->getScore() << "\n";
        }

        // get node with minimal score
        for (n1 = L.back().begin(); n1 != L.back().end(); n1++) {
            currentScore = (*n1)->getScore();
            //std::cout << currentScore << " is tested... \n";
            if (currentScore >= maxScore) {
                maxScore = currentScore;
                n2 = n1;
            }
        }

        nd = *n2;
        L.back().erase(n2);
        //std::cout << "\n score = " << nd->getScore() << "\n";
    }
    else {
        throw std::string("Error: undefined rule for the current layer of nodes.");
    }
    
    // compute the new rule, if relevant
    if (P->nodeSelection == HYBRID) {
        double pctInt = nd->getPercentageIntegralityLB();
        //double pctFeas = nd->getPercentageFeasibilityLB();
        if (pctInt != -1) {
            if (pctInt > threshold) {
                currentNodeRule = BREADTH_FIRST;
            }
            else {
                currentNodeRule = DEPTH_FIRST;
            }
        }
    }
    else {
        currentNodeRule = P->nodeSelection;
    }

    return nd;
}

/* Add a layer to the tree, i.e. create a new sub-tree with a new exploration rule.
 */
void TreeManager::addLayer(int rule) {
    L.push_back(std::list<Node*>(0));
    ruleLayer.push_back(rule);
    nbLayers++;
    //std::cout << nbLayers << " layers\n";
    //std::cout << "switch to " << rule << "\n";
}

/* Add a new node to the tree.
 *
 * \param Node* nd. A pointer to the new node added.
 */
void TreeManager::pushNode(Node* nd) {

    // if we observe a change in the rule, we add a new layer
    if (currentNodeRule != ruleLayer.back()) {
        addLayer(currentNodeRule);
    }

    // add the node to the last layer.
    if (P->nodeSelection == BEST_BOUND_WS || P->nodeSelection == BEST_BOUND_GAP) { //  || P->nodeSelection == BEST_BOUND_MAXMIN_GAP
        heapL.push(nd);
    }
    else {
        L.back().push_back(nd);
    }
    //if (ruleLayer.back() == BEST_BOUND_WS) push_heap(L.back().begin(), L.back().end());
}

/* Check whether the tree is empty, i.e. there is no node to explore remaining. Remove the last empty layers.
 *
 * \return true if L is empty.
 */
bool TreeManager::isEmpty() {

    while (L.size() != 0 && L.back().size() == 0) {
        L.pop_back();
        nbLayers--;
        //std::cout << nbLayers << " layers\n";
    }

    return (L.size() == 0) || ((P->nodeSelection == BEST_BOUND_WS || P->nodeSelection == BEST_BOUND_MAXMIN_GAP || P->nodeSelection == BEST_BOUND_GAP) && heapL.empty());
}

/* RELEVANT FOR BEST_BOUND_WS NODE SEL ONLY
 * Recreate the priority list after the score was changed
 */
void TreeManager::updateScoreList() {

    std::priority_queue<Node*, std::vector<Node*>, CompNodePtrs> newList;
    newList = std::priority_queue<Node*, std::vector<Node*>, CompNodePtrs>();
    Node* nd;

    //srand(time(0));

    // compute new weight (random)

    int p = P->searchDir.size();
    if (true) {
        std::vector<double> rng(p);
        double sum = 0;
        for (int k = 0; k < p; k++) {
            rng[k] = rand();
            sum += rng[k];
        }
        for (int k = 0; k < p; k++) rng[k] /= sum;

        P->selDir = rng;
        std::cout << " \n New search direction : ";
        for (int k = 0; k < p; k++) std::cout << rng[k] << "  ";
        std::cout << "\n";
    }

    while (!heapL.empty()) {
        nd = heapL.top();
        heapL.pop();

        nd->computeWsScore();

        newList.push(nd);
    }

    heapL = newList;
}

/* RELEVANT FOR BEST_BOUND_WS NODE SEL ONLY
 * Recreate the priority list after the score was changed
 *
 * \param l, vector of double. The new weight vector used as reference for node selection.
 */
void TreeManager::updateScoreList(UpperBoundSet* U) {

    std::priority_queue<Node*, std::vector<Node*>, CompNodePtrs> newList;
    newList = std::priority_queue<Node*, std::vector<Node*>, CompNodePtrs>();
    Node* nd;

    //srand(time(0));

    // compute furthest lub


    while (!heapL.empty()) {
        nd = heapL.top();
        heapL.pop();

        nd->computeWsScore(); // if needed, toDo: add param s to check if node in slub of u?

        newList.push(nd);
    }

    heapL = newList;

}

/* Update the gap computations, and the list of nodes
*/
void TreeManager::updateGap() {

    //std::cout << "\n\n ------- New shuffle -------\n\n";

    std::list<Node*> temp(0);
    Node* nd;
    double maxGap = -1;

    if (lastShuffle / 1 >= 0) { // update only every x iterations at most

        while (!heapL.empty()) {
            nd = heapL.top();
            heapL.pop();

            double oldGap = nd->getGap();
            if (U->checkDiscardedLubs(nd->getGapingLub())) { //true || 
                if (P->gapMeasure == LB_SHIFT) nd->computeGap();
                else if (P->gapMeasure == HAUSDORFF) nd->computeGap2();
                else throw std::string("Error: no gap measure associated chosen.\n");
            }
            //if (nd->getGap() > oldGap) std::cout << "pb !!!\n";
            if (maxGap < nd->getGap()) maxGap = nd->getGap();
            //std::cout << " node gap : " << nd->getGap() << "\n";

            temp.push_back(nd);
        }

        while (!temp.empty()) {
            nd = temp.front();
            temp.pop_front();

            heapL.push(nd);
        }

        globalGap = maxGap;
        U->clearDiscardedLubs();
        lastShuffle = 0;
        //std::cout << " size of the heap : " << heapL.size() << "\n";
        //std::cout << "    !!! MAX GAP : " << globalGap << " !!!\n";

    }
}

/* Update the gap computations, and the list of nodes
*/
void TreeManager::updateGapFinal() {

    //std::cout << "\n\n ------- New shuffle -------\n\n";

    std::list<Node*> temp(0);
    Node* nd;
    double maxGap = -1;

    //if (lastShuffle / 1 >= 1) { // update only every x iterations at most

        while (!heapL.empty()) {
            nd = heapL.top();
            heapL.pop();

            if (P->gapMeasure == LB_SHIFT) nd->computeGap();
            else if (P->gapMeasure == HAUSDORFF) nd->computeGap2();
            else throw std::string("Error: no gap measure associated chosen.\n");

            //std::cout << nd->getGap() << "\n\n";
            if (maxGap < nd->getGap()) maxGap = nd->getGap();
            temp.push_back(nd);
        }

        while (!temp.empty()) {
            nd = temp.front();
            temp.pop_front();

            heapL.push(nd);
        }

        globalGap = maxGap;
        U->clearDiscardedLubs();
        lastShuffle = 0;
        //std::cout << " size of the heap : " << heapL.size() << "\n";
        //std::cout << "    !!! MAX GAP : " << globalGap << " !!!\n";
        //std::cout << " Final gap: " << globalGap << "\n";

    //}
}


void TreeManager::updateGap(double val) {
    globalGap = val;
}

double TreeManager::getGlobalGap() {
    return globalGap;
}


void TreeManager::showScoreList() {

    std::list<Node*> temp(0);
    Node* nd;

    std::cout << " List scores: ";

    while (!heapL.empty()) {
        nd = heapL.top();
        heapL.pop();

        std::cout << nd->getGap() << " ";

        temp.push_back(nd);
    }

    while (!temp.empty()) {
        nd = temp.front();
        temp.pop_front();

        heapL.push(nd);
    }
    std::cout << "\n";
    //std::cout << "\n";
}

void TreeManager::resetScores(std::list<GlobalHyperplane>* G, std::list<Node*>* tempStorage) {

    Node* nd;
    double ws;

    for (std::list<GlobalHyperplane>::iterator h = G->begin(); h != G->end(); h++) {
        h->setRHS(10000000);
    }

    while (!heapL.empty()) {

        nd = heapL.top();
        heapL.pop();

        for (std::list<GlobalHyperplane>::iterator h = G->begin(); h != G->end(); h++) {
            ws = nd->getWsScore(h->getNormalVector());
            if (ws < h->getRHS()) {
                h->setRHS(ws);
                //std::cout << " hpp ";
                //h->print();
                //std::cout << " get d = " << ws << "\n";
            }
        }

        tempStorage->push_front(nd);
    }
}


void TreeManager::refillTree(std::list<Node*>* tempStorage, std::vector<double>& l) {

    while (!tempStorage->empty()) {

        tempStorage->front()->computeWsScore(P->selDir); // l
        heapL.push(tempStorage->front());
        tempStorage->pop_front();

    }

}


int TreeManager::size() {
    return heapL.size();
}



bool CompNodePtrs::operator()(const Node* n1, const Node* n2) const {
    return n1->operator>(*n2);
}