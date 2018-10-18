/*
 * GoalPath.h
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#ifndef GOALPATH_H_
#define GOALPATH_H_

#include <iostream>
#include <vector>
#include "Track.h"

class GoalPath {
public:
	GoalPath();
	~GoalPath();

	// functional methods
	void setGoalPathInfo(int num, int index);
	int getGoalPathInfo(int index);
	void setGoalPathState(int state);
	int getGoalPathState();
	void emptyGoalPaths();
	void pushGoalPath(Track* track);
	std::vector<Track*> *getPaths();

	// debugging methods
	void printGoalPath();

private:
    int m_sourceX;
    int m_sourceY;
    int m_sourceP;
    int m_targetX;
    int m_targetY;
    int m_targetP;

    int m_pathState; // unsolved: 0, solved: 1, unable to be solved: 2
    std::vector<Track*> m_paths;
};

#endif /* GOALPATH_H_ */
