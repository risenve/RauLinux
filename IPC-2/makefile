GCC = g++
SRC = main.cpp
NAME = prime-calculator
OBJ = main.o
FLAGS = -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJ)
	@ $(GCC) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ): $(SRC)
	@ $(GCC) $(FLAGS) -c $(SRC) -o $(OBJ)

clean:
	@ rm -rf $(OBJ)


