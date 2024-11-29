# CPLEX-section
# lise cluster
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
# CPLEX common directories
CPLEXDIR   = /apps/cplex_studio128/cplex
CONCERTDIR = /apps/cplex_studio128/concert
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
# compilation flags
# normal version
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes
# profiling version with Saturn
#CCOPT = -m32 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -finstrument-functions
CPLEXFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
# linking flags
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -ldl
# end of CPLEX section

#
# Things that _must_ be defined in this filexs
CXX              = g++
CXXFLAGS         = $(CPLEXFLAGS)
LD               = $(CXX)
LDFLAGS          = $(CCLNFLAGS)
