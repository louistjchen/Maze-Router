/*
 * GoalPath.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#include "GoalPath.h"

GoalPath::GoalPath() {

    m_sourceX = -1;
    m_sourceY = -1;
    m_sourceP = -1;
    m_targetX = -1;
    m_targetY = -1;
    m_targetP = -1;

    m_pathState = 0;
}

GoalPath::~GoalPath() {}

void GoalPath::setGoalPathInfo(int num, int index) {

	switch(index) {
	case 1:	m_sourceX = num; break;
	case 2: m_sourceY = num; break;
	case 3: m_sourceP = num; break;
	case 4: m_targetX = num; break;
	case 5: m_targetY = num; break;
	case 0: m_targetP = num; break;
	default:
		std::cout << "[ERROR GoalPath::setGoalPathInfo] Invalid source/target index: " << index << std::endl;
		break;
	}
}

int GoalPath::getGoalPathInfo(int index) {

	switch(index) {
	case 1:	return m_sourceX;
	case 2: return m_sourceY;
	case 3: return m_sourceP;
	case 4: return m_targetX;
	case 5: return m_targetY;
	case 0: return m_targetP;
	default:
		std::cout << "[ERROR GoalPath::getGoalPathInfo] Invalid source/target index: " << index << std::endl;
		break;
	}
	return -1;
}

void GoalPath::setGoalPathState(int state) {

	m_pathState = state;
}

int GoalPath::getGoalPathState() {

	return m_pathState;
}

void GoalPath::emptyGoalPaths() {

	m_paths.clear();
}

void GoalPath::pushGoalPath(Track *track) {

	m_paths.push_back(track);
}

std::vector<Track*> *GoalPath::getPaths() {

	return &m_paths;
}

void GoalPath::printGoalPath() {

	std::cout << "(";
	std::cout << m_sourceX;
	std::cout << "," << m_sourceY;
	std::cout << "," << m_sourceP;
	std::cout << ") --> (";
	std::cout << m_targetX;
	std::cout << "," << m_targetY;
	std::cout << "," << m_targetP;
	std::cout << ")";

	if(m_pathState == 0)
		std::cout << " --- unsolved";
	else if(m_pathState == 2)
		std::cout << " --- unable to be solved";
	else if(m_pathState == 1) {
		std::cout << " --- solved" << "\t";
		for(unsigned int i=0; i<m_paths.size(); i++) {
			m_paths[i]->printTrack();
			if(i != m_paths.size()-1)
				std::cout << " --> ";
		}
	}
}
