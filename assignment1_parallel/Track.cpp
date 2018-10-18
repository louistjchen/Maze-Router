/*
 * Track.cpp
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#include "Track.h"

Track::Track() {

	m_alignType = -1;
	m_xCoord = -1;
	m_yCoord = -1;
	m_trackIndex = -1;

	m_occupied = false;
	m_label = -1;
}

Track::Track(int alignType, int xCoord, int yCoord, int trackIndex) {

	m_alignType = alignType;
	m_xCoord = xCoord;
	m_yCoord = yCoord;
	m_trackIndex = trackIndex;

	m_occupied = false;
	m_label = -1;
}

Track::~Track() {}

int Track::getTrackInfo(int index) {

	switch(index) {
	case 1:	return m_alignType;
	case 2: return m_xCoord;
	case 3: return m_yCoord;
	case 4: return m_trackIndex;
	default:
		std::cout << "[ERROR Track::getTrackInfo] Invalid index: " << index << std::endl;
		break;
	}
	return -1;
}

void Track::setOccupied(bool occupied) {

	m_occupied = occupied;
}

bool Track::ifOccupied() {

	return m_occupied;
}

void Track::setLabel(int label) {

	m_label = label;
}

int Track::getLabel() {

	return m_label;
}

void Track::printTrack() {

	std::cout << "(";
	std::cout << m_alignType;
	std::cout << "," << m_xCoord;
	std::cout << "," << m_yCoord;
	std::cout << "," << m_trackIndex;
	std::cout << ")";
}
