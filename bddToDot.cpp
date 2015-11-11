#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <list>

#ifdef USE_CUDD

// This is for those who wish to test their BDD package with CUDD

#include "cuddObj.hh"
typedef Cudd bddMgr;

#else
#include "bddObj.h"
#endif

////////////////////////////////////////////////////////////////////////
// NOTE:
//
// There is no need for the variable numVars in bdd_travers or bddToDot.
// It only exists so the function does not run into an infinite loop
// on a bad BDD package implementation.  The next version should remove
// this variable.
////////////////////////////////////////////////////////////////////////

/* stuct bddID
 *
 * The structure is meant to associate a BDD with an instance number on 
 * its variable in the graph.  This is used to create a hash table of 
 * nodes in the dot output to prevent printing creating different
 * nodes for common subformulas.
 */

struct bddID {
  BDD f;
  int instance;

  bool operator==(const bddID& g) const {
    BDD a(f);
    BDD b(g.f);
    return (a == b);
  }

  bddID(const BDD& g, int i) : f(g), instance(i) {};
  bddID(const bddID& b) : f(b.f), instance(b.instance) {};

};


/* bddHash
 *
 * This is the hash table to keep track of common subformulas in printing
 * BDDs
 */

using std::find;
using std::list;
typedef list<struct bddID> bddHash;


/* void
 * bddTraverse
 *
 * INPUT
 *
 *       bddMgr: the BDD manager for the f
 *            f: the function to print to a dot file
 *   currentVar: the index of the current variable in the total
 *               order imposed by '<' (assumes '<' defined on integers.
 *       fp_dot: the output file
 *     instance: an array of instance indexes the size of numVars to 
 *               differentiate functions with same root variable but 
 *               different functions.
 *        debug: output debug information if nonzero
 *      verbose: verbose output if nonzero
 *
 * OUTPUT
 *
 *   none
 *
 * FUNCTION
 *
 *   Recursively traverse the BDD f to create a DOT file.
 *
 * ASSUMPTIONS;
 *
 *   1) fp_dot: valid file pointer and writes never fail.
 *   2) bddMgr is the correct manager for f
 *   3) instance is at least  numVars long!
 *   4) There are numVars or fewer variables in the BDD.
 */

