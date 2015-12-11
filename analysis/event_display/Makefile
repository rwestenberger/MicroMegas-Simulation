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

event_display: event_display.cpp event_display.hpp
	$(CXX) $(CFLAGS) event_display.cpp event_display.hpp
	$(CXX) -o event_display event_display.o $(LDFLAGS)
	rm event_display.o

