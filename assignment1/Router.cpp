/*
 * Router.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#include "Router.h"

Router::Router() {
    m_gridSize = -1;
    m_trackWidth = -1;

    m_multiLoadOpt = true;
    m_shareSourcePin = false;
    m_shareGoalPath = NULL;
    m_shareTracks = NULL;
}

Router::Router(bool multiLoadOpt) {
    m_gridSize = -1;
    m_trackWidth = -1;

    m_multiLoadOpt = multiLoadOpt;
    m_shareSourcePin = false;
    m_shareGoalPath = NULL;
    m_shareTracks = NULL;
}

Router::~Router() {

	// free GoalPath
	for(unsigned int i=0; i<m_goalPath.size(); i++)
		if(m_goalPath[i]) {
			delete m_goalPath[i];
			m_goalPath[i] = NULL;
		}

	// free TrackMap
	for(unsigned int i=0; i<m_trackMap.size(); i++)
		for(unsigned int j=0; j<m_trackMap[i].size(); j++)
			for(unsigned int k=0; k<m_trackMap[i][j].size(); k++)
				for(unsigned int h=0; h<m_trackMap[i][j][k].size(); h++)
					if(m_trackMap[i][j][k][h]) {
						delete m_trackMap[i][j][k][h];
						m_trackMap[i][j][k][h] = NULL;
					}
}

int Router::getGridSize() {

	return m_gridSize;
}

int Router::getTrackWidth() {

	return m_trackWidth;
}

void Router::parseInputFile(const char *inputFile) {

    std::ifstream file;
    file.open(inputFile, std::ifstream::in);

    char s[8];
    int count = -1;
    GoalPath *goalPath = NULL;

    while(file >> s) {

        int num = atoi(s);
        if(num == -1)
            break;
        if(count == -1)
            m_gridSize = num;
        else if(count == 0)
            m_trackWidth = num;
        else {
        	int index = count % 6;
			if(index == 1)
				goalPath = new GoalPath();
			goalPath->setGoalPathInfo(num, index);
			if(index == 0)
				m_goalPath.push_back(goalPath);
        }
        count++;
    }
    file.close();
}

void Router::constructTrackMap() {

	// define 1D element to be
	// horizontal track # - 0
	// vertical track # - 1
	int size_1D = 2;
	for(int i=0; i<size_1D; i++) {

		std::vector<std::vector<std::vector<Track*> > > element1;
		m_trackMap.push_back(element1);

		// define 2D element to be X coordinate
		// horizontal size - m_trackWidth
		// vertical size - m_trackWidth+1
		int size_2D = (i == 0) ? m_gridSize : m_gridSize+1;
		for(int j=0; j<size_2D; j++) {

			std::vector<std::vector<Track*> > element2;
			m_trackMap[i].push_back(element2);

			// define 3D element to be Y coordinate
			// horizontal size - m_trackWidth+1
			// vertical size - m_trackWidth
			int size_3D = (i == 0) ? m_gridSize+1 : m_gridSize;
			for(int k=0; k<size_3D; k++) {

				std::vector<Track*> element3;
				m_trackMap[i][j].push_back(element3);

				// define 4D element to be the ID of tracks
				int size_4D = m_trackWidth;
				for(int h=0; h<size_4D; h++) {

					Track *track = new Track(i, j, k, h);
					m_trackMap[i][j][k].push_back(track);
				} // end of h
			} // end of k
		} // end of j
	} // end of i
}

void Router::startRouter() {

	int foundUnroute = false;
	int numShuffle = 0;

	do {
		// if found unroute
		if(foundUnroute) {
			resetRouterShuffleGoalPath();
			numShuffle++;
			foundUnroute = false;
		}

		// loop every goal path in m_goalPath
		for(unsigned int i=0; i<m_goalPath.size(); i++) {

			// declare a pointer to current goal path
			GoalPath *goalPath = m_goalPath[i];
//			GoalPath *goalPath = m_goalPath[m_goalPath.size()-i-1];

			// error handling on invalid source/target coordinates and pins
			if(!validGoalPathCoordsPins(goalPath)) {
				// need to add break condition
				break;
			}

			// reset label to -1 for all unoccupied tracks
			relabelUnoccupiedTracks();

			// label -2 for all available segments adjacent to target
			if(labelTargetTracks(goalPath) == 0) {
				std::cout << "[ERROR Router::startRouter] Target pin has no more available adjacent tracks to connect" << std::endl;
				foundUnroute = true;
				break;
			}

			// declare a queue for routing expansion
			std::queue<Track*> q;

			// label 0 for all available segments adjacent to source
			if(labelSourceTracks(goalPath, q)) {
				// source and target shares the same immediate track
				// labelSourceTracks will handle goalPath setup
				continue;
			}

			// start Lee-Moore maze routing algorithm
			bool foundTarget = false;
			Track *currTrack;
			int currLabel;
			while(!q.empty()) {

				// peek next element in expansion list
				currTrack = q.front();
				currLabel = currTrack->getLabel();

				// obtain a vector of tracks adjacent to currTrack
				std::vector<Track*> adjacentTracks;
				getAdjacentTracks(currTrack, adjacentTracks);
				for(unsigned int j=0; j<adjacentTracks.size(); j++) {

					Track *adjacentTrack = adjacentTracks[j];
					if(adjacentTrack->ifOccupied())
						continue;
					int adjacentLabel = adjacentTrack->getLabel();
					if(adjacentLabel == -2) {
						// set this target track label to -3
						adjacentTrack->setLabel(-3);
						foundTarget = true;
						break;
					}
					if(adjacentLabel != -1)
						continue;

					adjacentTrack->setLabel(currLabel+1);
					q.push(adjacentTrack);
				}
				if(foundTarget)
					break;
				q.pop();
			}

			if(foundTarget) {
	//			goalPath->setGoalPathState(1);		// don't do this yet because labels can be unfindable
				updateGoalPath(goalPath, currLabel);
			}
			else {
				goalPath->setGoalPathState(2);
				std::cout << "[ERROR Router::startRouter] Unable to find a path for ";
				goalPath->printGoalPath();
				std::cout << std::endl;
				foundUnroute = true;
				break;
			}
		} // end of m_goalPath loop
	} while(foundUnroute);

	std::cout << "[RESULT Router::startRouter] Number of shuffles on input path order: "
			<< numShuffle << std::endl;
}

void Router::displayNumTrackOccupied() {

	int numTrack = 0;
	int numTotalTrack = m_gridSize * (m_gridSize + 1) * 2 * m_trackWidth;
	for(unsigned int i=0; i<m_goalPath.size(); i++) {
		GoalPath *goalPath = m_goalPath[i];
		std::vector<Track*> *tracks = goalPath->getPaths();
		for(unsigned j=0; j<tracks->size(); j++)
			numTrack++;
	}

	std::cout << "[RESULT Router::displayNumTrackOccupied] "
			<< numTrack << " out of " << numTotalTrack
			<< " available tracks are occupied (average occupation ratio "
			<< (float)100*numTrack/numTotalTrack << "%)" << std::endl;
}

void Router::displayMinTrackWidth() {

	int minTrackWidth = m_trackWidth;
	int minTrackWidthAlignType = -1;
	int minTrackWidthX = -1;
	int minTrackWidthY = -1;

	for(unsigned int i=0; i<m_trackMap.size(); i++)
		for(unsigned int j=0; j<m_trackMap[i].size(); j++)
			for(unsigned int k=0; k<m_trackMap[i][j].size(); k++) {
				int localTrackWidth = m_trackWidth;
				for(unsigned int h=0; h<m_trackMap[i][j][k].size(); h++)
					if(m_trackMap[i][j][k][h]->ifOccupied())
						localTrackWidth--;
				if(localTrackWidth < minTrackWidth) {
					minTrackWidth = localTrackWidth;
					minTrackWidthAlignType = i;
					minTrackWidthX = j;
					minTrackWidthY = k;
				}
			}

	if(minTrackWidthAlignType == 0) {
		std::cout << "[RESULT Router::displayMinTrackWidth] Minimum # of tracks left (highest congestion) " << minTrackWidth
					<< "/" << m_trackWidth << " occurring at horizontal track channel (";
		std::cout << minTrackWidthX << "," << minTrackWidthY << ")" << std::endl;
	}
	else if(minTrackWidthAlignType == 1) {
		std::cout << "[RESULT Router::displayMinTrackWidth] Minimum # of tracks left (highest congestion) " << minTrackWidth
					<< "/" << m_trackWidth << " occurring at vertical track channel (";
		std::cout << minTrackWidthX << "," << minTrackWidthY << ")" << std::endl;
	}
	else {
		std::cout << "[ERROR Router::displayMinTrackWidth] Invalid track channel align type (grid might contain empty paths)"
				<< std::endl;
	}
}

std::vector<GoalPath*> *Router::getGoalPath() {

	return &m_goalPath;
}

bool Router::validGoalPathCoordsPins(GoalPath *goalPath) {

	int sourceX = goalPath->getGoalPathInfo(1);
	int sourceY = goalPath->getGoalPathInfo(2);
	int sourceP = goalPath->getGoalPathInfo(3);
	int targetX = goalPath->getGoalPathInfo(4);
	int targetY = goalPath->getGoalPathInfo(5);
	int targetP = goalPath->getGoalPathInfo(0);

	bool valid = true;
	if(sourceX < 0 || sourceX >= m_gridSize)
		valid = false;
	else if(sourceY < 0 || sourceY >= m_gridSize)
		valid = false;
	else if(sourceP != 5)
		valid = false;
	else if(targetX < 0 || targetX >= m_gridSize)
		valid = false;
	else if(targetY < 0 || targetY >= m_gridSize)
		valid = false;
	else if(targetP < 1 || targetP > 4)
		valid = false;

	if(!valid) {
		std::cout << "[ERROR Router::validGoalPath] The path ";
		printGoalPaths();
		std::cout << " contains invalid source/target coordinates or pins." << std::endl;
	}
	return valid;
}

void Router::relabelUnoccupiedTracks() {

	for(unsigned int i=0; i<m_trackMap.size(); i++)
		for(unsigned int j=0; j<m_trackMap[i].size(); j++)
			for(unsigned int k=0; k<m_trackMap[i][j].size(); k++)
				for(unsigned int h=0; h<m_trackMap[i][j][k].size(); h++) {
					Track *track = m_trackMap[i][j][k][h];
					if(!track->ifOccupied())
						track->setLabel(-1);
				}
}

int Router::labelTargetTracks(GoalPath *goalPath) {

	int targetX = goalPath->getGoalPathInfo(4);
	int targetY = goalPath->getGoalPathInfo(5);
	int targetP = goalPath->getGoalPathInfo(0);

	// push all connected tracks into a vector before labeling them, regardless of label
	std::vector<Track*> targetTracks;
	int mod = (targetP == 1 || targetP == 2) ? 0 : 1;
	for(int i=0; i<m_trackWidth; i++) {
		if(i % 2 == mod) {
			// push north and west sets of tracks
			targetTracks.push_back(m_trackMap[0][targetX][targetY][i]);
			targetTracks.push_back(m_trackMap[1][targetX+1][targetY][i]);
			// push east set of tracks if targetX not on last column
			if(targetX < m_gridSize-1)
				targetTracks.push_back(m_trackMap[0][targetX+1][targetY][i]);
			// push south set of tracks if targetY not on first row
			if(targetY > 0)
				targetTracks.push_back(m_trackMap[1][targetX+1][targetY-1][i]);
		}
	}

	int numLabel = 0;
	// traverse vector to label -2 on the available target tracks
	for(unsigned int i=0; i<targetTracks.size(); i++) {
		Track *track = targetTracks[i];
		if(!track->ifOccupied()) {
			track->setLabel(-2);
			numLabel++;
		}
	}
	return numLabel;
}

bool Router::labelSourceTracks(GoalPath *goalPath, std::queue<Track*> &q) {

	// for same source multiple loads
	// check to see if current goal path shares the same source as previous paths
	if(m_multiLoadOpt) {
		m_shareSourcePin = false;
		m_shareGoalPath = NULL;
		m_shareTracks = NULL;
		for(unsigned int i=0; i<m_goalPath.size(); i++) {
			if(m_goalPath[i]->getGoalPathState() == 1)
				if(m_goalPath[i]->getGoalPathInfo(1) == goalPath->getGoalPathInfo(1) &&
						m_goalPath[i]->getGoalPathInfo(2) == goalPath->getGoalPathInfo(2) &&
						m_goalPath[i]->getGoalPathInfo(3) == goalPath->getGoalPathInfo(3)) {
					m_shareSourcePin = true;
					m_shareGoalPath = m_goalPath[i];
					m_shareTracks = m_shareGoalPath->getPaths();
				}
		}
	}

	int foundTarget = false;
	int sourceX = goalPath->getGoalPathInfo(1);
	int sourceY = goalPath->getGoalPathInfo(2);

	// push all connected tracks into a vector before labeling them, regardless of label
	std::vector<Track*> sourceTracks;
	for(int i=0; i<m_trackWidth; i++) {
		// push north and west sets of tracks
		sourceTracks.push_back(m_trackMap[0][sourceX][sourceY][i]);
		sourceTracks.push_back(m_trackMap[1][sourceX+1][sourceY][i]);
		// push east set of tracks if targetX not on last column
		if(sourceX < m_gridSize-1)
			sourceTracks.push_back(m_trackMap[0][sourceX+1][sourceY][i]);
		// push south set of tracks if targetY not on first row
		if(sourceY > 0)
			sourceTracks.push_back(m_trackMap[1][sourceX+1][sourceY-1][i]);
	}

	// traverse vector to label 0 on the available source tracks
	Track *track;
	for(unsigned int i=0; i<sourceTracks.size(); i++) {
		track = sourceTracks[i];
		if(!track->ifOccupied()) {
			// break if source and target pin shares the same track
			if(track->getLabel() == -2) {
				foundTarget = true;
				break;
			}
			track->setLabel(0);
			q.push(track);
		}
	}

	// update goalPath info if target is found
	if(foundTarget) {
		track->setOccupied(true);
		track->setLabel(-3);
		goalPath->setGoalPathState(1);
		goalPath->emptyGoalPaths();
		goalPath->pushGoalPath(track);

		// for same source multiple loads
		if(m_multiLoadOpt)
			resetShareGoalPath();
	}
	// for same source multiple loads
	// if found, set previous goal path tracks to unoccupied, label with 0
	// push them into queue, and return false to bypass the function
	// IMPORTANT!!! NEED TO LABEL BACK AS OCCUPIED IN UPDATEGOALPATH!!!
	if(m_multiLoadOpt)
		if(m_shareSourcePin) {
			for(unsigned int i=0; i<m_shareTracks->size(); i++) {
				Track *track = m_shareTracks->at(i);
				track->setOccupied(false);
				track->setLabel(0);
				q.push(track);
			}
		}

	return foundTarget;
}

void Router::getAdjacentTracks(Track *track, std::vector<Track*> &adjacentTracks) {

	int alignType = track->getTrackInfo(1);
	int xCoord = track->getTrackInfo(2);
	int yCoord = track->getTrackInfo(3);
	int trackIndex = track->getTrackInfo(4);

	// consider horizontal track
	if(alignType == 0) {
		// horizontal left set
		if(xCoord-1 >= 0)
			adjacentTracks.push_back(m_trackMap[alignType][xCoord-1][yCoord][trackIndex]);
		// horizontal right set
		if(xCoord+1 < m_gridSize)
			adjacentTracks.push_back(m_trackMap[alignType][xCoord+1][yCoord][trackIndex]);
		// vertical 2 bottom sets
		if(yCoord-1 >= 0) {
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord][yCoord-1][trackIndex]);
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord+1][yCoord-1][trackIndex]);
		}
		// vertical 2 top sets
		if(yCoord+1 <= m_gridSize) {
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord][yCoord][trackIndex]);
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord+1][yCoord][trackIndex]);
		}
	}
	// consider vertical track
	else if(alignType == 1){
		// vertical bottom set
		if(yCoord-1 >= 0)
			adjacentTracks.push_back(m_trackMap[alignType][xCoord][yCoord-1][trackIndex]);
		// vertical top set
		if(yCoord+1 < m_gridSize)
			adjacentTracks.push_back(m_trackMap[alignType][xCoord][yCoord+1][trackIndex]);
		// horizontal 2 left sets
		if(xCoord-1 >= 0) {
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord-1][yCoord][trackIndex]);
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord-1][yCoord+1][trackIndex]);
		}
		// horizontal 2 right sets
		if(xCoord+1 <= m_gridSize) {
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord][yCoord][trackIndex]);
			adjacentTracks.push_back(m_trackMap[1-alignType][xCoord][yCoord+1][trackIndex]);
		}
	}
	else {
		std::cout << "[ERROR Router::getAdjacentTracks] Input track ";
		track->printTrack();
		std::cout << " has invalid alignType: " << alignType << std::endl;
	}

	// could possibly do randomization here
//	random_shuffle(adjacentTracks.begin(), adjacentTracks.end());
}

void Router::updateGoalPath(GoalPath *goalPath, int label) {

	// empty m_paths if not empty
	goalPath->emptyGoalPaths();

	int targetX = goalPath->getGoalPathInfo(4);
	int targetY = goalPath->getGoalPathInfo(5);
	int targetP = goalPath->getGoalPathInfo(0);

	// push all target connected tracks into a vector
	std::vector<Track*> targetTracks;
	int mod = (targetP == 1 || targetP == 2) ? 0 : 1;
	for(int i=0; i<m_trackWidth; i++) {
		if(i % 2 == mod) {
			// push north and west sets of tracks
			targetTracks.push_back(m_trackMap[0][targetX][targetY][i]);
			targetTracks.push_back(m_trackMap[1][targetX+1][targetY][i]);
			// push east set of tracks if targetX not on last column
			if(targetX < m_gridSize-1)
				targetTracks.push_back(m_trackMap[0][targetX+1][targetY][i]);
			// push south set of tracks if targetY not on first row
			if(targetY > 0)
				targetTracks.push_back(m_trackMap[1][targetX+1][targetY-1][i]);
		}
	}

	// push target track (labeled -3) onto m_paths
	Track *track;
	bool foundNextTrack = false;
	for(unsigned int i=0; i<targetTracks.size(); i++) {
		track = targetTracks[i];
		if(!track->ifOccupied()) {
			if(track->getLabel() == -3) {
				foundNextTrack = true;
				track->setOccupied(true);
				goalPath->pushGoalPath(track);
				break;
			}
		}
	}
	if(!foundNextTrack) {
		std::cout << "[ERROR Router::updateGoalPath] Entering updateGoalPath without being able to find target track on path ";
		goalPath->printGoalPath();
		std::cout << std::endl;
		return;
	}

	// push the rest tracks (labeled "label") onto m_paths, in descending "label" order
	foundNextTrack = false;
	while(label >= 0) {

		std::vector<Track*> adjacentTracks;
		getAdjacentTracks(track, adjacentTracks);
		for(unsigned int i=0; i<adjacentTracks.size(); i++) {
			Track *adjacentTrack = adjacentTracks[i];
			if(!adjacentTrack->ifOccupied()) {
				if(adjacentTrack->getLabel() == label) {
					foundNextTrack = true;
					adjacentTrack->setOccupied(true);
					goalPath->pushGoalPath(adjacentTrack);
					track = adjacentTrack;
					label--;
					break;
				}
			}
		}

		if(!foundNextTrack) {
			std::cout << "[ERROR Router::updateGoalPath] Entering updateGoalPath without being able to find track ";
			std::cout << "(label: " << label << ") on path ";
			goalPath->printGoalPath();
			std::cout << std::endl;
			return;
		}
	}
	goalPath->setGoalPathState(1);

	// if this goal path shares the same source pin
	// set shared tracks back to occupied and reset data members
	if(m_multiLoadOpt)
		resetShareGoalPath();
}

void Router::resetRouterShuffleGoalPath() {

	// reset m_trackMap
	for(unsigned int i=0; i<m_trackMap.size(); i++)
		for(unsigned int j=0; j<m_trackMap[i].size(); j++)
			for(unsigned int k=0; k<m_trackMap[i][j].size(); k++)
				for(unsigned int h=0; h<m_trackMap[i][j][k].size(); h++) {
					Track *track = m_trackMap[i][j][k][h];
					track->setOccupied(false);
					track->setLabel(-1);
				}

	// reset m_goalPath
	for(unsigned int i=0; i<m_goalPath.size(); i++) {
		GoalPath *goalPath = m_goalPath[i];
		goalPath->setGoalPathState(0);
		goalPath->emptyGoalPaths();
	}

	// shuffle m_goalPath
	random_shuffle(m_goalPath.begin(), m_goalPath.end());
}

void Router::resetShareGoalPath() {

	if(m_shareSourcePin) {
		for(unsigned int i=0; i<m_shareTracks->size(); i++)
			m_shareTracks->at(i)->setOccupied(true);
		m_shareSourcePin = false;
		m_shareGoalPath = NULL;
		m_shareTracks = NULL;
	}
}

void Router::printGoalPaths() {

	std::cout << "[DEBUG Router::printGoalPath] Printing goal path..." << std::endl;

	unsigned int size = m_goalPath.size();
	for(unsigned int i=0; i<size; i++) {
		GoalPath *goalPath = m_goalPath[i];
		std::cout << "\t";
		goalPath->printGoalPath();
		std::cout << std::endl;
	}
}

void Router::printTrackMap() {

	std::cout << "[DEBUG Router::printTrackMap] Printing track map..." << std::endl;

	for(unsigned int i=0; i<m_trackMap.size(); i++)
		for(unsigned int j=0; j<m_trackMap[i].size(); j++)
			for(unsigned int k=0; k<m_trackMap[i][j].size(); k++) {
				for(unsigned int h=0; h<m_trackMap[i][j][k].size(); h++) {
					Track *track = m_trackMap[i][j][k][h];
					std::cout << "\t";
					track->printTrack();
					std::cout << " ";
				}
				std::cout << std::endl;
			}
}
