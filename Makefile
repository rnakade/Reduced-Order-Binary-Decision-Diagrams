# CS 486 
# Brigham Young University

# Build Targets
#
# bddObj.o: object files for kripke parser imlementation
# bdd-test: simple test framework

# If you are going to use the CUDD BDD package for testing, then you need to 
# set the value of CUDD_DEV to 1; otherwise set it to nothing (i.e., leave
# everything after the '=' sign blank
#
# BTW, you can always use make USE_CUDD=1 or make USE_CUDD="" as an alternative.

USE_CUDD	= 1

ifdef USE_CUDD

# NOTE: You need to configure the CUDD variable to point to your 
# install location.  This should be the path to the build directory.
# See the README file for details on how to build and configure CUDD
#
# YOU NEED TO CONFIGURE THIS TO YOUR SYSTEM

CUDD	= /Users/rnakade/Documents/CUDD/cudd-2.4.2

# You should not need to change the rest of these variable definitions
# to use CUDD

 CUDDINC	= -I$(CUDD)/include
 CUDDLIB	= -L$(CUDD)/lib
CUDDLIBS	= -lobj -lcudd -lmtr -lutil -lst -lepd
    OBJS	=  bddToDot.o main.o 
 DEFINES	= -D USE_CUDD

# These are definitions to use your own BDD package.  You should not need
# to change any of these unless I left off an object file.

else

 CUDDINC	= 
 CUDDLIB	= 
    CUDD	= 
CUDDLIBS	= 
    OBJS	= bddToDot.o bddObj.o main.o 
 DEFINES	=

endif

# You may need to change these for your particular environment (i.e.,
# nonUnix based).

CC 		= g++
CFLAGS 		= -g -Wall $(DEFINES)

# Simple name a different target to change default 'make' behavior
# when no target is specified.

DEFAULT		= bdd-test

########################################################################
# BUILD TARGETS
########################################################################

default:	$(DEFAULT)

bddObj.o:	bddObj.cpp bddObj.h
		$(CC) $(CFLAGS) $(CUDDINC) -c $<

%.o:		%.cpp
		$(CC) $(CFLAGS) $(CUDDINC) -c $<

bdd-test:	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o bdd-test $(CUDDLIB) $(CUDDLIBS)

clean:
	rm -f *.o *~ bdd-test
