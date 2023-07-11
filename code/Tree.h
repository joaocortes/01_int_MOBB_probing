#pragma once
#include <stdlib.h>
#include <list>
#include <vector>
#include "Node.h"
#include "GlobalConstants.h"
#include "GlobalLB.h"
#include "UB.h"
#include <queue>

class Node;
class GlobalLB;
class GlobalHyperplane;

struct CompNodePtrs {
	bool operator()(const Node* n1, const Node* n2) const;
};

class TreeManager
{
private:
	Parameters* P; //!< a pointer to the parameters of the algorithm.
	UpperBoundSet* U;
	double threshold; //!< threshold for switching node selection rule
	std::list<std::list<Node*>> L; //!< list including the nodes of the tree remaining to explore. Each list correspond to a sub-tree with a specific rule.
	std::priority_queue<Node*, std::vector<Node*>, CompNodePtrs> heapL; //!< Same role as L, but is stored as a heap. Used for rules with min or max scores.
	//std::list<int> L;
	int nbLayers; //!< number of layers in L, i.e. outer lists.
	std::list<int> ruleLayer; //!< describre the rule used for each layer. See DEPTH_FIRST, BREADTH_FIRST.
	int currentNodeRule; //!< rule of the current node according to the LB set from its father node
	double globalGap; //!< global gap for the tree
	int lastShuffle;

public:

	TreeManager();

	/* Constructor of the Tree, initialized with the root node.
	 *
	 * \param Node* nd. A pointer to the root node.
	 * \param Parameters* P. Parameters of the algorithm, that in particular tells how the tree should be explored.
	 */
	TreeManager(Node* nd, Parameters* param, UpperBoundSet* UB);

	/* Extract the next node to explore. Set currentNode to the extracted node.
	 *
	 * \return a pointer to the next node explored.
	 */
	Node* extractNode();

	/* Add a layer to the tree, i.e. create a new sub-tree with a new exploration rule, specified in the parameters.
	 *
	 * \param int rule. The ID of the rule of the new layer.
	 */
	void addLayer(int rule);

	/* Add a new node to the tree.
	 *
	 * \param Node* nd. A pointer to the new node added.
	 */
	void pushNode(Node* nd);

	/* Check whether the tree is empty, i.e. there is no node to explore remaining. Remove the last empty layers.
	 *
	 * \return true if L is empty.
	 */
	bool isEmpty();

	/* RELEVANT FOR BEST_BOUND_WS NODE SEL ONLY
	 * Recreate the priority list after the score was changed
	 *
	 * \param l, vector of double. The new weight vector used as reference for node selection.
	 */
	void updateScoreList(); // std::vector<double> l

	/* RELEVANT FOR BEST_BOUND_WS NODE SEL ONLY
	 * Recreate the priority list after the score was changed
	 *
	 * \param l, vector of double. The new weight vector used as reference for node selection.
	 */
	void updateScoreList(UpperBoundSet* U);

	/* Update the gap computations, and the list of nodes
	*/
	void updateGap();
	void updateGapFinal();
	void updateGap(double val);
	double getGlobalGap();
	void showScoreList();

	void resetScores(std::list<GlobalHyperplane>* G, std::list<Node*>* tempStorage);
	void refillTree(std::list<Node*>* tempStorage, std::vector<double>& l);

	int size();
};