/*
 * Router.h
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <queue>
#include "GoalPath.h"
#include "Track.h"

class Router {
public:
    Router();
    Router(bool multiLoadOpt, int seed);
    ~Router();

    // functional methods
    int getGridSize();
    int getTrackWidth();
    void parseInputFile(const char *inputFile);
    void constructTrackMap();
    void startRouter();
    void displayNumTrackOccupied();
    void displayMinTrackWidth();
    std::vector<GoalPath*> *getGoalPath();

    // debugging methods
    void printGoalPaths();
    void printTrackMap();

protected:
    // helper methods
	bool validGoalPathCoordsPins(GoalPath *goalPath);
	void relabelUnoccupiedTracks();
    int labelTargetTracks(GoalPath *goalPath);
    bool labelSourceTracks(GoalPath *goalPath, std::queue<Track*> &q);
    void getAdjacentTracks(Track *track, std::vector<Track*> &adjacentTracks);
    void updateGoalPath(GoalPath *goalPath, int label);
    void resetRouterShuffleGoalPath();
    void resetShareGoalPath();

private:
    int m_gridSize;
    int m_trackWidth;
    std::vector<GoalPath*> m_goalPath;
    std::vector<std::vector<std::vector<std::vector<Track*> > > > m_trackMap;

    // data members that deal with same source pin multiple load pins
    bool m_multiLoadOpt;
    bool m_shareSourcePin;
    GoalPath *m_shareGoalPath;
    std::vector<Track*> *m_shareTracks;

    // for parallelization
    int m_seed;
};

#endif /* ROUTER_H_ */
