INCDIR = $(GARFIELD_HOME)/Include
HEEDDIR = $(GARFIELD_HOME)/Heed
LIBDIR = $(GARFIELD_HOME)/Library
PYTHONPATH=..:$PYTHONPATH

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-long-long \
	`root-config --cflags` \
	-O3 -fno-common -c \
	-I$(INCDIR) -I$(HEEDDIR)

LDFLAGS = `root-config --glibs` -lGeom -lgfortran -lm \
	-L$(LIBDIR) -lGarfield

all: avalanche

avalanche: avalanche.cpp ../simulation.conf
	cog.py -r avalanche.cpp
	$(CXX) $(CFLAGS) avalanche.cpp
	$(CXX) -o avalanche avalanche.o $(LDFLAGS)
	rm avalanche.o