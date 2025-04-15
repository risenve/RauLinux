CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra
LDFLAGS := -lpthread

all: init client destroy

init: init.cpp bank_structures.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

client: client.cpp bank_structures.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

destroy: destroy.cpp bank_structures.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f init client destroy

.PHONY: all clean
