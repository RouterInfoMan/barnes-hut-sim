CFLAGS= -g -lsfml-graphics -lsfml-window -lsfml-system 
SRC = ./src/main.cpp ./src/physics/physics.cpp ./src/physics/body.cpp ./src/appengine.cpp ./src/threadpool.cpp ./src/physics/cuda.cu
OUT = app

app: clean
	nvcc $(SRC) -o $(OUT) $(CFLAGS)



clean: 
	rm -f $(OUT)
	rm -f *.o ./src/physics/*.o
