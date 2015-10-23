# Compiler flags
CFLAGS = -Wall -Wextra -Wno-long-long \
	`root-config --cflags` \
	-O3 -fno-common -c -std=c++11\
	-I$(INCDIR) -I$(HEEDDIR)

# Debug flags
CFLAGS += -g

LDFLAGS = `root-config --glibs` -lGeom -lgfortran -lm
LDFLAGS += -L$(LIBDIR)
LDFLAGS += -g

analysis: analysis.cpp analysis.hpp
	$(CXX) $(CFLAGS) analysis.cpp analysis.hpp
	$(CXX) -o analysis analysis.o $(LDFLAGS)
	rm analysis.o

