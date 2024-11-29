# local.mk should define CXX, CXXFLAGS, LD and LDFLAGS;
# it should work with CPLEX
include local.mk

# build options
DEBUG 	= -O0 -g -pg
OPTIM	= -O3

# flags common to all builds
CFLAGS = -std=c++11 -Wall -Werror -pedantic -Wno-sign-compare -Wno-unused

BUILDFLAGS = $(CFLAGS) $(OPTIM)#$(DEBUG)

MAIN  = SYNCmain

all: $(MAIN)

SRC = \
project2.cpp \
BranchAndBound.cpp \
LB2.cpp\
Node.cpp \
solution.cpp \
Model.cpp \
Tree.cpp \
UB.cpp \
Point.cpp \
Hyperplane.cpp \
Lub.cpp \
SLUB.cpp \
GlobalLB.cpp \

OBJ = $(SRC:%.cpp=%.o) 

$(MAIN):  $(OBJ)
	$(LD) $(BUILDFLAGS) $(OBJ) -o $(MAIN) $(LDFLAGS)

%.o:%.cpp *.h
	$(CXX) -c $(BUILDFLAGS) $(CXXFLAGS) $< -o $(<:%.cpp=%.o)

clean:
	rm -f *.o $(MAIN) && chmod -x *.h *.cpp Makefile
