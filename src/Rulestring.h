/*
 * Rulestring.h
 *
 * Contains all the rules for the cellular automata
 *
 * Format:
 * 		S/B
 * 		S = Survive
 * 		B = Birth
 * If there are S amount of cells as a it's neighbour, then survival occurs
 * If there are B amount of cells as a it's neighbour, then birth occurs
 *
 * S and B can both be a list of integers from 0-8
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#ifndef SRC_RULESTRING_H_
#define SRC_RULESTRING_H_

#include <vector>

using namespace std;


// #-1	Your own rule
vector<int> Own_RS;

// #0	Game of Life
const vector<int> GameofLife_RS {2, 3, -1, 3};

// #1	HighLife
const vector<int> HighLife_RS {2, 3, -1, 3, 6};

// #2	Maze
const vector<int> Maze_RS {1, 2, 3, 4, 5, -1, 3};

// #3	Mazectric
const vector<int> Mazectric_RS {1, 2, 3, 4, -1, 3};

// #4	Replicator
const vector<int> Replicator_RS {1, 3, 5, 7, -1, 1, 3, 5, 7};


#endif /* SRC_RULESTRING_H_ */
