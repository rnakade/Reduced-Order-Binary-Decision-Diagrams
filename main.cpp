#include <stdlib.h>

//#include "argtable2.h"
#include "bddObj.h"


/* NOTE: the last 2 parameters are optional
 */

extern void
bddToDot(bddMgr& mgr, 
	 BDD f, 
	 int num_vars, 
	 FILE* fp_dot,
	 int debug = 0, 
	 int verbose = 0);

/* The gVerbose and gDebug globals are used to toggle verbose and debug
 * output.  This standard is followed in all projects for cs486.
 */

int gVerbose = 0, gDebug = 0;

/* This is a simple test function.  Please feel free to change it to
 * meet your needs.
 */

void test_BDD(FILE *fp_dot)
{
	  bddMgr mgr (0, 0);
	  BDD a = mgr.bddVar();
	  BDD b = mgr.bddVar();
	  BDD ap = mgr.bddVar();
	  BDD bp = mgr.bddVar();
	  BDD c = mgr.bddVar();
	  BDD cp = mgr.bddVar();
	  BDD d = mgr.bddVar();
	  BDD dp = mgr.bddVar();



	  BDD r = (~a & ~b & ap & bp) | (a & b & ~ap & ~bp) | (a & b & ap & ~bp) |
	    (a & ~b & ap & ~bp);

	  BDD l = (~ap & ~bp) | (ap & ~bp);
	  BDD s = (~a & ~b & ap & bp) | (a & b & ~ap & ~bp) | (c & d & dp & ap) | (d & ~c & ap & ~bp);
	  BDD t = a & b & c & d;

	  if (fp_dot)
	  	  {
	  	    bddToDot(mgr, t, 8, fp_dot, gDebug, gVerbose);
	  	  }
}

/* See:
 *
 * http://students.cs.byu.edu/~cs486ta/handouts/f04/bdd-package.html 
 *
 * For detailed instructions.
 */

int 
main(int argc, char* argv[])
{
  FILE *fp_dot = NULL;
  gVerbose = 1;
  gDebug = 1;
  if (argc > 1)
  {
	  if (gVerbose)
	  {
		  printf("Enabling dump dot\n");
	  }
	  fp_dot = fopen(argv[1], "w");
	  if (!fp_dot)
	  {
		  printf("ERROR: failed to open %s for writing\n", argv[1]);
	  }
  }

  test_BDD(fp_dot);
  if (fp_dot)
	  fclose(fp_dot);
  return 0;
}
