/*
 * main.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "Router.h"
#include "Display.h"

using namespace std;


int main(int argc, char** argv) {

	bool debugMode = false;
	bool displayMode = true;
	bool multiLoadOpt = true;
    if(argc == 1) {
        cout << "No circuit file specify!" << endl;
        return 0;
    }
    else if(argc > 2) {
    	for(int i=2; i<argc; i++) {
    		if(strcmp(argv[i], "-debugMode=on") == 0)
    			debugMode = true;
    		else if(strcmp(argv[i], "-debugMode=off") == 0)
    			debugMode = false;
    		else if(strcmp(argv[i], "-displayMode=on") == 0)
    			displayMode = true;
    		else if(strcmp(argv[i], "-displayMode=off") == 0)
    			displayMode = false;
    		else if(strcmp(argv[i], "-multiLoadOpt=on") == 0)
    			multiLoadOpt = true;
    		else if(strcmp(argv[i], "-multiLoadOpt=off") == 0)
    			multiLoadOpt = false;
    	}
    }

    cout << "1) Starting constructor." << endl;
    Router router(multiLoadOpt);
    cout << "   Finished constructor." << endl;

    cout << "2) Starting parseInputFile." << endl;
    router.parseInputFile(argv[1]);
    cout << "   Finished parseInputFile." << endl;

    if(debugMode) {
		cout << "2b) Starting printGoalPaths." << endl;
		router.printGoalPaths();
		cout << "    Finished printGoalPaths." << endl;
    }

    cout << "3) Starting constructTrackMap." << endl;
    router.constructTrackMap();
    cout << "   Finished constructTrackMap." << endl;

    if(debugMode) {
		cout << "3b) Starting printTrackMap." << endl;
		router.printTrackMap();
		cout << "    Finished printTrackMap." << endl;
    }

	cout << "4) Starting openDisplay." << endl;
	if(displayMode) openDisplay(&router);
	else cout << "   -displayMode=off disables graphics display." << endl;
	cout << "   Finished openDisplay." << endl;

    cout << "5) Starting startRouter." << endl << "   ";
    router.startRouter();
    cout << "   Finished startRouter." << endl;

    if(debugMode) {
		cout << "5b) Starting printGoalPaths." << endl;
		router.printGoalPaths();
		cout << "    Finished printGoalPaths." << endl;
    }

    cout << "6) Starting displayNumTrackOccupied." << endl << "   ";
    router.displayNumTrackOccupied();
    cout << "   Finished displayNumTrackOccupied." << endl;

    cout << "7) Starting displayMinTrackWidth." << endl << "   ";
    router.displayMinTrackWidth();
    cout << "   Finished displayMinTrackWidth." << endl;

	cout << "8) Starting updateDisplay." << endl;
	if(displayMode) updateDisplay();
	else cout << "   -displayMode=off disables graphics display." << endl;
	cout << "   Finished updateDisplay." << endl;

    return 0;
}
