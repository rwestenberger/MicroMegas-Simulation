INCDIR = $(GARFIELD_HOME)/Include
HEEDDIR = $(GARFIELD_HOME)/Heed
LIBDIR = $(GARFIELD_HOME)/Library
PYTHONPATH=..:$PYTHONPATH

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-long-long \
	`root-config --cflags` \
	-O3 -fno-common -c \
	-I$(INCDIR) -I$(HEEDDIR) \
	-g

LDFLAGS = `root-config --glibs` -lGeom -lgfortran -lm \
	-L$(LIBDIR) -lGarfield \
	-g

drift: drift.cpp
	cog.py -r drift.cpp
	$(CXX) $(CFLAGS) drift.cpp
	$(CXX) -o drift drift.o $(LDFLAGS)
	rm drift.o
