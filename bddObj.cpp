#include "bddObj.h"

/* Global variables to turn on and off different output you embed in your 
 * code.
 */

int g_debug = 0;
int g_verbose = 0;

/************************************************************************
 * MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/

BDD::BDD() {
	v = 0;
	id = 0;
	high = NULL;
	low = NULL;
	ptrMgr = NULL;
}

BDD::BDD(const BDD &from) {
	v = from.v;
	id = from.id;
	high = from.high;
	low = from.low;
	ptrMgr = from.ptrMgr;
}
 
BDD::~BDD() {
}
 
int 
BDD::operator==(const BDD& other) {
	if(id == other.id)
		return 1;
	return 0;
}

BDD& 
BDD::operator=(const BDD& from) {
	v = from.v;
	id = from.id;
	high = from.high;
	low = from.low;
	ptrMgr = from.ptrMgr;
  return *this;
}

BDD 
BDD::operator~() {
	return (Ite(ptrMgr->getZero(), ptrMgr->getOne(), 0) );
}

BDD 
BDD::operator&(const BDD& other) {
	return (Ite(other, ptrMgr->getZero(), 0) );
}

BDD 
BDD::operator|(const BDD& other) {
	return (Ite(ptrMgr->getOne(), other, 0) );
}

BDD
BDD::Restrict(BDD g) {
	   if (isOne() || isZero()) return *this;
	   int varIng = g.getVariable();
	   int varInf = this->getVariable();
 	   BDD high = g.getHigh();
	   BDD low = g.getLow();
	   // Make sure g is a variable or a negated variable
	   assert (high.isOne() || high.isZero());
	   assert (low.isOne() || low.isZero());
	   if (varIng > varInf) {
	      assert(0);
	   }
	   if (varIng == varInf) {
	      if (high.isOne())
	      {
	    	  return this->getHigh();
	      }
	      else
	         return this->getLow();
	   }
	   return *this;
}

BDD BDD::Ite(const BDD& g, const BDD& h, unsigned int limit) const
{
	for(int i = 0; i< ptrMgr->compute_table.size(); i++)
	{
		if(ptrMgr->compute_table[i].firstVar == *this)
		{
			if(ptrMgr->compute_table[i].SecondVar == g)
				if(ptrMgr->compute_table[i].ThirdVar == h)
					return ptrMgr->compute_table[i].Answer;
		}
	}
	compute_table_entries cte;
	cte.firstVar = *this;
	cte.SecondVar = g;
	cte.ThirdVar = h;
	if(this->isOne())
	{
		cte.Answer = g;
		ptrMgr->compute_table.push_back(cte);
      return g;
	}
	if(this->isZero())
	{
		cte.Answer = h;
		ptrMgr->compute_table.push_back(cte);
		return h;
	}
	if (&g == &h)
	{
		cte.Answer = g;
		ptrMgr->compute_table.push_back(cte);
      return g;
	}
	if(g.isOne() && h.isZero())
	{
		cte.Answer = *this;
		ptrMgr->compute_table.push_back(cte);
		return *this;
	}

	int v = getTopVariable(g, h);
	BDD t = basicCofactor(v, *this, 1).Ite(basicCofactor(v, g, 1), basicCofactor(v, h, 1), 0);
	BDD e = basicCofactor(v, *this, 0).Ite(basicCofactor(v, g, 0), basicCofactor(v, h, 0), 0);

   	if (t == e)
   	{
		cte.Answer = e;
		ptrMgr->compute_table.push_back(cte);
      return e;
   	}
   	BDD a = ptrMgr->createNode(v, t, e);
   	cte.Answer = a;
   	ptrMgr->compute_table.push_back(cte);
   	return a;
}

int BDD::isOne() const
{
	if(id == 1)
		return 1;
	return 0;
}
int BDD::isZero() const
{
	if(id == 0)
		return 1;
	return 0;
}
int BDD::getVariable() const
{
	return v;
}

BDD& BDD::getHigh() const
{
	return *high;
}
BDD& BDD::getLow() const
{
	return *low;
}
int BDD::getTopVariable(const BDD& g, const BDD& h) const
{
	if(!(v < 0))
	{
		if(!(g.v < 0))
		{
			if(!(h.v < 0))
			{
				if(v <= g.v && v <= h.v)
					return v;
				else if(g.v <= v && g.v <= h.v)
					return g.v;
				else
					return h.v;
			}
			if(v <= g.v)
				return v;
			else
				return g.v;
		}
		else
		{
			if(!(h.v < 0))
			{
				if(v <= h.v)
					return v;
				else
					return h.v;
			}
			else
				return v;
		}
	}else
	{
		if(!(g.v < 0))
		{
			if(!(h.v < 0))
			{
				if(g.v <= h.v)
					return g.v;
				else
					return h.v;
			}
			else
				return g.v;
		}
		else if(!(h.v < 0))
		{
			return h.v;
		}
	}
	return 0;
}
BDD BDD::basicCofactor(int var, const BDD& x, int pos) const
{
	if (var != x.getVariable())
	  return x;
	return (pos) ? x.getHigh() : x.getLow();
}

BDD bddMgr::createNode(int v, BDD& h, BDD& l)
{
	BDD * a = new BDD();
	a->v = v;
	for(int i = 0; i<unique.size(); i++)
	{
		if(h == *unique[i] || l == *unique[i])
		{
			if(h == *unique[i])
				a->high = unique[i];
			else
				a->low = unique[i];
		}
	}
	a->id = unique.size();
	a->ptrMgr = this;
	this->unique.push_back(a);
	return *a;
}


bddMgr::bddMgr(unsigned int numVars,
	       unsigned int numSlots,
	       unsigned int cacheSize)
{
	BDD tmp;
	verbose = 0;
	debug = 0;
	createbddZero();
	createbddOne();
}
  
bddMgr::bddMgr(bddMgr& x) {
	verbose = x.verbose;
	debug= x.debug;
	for(int i = 0; i<x.unique.size(); i++)
	{
		unique.push_back(x.unique[i]);
	}
}
  
bddMgr::~bddMgr() {
	for(int i = 0; i<unique.size(); i++)
		delete unique[i];
}
  
bddMgr& 
bddMgr::operator=(const bddMgr& right) {
	verbose = right.verbose;
	debug= right.debug;
	for(int i = 0; i<right.unique.size(); i++)
	{
		unique.push_back(right.unique[i]);
	}
  return *this;
}

BDD 
bddMgr::bddVar() {
	BDD * a = new BDD();
	a->v = unique.size() - 2;
	a->high = unique[1];
	a->low = unique[0];
	a->id = unique.size();
	a->ptrMgr = this;
	unique.push_back(a);
  return *a;
}

BDD 
bddMgr::bddVar(int index)
{
	for(int i = 0; i<unique.size(); i++)
	{
		if(unique[i]->v == index)
		{
			if(unique[i]->low->isZero() && unique[i]->high->isOne())
			{
				return *unique[i];
			}
		}
	}
  BDD a = bddVar();
  a.v = index;
  unique[unique.size() - 1]->v = index;
  return a;
}

BDD&
bddMgr::createbddOne() {
		BDD * one = new BDD();
		one->v = -1;
		one->id = 1;
		one->ptrMgr = this;
		unique.push_back(one);
	  return *one;
}

BDD&
bddMgr::createbddZero() {
		BDD * zero = new BDD();
		zero->v = -2;
		zero->id = 0;
		zero->ptrMgr = this;
		unique.push_back(zero);
		 return *zero;
}

BDD
bddMgr::getOne() {
	return *unique[1];
}

BDD
bddMgr::getZero() {
	return *unique[0];
}

BDD
bddMgr::bddOne() {
	return *unique[1];
}

BDD
bddMgr::bddZero() {
	return *unique[0];
}
/************************************************************************
 * END OF MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/

