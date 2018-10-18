all:
	g++ -c -std=c++0x -o main.o main.cpp
	g++ -c 		  -o Router.o Router.cpp
	g++ -c 		  -o GoalPath.o GoalPath.cpp
	g++ -c		  -o Track.o Track.cpp
	g++ -Wall -pthread -lX11 -o MazeRouterParallel-LouisChen main.o Router.o GoalPath.o Track.o

clean:
	rm -f main.o Router.o GoalPath.o Track.o MazeRouterParallel-LouisChen
