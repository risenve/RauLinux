CC = g++
CFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lrt -lpthread
TARGETS = init client destroy

all: $(TARGETS)

init: init.cpp
	$(CC) $(CFLAGS) init.cpp -o init $(LDFLAGS)

client: client.cpp bank.cpp bank_structures.h
	$(CC) $(CFLAGS) client.cpp bank.cpp -o client $(LDFLAGS)

destroy: destroy.cpp
	$(CC) $(CFLAGS) destroy.cpp -o destroy $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
