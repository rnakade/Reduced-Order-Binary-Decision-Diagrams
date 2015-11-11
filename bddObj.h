#ifndef _BDDOBJ_
#define _BDDOBJ_

#include <stdio.h>
#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

/************************************************************************
 * DO NOT CHANGE THIS #ifdef; enables testing with CUDD
 ************************************************************************/
#ifdef USE_CUDD

// This is for those who wish to test their BDD package with CUDD

#include "cuddObj.hh"
typedef Cudd bddMgr;

#else

/* g_debug is used to toggle on and off any debug output you embed in your
 * code.  If it is nonzero, then output the debug information.  Otherwise
 * do not output anything.
 */

extern int g_debug;

/* g_verbose is used to toggle on and off any verbose output you embed in
 * your code.  This is not debug output, just more information than nothing.
 */

extern int g_verbose;

/* This is the default size of each bddMgr's unique and compute tables
 * respectively.
 */

static const int BDDMGR_UNIQUE_SLOTS = 1000;
static const int BDDMGR_CACHE_SLOTS = 2000;

/************************************************************************
 * CLASSES YOU NEED TO IMPLEMENT
 ***********************************************************************/

/* BDD
 *
 * Wrapper for pointers in to the bddMgr data structures.  Each
 * BDD needs a hook to know which bddMgr it belongs too.
 *
 * IMPORTANT: you do need to add checks for mismatched managers and
 * empty BDDs that are uninitialized or have no manager from the
 * default constructor.  Correct behavior is to output a error message
 * and exit.
 *
 * The destructor should _not_ delete memory managed by the bddMgr.
 * It only frees memory related to the BDD wrapper around the 
 * accessors into the bddMgr's data structures.
 */

class bddMgr;

class BDD {
  
  friend class bddMgr;

protected:
  
  int v;
  BDD * high;
  BDD * low;
  int id;
  bddMgr* ptrMgr;

public:

  BDD();

  BDD(const BDD &from);

  ~BDD();

  int 
  operator==(const BDD& other);

  BDD& 
  operator=(const BDD& right);
  
  BDD 
  operator~();

  BDD 
  operator&(const BDD& other);

  BDD 
  operator|(const BDD& other);

  BDD
  Restrict(BDD g);

  BDD 
  Ite(const BDD& g, const BDD& h, unsigned int limit = 0) const;

  int isOne(void) const;
  int isZero(void) const;

  int getVariable(void) const;

  BDD& getHigh(void) const;
  BDD& getLow(void) const;

  int getTopVariable(const BDD& g, const BDD& h) const;

  BDD basicCofactor(int v, const BDD& x, int pos) const;
};


/* bddMgr
 *
 * The bddMgr manages the unique and compute tables.  It keeps track
 * of all the bdd variables.  BDDs are just wrappers to protect
 * accessors into the bddMgr data structures (pointer, etc.).
 *
 */

struct compute_table_entries{
	BDD firstVar;
	BDD SecondVar;
	BDD ThirdVar;
	BDD Answer;
};

class bddMgr {

  friend class BDD;

protected:

  int verbose;
  int debug;
  vector<BDD *> unique;
  vector <compute_table_entries> compute_table;

public:

  bddMgr(unsigned int numVars = 0,
	 unsigned int numSlots = BDDMGR_UNIQUE_SLOTS,
	 unsigned int cacheSize = BDDMGR_CACHE_SLOTS);

  /* Makes a complete copy of a bddMgr.  BDDs for x should not
   * work in the copy (i.e., the copy has its own memory etc.)
   */  

  bddMgr(bddMgr& x);
  
  ~bddMgr();
  
  void 
  makeVerbose() {verbose = 1;}
  
  void 
  makeDebug() {debug = 1;};

  /* This should use the copy constructor to make a complete
   * independent memory copy of the bddMgr.
   */

  bddMgr& 
  operator=(const bddMgr& right);

  /* Creates a new variable in this manager.  Variables begin at 0
   * and increase monotonically by 1 with each call to bddVar.
   */

  BDD 
  bddVar();

  /* Returns the BDD for variable index.  If the variable index does
   * not exist, then it creates the variable and returns the BDD.
   * 
   * If index < 0, then fail gracefully.
   */

  BDD 
  bddVar(int index);

  BDD
  bddOne();

  BDD
  bddZero();

  BDD
  getOne();

  BDD
  getZero();

  BDD& createbddOne();
  BDD& createbddZero();

  BDD createNode(int v, BDD& h, BDD& l);

};


/************************************************************************
 * END OF CLASSES YOU NEED TO IMPLELMENT
 ***********************************************************************/
#endif
#endif
