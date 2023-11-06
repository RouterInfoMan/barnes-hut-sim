CFLAGS= -g -Wall -Werror -lsfml-graphics -lsfml-window -lsfml-system
SRC = ./src/main.cpp ./src/physics/physics.cpp ./src/physics/Body.cpp
OUT = main

main: clean
	g++ $(SRC) $(CFLAGS) -o $(OUT)

clean: 
	rm -f $(OUT)
	rm -f ./src/*.o ./src/physics/*.o
