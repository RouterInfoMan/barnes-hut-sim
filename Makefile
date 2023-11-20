CFLAGS= -g -Wall -Werror -lsfml-graphics -lsfml-window -lsfml-system
SRC = ./src/main.cpp ./src/physics/physics.cpp ./src/physics/body.cpp ./src/physics/utils.cpp ./src/appengine.cpp
OUT = app

main: clean
	g++ $(SRC) $(CFLAGS) -o $(OUT)

clean: 
	rm -f $(OUT)
	rm -f ./src/*.o ./src/physics/*.o
