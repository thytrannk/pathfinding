//
// Created by Katie Tran on 2019-01-21.
//

#ifndef ASSIGNMENT1_VOXEL_H
#define ASSIGNMENT1_VOXEL_H

#include <vector>
#include <map>
#include <stdint.h>

using namespace std;

class StateVoxel {
public:
//    // create an "empty" state of 0 (which represents no state). Use for parent of the initial state
//    StateVoxel();
    // create a state from an array of coordinates
    explicit StateVoxel(int state[3]);
    // create a state from a state ID
    explicit StateVoxel(int id);
    // returns the ID of a state
    uint64_t getStateID() const;
    // returns the hash value of a state from its array of values at locations
    uint64_t hash(int state[3]);
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

bool operator==(const StateVoxel &lhs, const StateVoxel &rhs);
bool operator<(const StateVoxel &lhs, const StateVoxel &rhs);
bool operator>(const StateVoxel &lhs, const StateVoxel &rhs);

namespace std {
    template<>
    struct hash<StateVoxel> {
        size_t operator()(StateVoxel const &s) const noexcept;
    };
}

class ActionVoxel {
public:
    // create an action from a number representing a direction
    explicit ActionVoxel(short int dir);
    // returns the direction number
    short int getAction();
private:
    // move direction
    // 0-8 for going up (z+1) in 9 directions
    // 9-17 for going on the same z (13 is never used because it's the original location)
    // 18-26 for going down (z-1) in 9 directions
    short int moveDir;

};

class EnvironmentVoxel {
public:
    explicit EnvironmentVoxel(int x, int y, int z, vector<int*> *map_ptr);
    // check if location is blocked on map
    bool isBlocked(int x, int y, int z);
    // stores to a vector the possible actions that can be taken from a state
    void getActions(StateVoxel &node, vector<ActionVoxel> &actions);
    // move a state to a new state according to action
    void applyAction(StateVoxel &s, ActionVoxel a);
    // returns the cost of an edge between two states
    double edgeCost(StateVoxel &node1, StateVoxel &node2);
    // generate n different random locations on map
    void placeRandom(int n, vector<int> &coor);
    // generate n different random locations on map not including the coordinates in exclude vector
    void placeRandom(int n, vector<int> &coor, vector<int> &exclude);
    // return number of dimensions of environment
    int getDim();
    // return each dimension
    void getLength(int &x, int &y, int &z);
    void getLength(int &x, int &y);
//    bool UndoAction(StateVoxel &s, ActionVoxel a);
//    uint64_t GetStateHash(StateVoxel &node);
private:
    // pointer to the map
    vector<int*> *map;
    // dimensions of the map
    int xMap, yMap, zMap;
};

class HeuristicVoxel {
public:
    // constructor. Takes in a vector of heuristic.
    // For zero heuristic (Dijkstra's algorithm), pass in an empty vector
    // If only use default voxel heuristic without any differential heuristic, pass in vector<vector<double>>{{0}}
    explicit HeuristicVoxel(vector<vector<double>> list, char lookupChar, int x, int y, int z);
    // returns the hcost between two states
    // this is the max hcost of all the heuristics in *hList
    double hCost(StateVoxel &node1, StateVoxel &node2);
    // return if heuristic is consistent
    bool isConsistent();
private:
    // vector of all differential heuristic vectors
    vector<vector<double>> hList;
    // lookup strategy
    char lookup;
    // dimensions of the map
    int xMap, yMap, zMap;
};

#endif //ASSIGNMENT1_VOXEL_H
