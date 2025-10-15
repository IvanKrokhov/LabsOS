.PHONY: build clean run rebuild test

build:
	g++ -c -fPIC Number.cpp -o Number.o
	ar rcs libNumber.a Number.o
	g++ -c -fPIC Vector.cpp -o Vector.o
	g++ -shared -o libVector.so Vector.o -L. -lNumber
	g++ main.cpp -o main -L. -lNumber -lVector -Wl,-rpath,.

run: build
	@./main

clean:
	rm -f *.o *.a *.so main

rebuild: clean build