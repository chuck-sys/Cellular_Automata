/*
 * Rulestring.h
 *
 * Contains all the rules for the cellular automata
 *
 * Format:
 *         S/B
 *         S = Survive
 *         B = Birth
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

vector<int> GameofLife_RS {2, 3, -1, 3};
vector<int> HighLife_RS {2, 3, -1, 3, 6};
vector<int> Maze_RS {1, 2, 3, 4, 5, -1, 3};
vector<int> Mazectric_RS {1, 2, 3, 4, -1, 3};
vector<int> Replicator_RS {1, 3, 5, 7, -1, 1, 3, 5, 7};

// All the rulesets
vector<vector<int>> All_RS = {
    GameofLife_RS, HighLife_RS, Maze_RS, Mazectric_RS,
    Replicator_RS
};

#endif /* SRC_RULESTRING_H_ */
