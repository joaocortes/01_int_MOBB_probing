#include "GlobalLB.h"


GlobalHyperplane::GlobalHyperplane() : P(NULL), dim(0), v(0), d(0), gap(-1) {}

GlobalHyperplane::GlobalHyperplane(std::vector<double> v, double d, Parameters* P) : P(P), dim(v.size()), v(v), d(d), gap(-1) {}

/* Using euclidian distance to closest hpp
*/

double GlobalHyperplane::computeGapDist(LocalUpperBound* u) {


	// compute the hpp parallel to this and passing through u

	std::vector<double> uu(dim);
	for (int k = 0; k < dim; k++) uu[k] = min(double(u->get_coordinate(k)), 100000.0);
	double du = std::inner_product(v.begin(), v.end(), uu.begin(), 0.0);
	//std::cout << "   -> rhs lub : " << du << "\n";

	// compute the point of this that is the closest to the parallel hpp

	double t = (du - d) / (std::inner_product(v.begin(), v.end(), v.begin(), 0.0));
	std::vector<double> ll(dim);
	for (int k = 0; k < dim; k++) ll[k] = uu[k] - t * v[k];

	// compute the disance between the two points

	double dist = 0.0;
	for (int k = 0; k < dim; k++) dist += pow(uu[k] - ll[k], 2);
	if (t >= 0) dist = sqrt(dist);
	else dist = -sqrt(dist);

	return max(0.0, dist); // gap is 0 if u located below this hpp
}

/* Using eps-dominance
*/

double GlobalHyperplane::computeGapEps(LocalUpperBound* u) {


	// get lub's coordinates

	std::vector<double> uu(dim);
	for (int k = 0; k < dim; k++) uu[k] = min(double(u->get_coordinate(k)), 100000.0);


	// get denominator

	std::vector<double> e(dim, 1);
	double denom = std::inner_product(v.begin(), v.end(), P->selDir.begin(), 0.0); // e.begin()

	// get eps and the new point

	double t = (std::inner_product(v.begin(), v.end(), uu.begin(), 0.0) - d) / denom;

	return max(0.0, t); // gap is 0 if u located below this hpp
}


void GlobalHyperplane::print() {
	std::cout << "( ";
	for (int k = 0; k < dim - 1; k++) {
		std::cout << v[k] << " , ";
	}
	std::cout << v[dim - 1] << " ) = " << d << "\n";
}


void GlobalHyperplane::resetGap() {
	gap = 0;
}


std::vector<double> GlobalHyperplane::getNormalVector() {
	return v;
}


void GlobalHyperplane::setRHS(double v) {
	d = v;
}


double GlobalHyperplane::getRHS() {
	return d;
}


double GlobalHyperplane::getGap() {
	return gap;
}


void GlobalHyperplane::setGap(double v) {
	gap = v;
}



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////



GlobalLB::GlobalLB(MathematicalModel* lp, Parameters* P) : lp(lp), P(P), G(0), maxGap(10000000), sumGap(10000000) {

	std::vector<double> l(lp->get_p(), 0);
	for (int k = 0; k < lp->get_p(); k++) {
		for (int kk = 0; kk < lp->get_p(); kk++) l[kk] = 0;
		l[k] = 1;

		G.push_back(GlobalHyperplane(l, -1000000, P));
	}

	std::vector<double> ll(lp->get_p(), 1 / lp->get_p());
	G.push_back(GlobalHyperplane(ll, -1000000, P));

}

void GlobalLB::updateMeasures(UpperBoundSet* U, TreeManager* T) {

	// compute updated rhs of the hyperplanes

	std::list<Node*> tempStorage(0);
	for (std::list<GlobalHyperplane>::iterator h = G.begin(); h != G.end(); h++) {
		h->resetGap();
	}
	T->resetScores(&G, &tempStorage);

	//std::cout << " \n ---- final d : \n";
	//for (std::list<GlobalHyperplane>::iterator h = G.begin(); h != G.end(); h++) {
		//h->print();
	//}

	// check the gap for each LUB

	std::list<LocalUpperBound>* NU = U->getLubs();
	GlobalHyperplane* closestHpp = &(*G.begin()), * weightHpp = &(*G.begin());
	double g, gapMax = -1;
	for (std::list<LocalUpperBound>::iterator u = NU->begin(); u != NU->end(); u++) {

		// compute closest hpp to the lub u

		double gapMin = 10000000;
		for (std::list<GlobalHyperplane>::iterator hpp = G.begin(); hpp != G.end(); hpp++) {
			//g = hpp->computeGapDist(&(*u));
			g = hpp->computeGapEps(&(*u));
			if (g < gapMin) {
				gapMin = g;
				closestHpp = &(*hpp);
			}
		}

		if (gapMin > closestHpp->getGap()) {
			closestHpp->setGap(gapMin);
		}

		// if this gap is larger than the largest known, its closest hpp will be used as new weight for exploration

		if (gapMin > gapMax) {
			gapMax = gapMin;
			weightHpp = closestHpp; // weights for next exploration is given by normal vector of weightHpp
		}
	}

	maxGap = gapMax;

	// weightHpp or (1,...,1)
	//std::vector<double> l(lp->get_p(), 1 / lp->get_p());
	std::vector<double> l(weightHpp->getNormalVector());
	T->refillTree(&tempStorage, l);
	//P->selDir = l;

	std::cout << " max gap : " << gapMax << "\n";
	//std::cout << " search direction : " ;
	//weightHpp->print();
	//std::cout << "\n";
}


void GlobalLB::reset() {
	G.clear();
}


void GlobalLB::addNewHyperplane(Hyperplane* h) {

	std::vector<double> nv(h->get_dim() + 1);
	for (int k = 0; k <= h->get_dim(); k++) nv[k] = h->get_normalVector(k);

	G.push_back(GlobalHyperplane(nv, h->get_rhs(), P));
}


std::list<GlobalHyperplane>* GlobalLB::getG() {
	return &G;
}


double GlobalLB::getGapMax() {
	return maxGap;
}