void
bddTraverse(bddMgr& mgr, 
	    BDD& f, 
	    int currentVar, 
	    int numVars, 
	    FILE* fp_dot,
	    int *instance,
	    bddHash& h,
	    int debug, 
	    int verbose) {

  /* nextVar is used to find the next variable that the function f
   * depends on following the default variable ordering.
   */

  int nextVar = currentVar;
  int copyNextVar = nextVar;

  bddHash::const_iterator iter;

  /* x is the BDD for variable variables (currentVar or nextVar).
   * cofactor is the positive or negative cofactor with respect to x.
   */

  BDD x, cofactor;
  
  /* 1) Output this node
   * 
   *    NOTE: we increment the instance of currentVar here, so future
   *    references need to subtract 1 to get correct reference.
   */
  
  //  The circle is the default shape, and for some reason, on
  //  other versions of Graphviz, it causes warnings.
  //
  //  fprintf(fp_dot, "v%di%d [style=circle]\n", currentVar,
  //  instance[currentVar]++);

  fprintf(fp_dot, "v%di%d \n", currentVar, instance[currentVar]++);

  if (debug)
    printf("Current variable is v%di%d\n", currentVar, instance[currentVar]-1);
  
  /* 2) Find the next dependent variable for the positive cofactor function f.
   *    with respect to x.
   *    NOTE: if the cofactor with the variable leaves the function
   *    unchanged, then the function does not rely on the variable.
   */

  x = mgr.bddVar(currentVar);
  cofactor = f.Restrict(x);
  while (++nextVar < numVars && 
	 cofactor.Restrict(mgr.bddVar(nextVar)) == cofactor);

  /* 3) Compute the positive cofactor of f with respect to x.
   *    There are 3 cases to consider in the cofactor:
   *
   *       a) the cofactor leads to a terminal;
   *       b) the cofactor is a subformula thate
   *          we have seen before; or
   *       b) the cofactor leads to another function that
   *          we have yet to see.
   *
   *    If the cofactor is a terminal, then all we need to do 
   *    is output the connection to the terminal.  
   *    If the cofactor is a formulat that we have already seen,
   *    then we can skip the recursive call, and create an
   *    edge to the existing node in the dot graph.  If
   *    the cofactor leads to a function, then we need
   *    to make the recursive call on the new function, and we need
   *    to add the function to our hash table h.
   */

  /* From this point forward, x is the BDD for the current variable.
   */


  if (debug)
    printf("Positive cofactor %d: ", currentVar);
  if (cofactor == mgr.bddOne()) {

    if (debug)
      printf("bddOne\n");
    fprintf(fp_dot, "v%di%d -> One\n", 
	    currentVar, instance[currentVar]-1);
  }
  else if (cofactor == mgr.bddZero()) {

    if (debug)
      printf("bddZero\n");
    fprintf(fp_dot, "v%di%d -> Zero\n", 
	    currentVar, instance[currentVar]-1);
  }

  /* We need to return if we are not at a terminal and
   * we have run out of variables to check.
   */


  else if (nextVar == numVars) {
	  return;
  }

  // egm +++ 10/13/04
  // 
  // under development: prevents the printing of duplicate subtress.

  else if ((iter = find(h.begin(), h.end(), bddID(cofactor,0))) != h.end()) {
    if (debug)
      printf("hash hit v%di%d\n", nextVar, instance[nextVar]);
    fprintf(fp_dot, "v%di%d -> v%di%d\n", 
	    currentVar, instance[currentVar] - 1, 
	    nextVar, (*iter).instance);
  }

  else {
    if (debug) {
      printf("bddTraverse v%di%d\n", nextVar, instance[nextVar]);
    }
    fprintf(fp_dot, "v%di%d -> v%di%d\n", 
	    currentVar, instance[currentVar] - 1, 
	    nextVar, instance[nextVar]);

    /* Add a entry in the cache for the positive cofactor on nextVar.
     */

    h.push_front(bddID(cofactor, instance[nextVar]));

    if (debug) {
      printf("Adding v%di%d to cache\n", nextVar, nextVar[instance]);
    }
    bddTraverse(mgr, cofactor, nextVar, numVars, fp_dot, instance, h,
		debug, verbose); 
  }
  
  /* 4) Compute the negative cofactor of f with respect to the
   *    current variable (currentVar).
   */

  /* This finds the next dependent variable for the negative cofactor.
   * This variable can be different than the postive cofactor depending
   * on the function.
   */

  cofactor = f.Restrict(~x);
  nextVar = copyNextVar;
  while (++nextVar < numVars && 
	 cofactor.Restrict(mgr.bddVar(nextVar)) == cofactor);

  if (debug)
    printf("Negative cofactor %d: ", currentVar);
  if (cofactor == mgr.bddOne()) {
    if (debug)
      printf("bddOne\n");
    fprintf(fp_dot, "v%di%d -> One [style=dashed]\n", 
	    currentVar, instance[currentVar]-1);
  }
  else if (cofactor == mgr.bddZero()) {
    if (debug)
      printf("bddZero\n");
    fprintf(fp_dot, "v%di%d -> Zero [style=dashed]\n", 
	    currentVar, instance[currentVar]-1);
  }

  /* We need to return if we are not at a terminal and
   * we have run out of variables to check.
   */

  else if (nextVar == numVars) {
    return;
  }

  // egm +++ 10/13/04
  // 
  // under development: prevents the printing of duplicate subtress.

  else if ((iter = find(h.begin(), h.end(), bddID(cofactor,0))) != h.end()) {
    if (debug)
      printf("hash hit v%di%d\n", nextVar, instance[nextVar]);
    fprintf(fp_dot, "v%di%d -> v%di%d [style=dashed]\n", 
	    currentVar, instance[currentVar] - 1, 
	    nextVar, (*iter).instance);
  }

  // -----
  else {
    if (debug) {
      printf("bddTraverse v%di%d\n", nextVar, instance[nextVar]);
    }
    fprintf(fp_dot, "v%di%d -> v%di%d [style=dashed]\n", 
	    currentVar, instance[currentVar] - 1,
	    nextVar, instance[nextVar]);

    /* Add a entry in the cache for the negative cofactor on nextVar.
     */

    h.push_front(bddID(cofactor, instance[nextVar]));

    if (debug) {
      printf("Adding v%di%d to cache\n", nextVar, nextVar[instance]);
    }
    bddTraverse(mgr, cofactor, nextVar, numVars, fp_dot, instance, h,
		debug, verbose); 
  }
}



