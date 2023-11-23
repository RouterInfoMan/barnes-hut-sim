CFLAGS= -g -Wall -lsfml-graphics -lsfml-window -lsfml-system
SRC = ./src/main.cpp ./src/physics/physics.cpp ./src/physics/body.cpp ./src/physics/utils.cpp ./src/appengine.cpp ./src/threadpool.cpp
OUT = app

main: clean
	g++ $(SRC) $(CFLAGS) -o $(OUT)

clean: 
	rm -f $(OUT)
	rm -f ./src/*.o ./src/physics/*.o
