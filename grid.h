//
// Created by Katie Tran on 2019-03-22.
//

#ifndef ASSIGNMENT4_GRID_H
#define ASSIGNMENT4_GRID_H

#include <vector>
#include <map>
#include <stdint.h>

using namespace std;

class StateGrid {
public:
//    // create an "empty" state of 0 (which represents no state). Use for parent of the initial state
//    StateGrid();
    // create a state from an array of coordinates
    explicit StateGrid(int state[2]);
    // create a state from a state ID
    explicit StateGrid(int id);
    // returns the ID of a state
    uint64_t getStateID() const;
    // returns the hash value of a state from its array of values at locations
    uint64_t hash(int state[2]);
    // get value at a location
    int getValue(int axis);
    // assign a value to a coordinate
    void assignValue(int val, int axis);
//    void swap(int location1, int location2);
//    int getLocation(int val);
    // prints out a state
    void display();
private:
    // hash key of state
    uint64_t stateID;
};

bool operator==(const StateGrid &lhs, const StateGrid &rhs);
bool operator<(const StateGrid &lhs, const StateGrid &rhs);
bool operator>(const StateGrid &lhs, const StateGrid &rhs);

namespace std {
    template<>
    struct hash<StateGrid> {
        size_t operator()(StateGrid const &s) const noexcept;
    };
}

class ActionGrid {
public:
    // create an action from a number representing a direction
    explicit ActionGrid(short int dir);
    // returns the direction number
    short int getAction();
private:
    // move direction
    // 0-2 for going to negative y direction
    // 3 & 5 for going left and right
    // 6-8 for going to positive y direction
    short int moveDir;

};

class EnvironmentGrid {
public:
    explicit EnvironmentGrid(int x, int y, vector<int*> *map_ptr);
    // check if location is blocked on map
    bool isBlocked(int x, int y);
    // stores to a vector the possible actions that can be taken from a state
    void getActions(StateGrid &node, vector<ActionGrid> &actions);
    // move a state to a new state according to action
    void applyAction(StateGrid &s, ActionGrid a);
    // returns the cost of an edge between two states
    double edgeCost(StateGrid &node1, StateGrid &node2);
    // generate n different random locations on map
    void placeRandom(int n, vector<int> &coor);
    // generate n different random locations on map not including the coordinates in exclude vector
    void placeRandom(int n, vector<int> &coor, vector<int> &exclude);
    // return number of dimensions of environment
    int getDim();
    // return each dimension
    void getLength(int &x, int &y);
    void getLength(int &x, int &y, int &z);
//    bool UndoAction(StateGrid &s, ActionGrid a);
//    uint64_t GetStateHash(StateGrid &node);
private:
    // pointer to the map
    vector<int*> *map;
    // dimensions of the map
    int xMap, yMap;
};

class HeuristicGrid {
public:
    // constructor. Takes in a vector of heuristic.
    // For zero heuristic (Dijkstra's algorithm), pass in an empty vector
    // If only use default grid heuristic without any differential heuristic, pass in vector<vector<double>>{{0}}
    explicit HeuristicGrid(int heuristic);
    // returns the hcost between two states
    // this is the max hcost of all the heuristics in *hList
    double hCost(StateGrid &node1, StateGrid &node2);
    // return if heuristic is consistent
    bool isConsistent();
private:
    // h == 0 for zero heuristic; h == 1 for default heuristic
    int h;
};

#endif //ASSIGNMENT4_GRID_H