/* void
 * bddToDot
 *
 * INPUT
 *   
 *        mgr: a valid BDD manager 
 *	    f: a function f ownded by the manager
 *    numVars: the number of variables known to the manager
 *     fp_dot: a valid file pointer
 *	debug: output debug information when nonzero
 *    verbose: output verbose information when nonzero
 *
 * OUTPUT
 *
 *   none
 *
 * FUNCTION
 *
 *   Create a valid dot file to display the structure of the BDD f.
 *
 * ASSUMPTIONS
 *
 *   1) fp_dot is a valid file pointer and writes into fp_dot do not
 *      fail (not enforced)
 *   2) mgr is the correct manager for f (not enforced)
 *   3) numVars is nonnegative.
 *   4) There are numVars or less variables in the BDD.
 *
 */

void
bddToDot(bddMgr& mgr, 
	 BDD f, 
	 int numVars, 
	 FILE* fp_dot,
	 int debug = 0, 
	 int verbose = 0) {

  /* This is used to find the first variable the funtion f depends
   * on in the total order '<'
   */

  int firstVar = -1;

  /* Make sure the file pointer is nonNULL
   */

  if (!fp_dot) {
    printf("ERROR: NULL file pointer in bddToDot\n");
    return;
  }

  /* Enforce the nonzero assumption on numVars
   */

  assert(numVars >= 0);

  /* Output the graph header.
   */

  fprintf(fp_dot, 
	  "digraph massive_output {\n");

  /* Check for the base case of someone calling the function
   * with a terminal node.
   */

  if (f == mgr.bddOne() || f == mgr.bddZero()) {
    if (f == mgr.bddOne())
      fprintf(fp_dot, "One  [shape=box,style=solid,color=green]\n");
    if (f == mgr.bddZero())	
      fprintf(fp_dot, "Zero [shape=box,style=solid,color=red]\n");
  }

  /* This is the nonterminal case.  We need to define the nodes
   * for dot, and then make the call to our helper function
   * to traverse the structure of the BDD.
   *
   * NOTE: we need to find the first variable the function
   * depends on in the total order.  This is easily found
   * using the cofactor on each variable.  If the function
   * does not change, then it is independent of the variable.
   * We know it must depend on at least one variable since
   * we checked the base case above.
   */

  else {
    bddHash h;

    /* Alloocate an instance array to differentiate different
     * BDD nodes that use the same variable as roots for different
     * functions.
     */

    int* instance = (int *)calloc(numVars, sizeof(int));
    if (!instance) {
      printf("ERROR: failed to allocate index array in bddToDot\n");
      return;
    }


    /* Find the first dependent variable.
     */
     while (++firstVar < numVars && f.Restrict(mgr.bddVar(firstVar)) == f);

    /* We need to return if we run out of variables to check.
     */
    
    if (firstVar == numVars) goto exit;;

    fprintf(fp_dot, "One  [shape=box,style=solid,color=green]\n");
    fprintf(fp_dot, "Zero [shape=box,style=solid,color=red]\n");

    bddTraverse(mgr, f, firstVar, numVars, fp_dot, instance, h, debug, verbose);
    exit:
    free(instance);
  }

  /* Output the graph footer.
   */

  fprintf(fp_dot, "}\n");
}


