CC = g++
CFLAGS = -pthread
OBJS = server.o client.o

all: server client

server: server.o
	$(CC) $< -o $@ $(CFLAGS)

client: client.o
	$(CC) $< -o $@ $(CFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f server client
.PHONY: server client clean fclean
