CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -lrt -lpthread

all: init client destroy

init: init.cpp bank.cpp
	$(CXX) $(CXXFLAGS) init.cpp bank.cpp -o init

client: client.cpp bank.cpp
	$(CXX) $(CXXFLAGS) client.cpp bank.cpp -o client

destroy: destroy.cpp
	$(CXX) $(CXXFLAGS) destroy.cpp -o destroy

clean:
	rm -f init client destroy
