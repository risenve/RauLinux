CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pthread
LDFLAGS = -lrt -pthread

TARGETS = init destroy client
SRC_DIR = .
OBJ_DIR = obj

SOURCES = init.cpp destroy.cpp client.cpp bank.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGETS)

init: $(OBJ_DIR)/init.o $(OBJ_DIR)/bank.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

destroy: $(OBJ_DIR)/destroy.o $(OBJ_DIR)/bank.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: $(OBJ_DIR)/client.o $(OBJ_DIR)/bank.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGETS)
