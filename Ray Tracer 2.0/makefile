Ray Tracer: main.o
	g++ main.o -ggdb -o main -fopenmp -lgdi32 -lmingw32

main.o: main.cpp camera.h
	g++ -c main.cpp -fopenmp

clean:
	rm -f main.o
