/*
 * main.cpp
 *
 *  Created on: Oct 10, 2018
 *      Author: chentuju
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <sys/time.h>
#include "Router.h"

using namespace std;

struct timeval t1_start;
struct timeval t1_end;
struct timeval t2_start;
struct timeval t2_end;
struct timeval t3_start;
struct timeval t3_end;
struct timeval t4_start;
struct timeval t4_end;

void thread1(char *inputFile, bool multiLoadOpt, int seed) {

    Router *router = new Router(multiLoadOpt, seed);
    router->parseInputFile(inputFile);
    router->constructTrackMap();
    gettimeofday(&t1_start, NULL);
    router->startRouter();
    gettimeofday(&t1_end, NULL);
    //router->displayNumTrackOccupied();
    //router->displayMinTrackWidth();
    delete router;
}

void thread2(char *inputFile, bool multiLoadOpt, int seed) {
    Router *router = new Router(multiLoadOpt, seed);
    router->parseInputFile(inputFile);
    router->constructTrackMap();
    gettimeofday(&t2_start, NULL);
    router->startRouter();
    gettimeofday(&t2_end, NULL);
    //router->displayNumTrackOccupied();
    //router->displayMinTrackWidth();
    delete router;
}

void thread3(char *inputFile, bool multiLoadOpt, int seed) {
    Router *router = new Router(multiLoadOpt, seed);
    router->parseInputFile(inputFile);
    router->constructTrackMap();
    gettimeofday(&t3_start, NULL);
    router->startRouter();
    gettimeofday(&t3_end, NULL);
    //router->displayNumTrackOccupied();
    //router->displayMinTrackWidth();
    delete router;
}

void thread4(char *inputFile, bool multiLoadOpt, int seed) {
    Router *router = new Router(multiLoadOpt, seed);
    router->parseInputFile(inputFile);
    router->constructTrackMap();
    gettimeofday(&t4_start, NULL);
    router->startRouter();
    gettimeofday(&t4_end, NULL);
    //router->displayNumTrackOccupied();
    //router->displayMinTrackWidth();
    delete router;
}

int main(int argc, char** argv) {

    if(argc == 1) {
        cout << "No circuit file specify!" << endl;
        return 0;
    }

    thread t1(thread1, argv[1], false, 0);
    thread t2(thread2, argv[1], false, 1);
    thread t3(thread3, argv[1], true, 0);
    thread t4(thread4, argv[1], true, 1);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    long int t1_ms = (t1_end.tv_sec*1000 + t1_end.tv_usec/1000) - 
		(t1_start.tv_sec*1000 + t1_start.tv_usec/1000);
    long int t2_ms = (t2_end.tv_sec*1000 + t2_end.tv_usec/1000) - 
		(t2_start.tv_sec*1000 + t2_start.tv_usec/1000);
    long int t3_ms = (t3_end.tv_sec*1000 + t3_end.tv_usec/1000) - 
		(t3_start.tv_sec*1000 + t3_start.tv_usec/1000);
    long int t4_ms = (t4_end.tv_sec*1000 + t4_end.tv_usec/1000) - 
		(t4_start.tv_sec*1000 + t4_start.tv_usec/1000);

    cout << endl << endl;
    cout << "Thread 1 took " << t1_ms << "ms to finish routing." << endl;
    cout << "Thread 2 took " << t2_ms << "ms to finish routing." << endl;
    cout << "Thread 3 took " << t3_ms << "ms to finish routing." << endl;
    cout << "Thread 4 took " << t4_ms << "ms to finish routing." << endl;

    return 0;
}
