RM=rm -f
CXXFLAGS=-g -Wall
CXX=g++
P=v
SRCS:=$(wildcard *.cpp)
OBJS:=$(SRCS:.cpp=.o)

all: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(P) $(OBJS)

depend:
	$(RM) .depend
	makedepend -f- -- $(CXXFLAGS) -Y $(SRCS) 2>/dev/null > .depend

clean:
	$(RM) $(OBJS) $(P)


.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

include .depend

