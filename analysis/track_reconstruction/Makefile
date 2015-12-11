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

track_reco: track_reco.cpp track_reco.hpp
	$(CXX) $(CFLAGS) track_reco.cpp track_reco.hpp
	$(CXX) -o track_reco track_reco.o $(LDFLAGS)
	rm track_reco.o

