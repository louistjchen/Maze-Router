/*
 * Track.h
 *
 *  Created on: Sep 26, 2018
 *      Author: chentuju
 */

#ifndef TRACK_H_
#define TRACK_H_

#include <iostream>

class Track {

public:
	Track();
	Track(int alignType, int xCoord, int yCoord, int trackIndex);
	~Track();

	// functional methods
	int getTrackInfo(int index);
	void setOccupied(bool occupied);
	bool ifOccupied();
	void setLabel(int label);
	int getLabel();

	// debugging methods
	void printTrack();

private:
	// basic track info
	int m_alignType; // either horizontal or vertical
	int m_xCoord;
	int m_yCoord;
	int m_trackIndex;

	// track flags
	bool m_occupied;	// true - occupied for certain path; false - free to use
	int m_label; 		// normally 0 to target, unlabeled -1, target -2, real target -3
};

#endif /* TRACK_H_ */
