#pragma once

#include "UB.h"
#include "Model.h"
#include "Lub.h"
#include "Node.h"
#include "Tree.h"
#include "GlobalConstants.h"
#include "Hyperplane.h"
#include <vector>
#include <numeric>

class TreeManager;

class GlobalHyperplane
{
private:
	Parameters* P;
	int dim; //!< dimension of the pb
	std::vector<double> v; //!< normal vector
	double d; //!< rhs of the hyperplane
	//std::list<LocalUpperBound> lubs; // list of lubs such that this is the closest hyperplane
	double gap; //!< largest gap with one of its defining lubs

public:

	GlobalHyperplane();
	GlobalHyperplane(std::vector<double> v, double d, Parameters* P);

	double computeGapDist(LocalUpperBound* u);
	double computeGapEps(LocalUpperBound* u);
	void resetGap();
	std::vector<double> getNormalVector();
	void setRHS(double v);
	double getRHS();
	double getGap();
	void setGap(double v);
	void print();
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class GlobalLB
{
private:
	MathematicalModel* lp;
	Parameters* P;
	std::list<GlobalHyperplane> G;
	double maxGap;
	double sumGap;

public:
	GlobalLB(MathematicalModel* lp, Parameters* P);

	void updateMeasures(UpperBoundSet* U, TreeManager* T);
	std::list<GlobalHyperplane>* getG();
	void addNewHyperplane(Hyperplane* h);
	void reset();
	double getGapMax();
};

