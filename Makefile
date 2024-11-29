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

SRC_DIR=code
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
GlobalLB.cpp

SRC_P = $(addprefix $(SRC_DIR)/, $(SRC))

OBJ = $(SRC_P:%.cpp=%.o) 

$(MAIN):  $(OBJ)
	$(LD) $(BUILDFLAGS) $(OBJ) -o $(MAIN) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/*.h
	$(CXX) -c $(BUILDFLAGS) $(CXXFLAGS) $< -o $(<:%.cpp=%.o)

clean:
	-rm -f  $(MAIN) $(OBJ) && chmod -x *.h *.cpp Makefile
