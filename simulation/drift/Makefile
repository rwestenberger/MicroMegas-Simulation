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

# profiling and debugging flags
CFLAGS = $(CFLAGS) -g -pg -static-libgcc
LDFLAGS = $(LDFLAGS) -g -pg

drift: drift.cpp ../simulation.conf
	cog.py -r drift.cpp
	$(CXX) $(CFLAGS) drift.cpp
	$(CXX) -o drift drift.o $(LDFLAGS)
	rm drift.o
