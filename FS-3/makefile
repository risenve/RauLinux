GCC = g++
SRC = hacker-rm.cpp
NAME = hacker-rm
OBJ = hacker-rm.o
FLAGS = -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJ)
	@ $(GCC) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ): $(SRC)
	@ $(GCC) $(FLAGS) -c $(SRC) -o $(OBJ)

clean:
	@ rm -rf $(OBJ)


