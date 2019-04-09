//
// Created by Katie Tran on 2019-04-07.
//

#ifndef PROJECT_SALESMAN_H
#define PROJECT_SALESMAN_H

//
// Created by Katie Tran on 2019-03-22.
//

#include <vector>
#include <string>
#include <map>
#include <stdint.h>

using namespace std;

class StateSales {
public:
//    // create an "empty" state of 0 (which represents no state). Use for parent of the initial state
//    StateSales();
//    // create a state from an array of coordinates
//    explicit StateSales();
    // create a state from a state ID
    explicit StateSales(int id);
    // returns the vector of a state
    string getStateID() const;
    // returns the number of cities
    int getNoCities() const;
//    // returns the hash value of a state from its array of values at locations
//    uint64_t hash(int state[2]);
    // get value at a location
    int getValue(int loc);
    // assign a value to a coordinate
    void assignValue(char val, int loc);
//    void swap(int location1, int location2);
//    int getLocation(int val);
    // prints out a state
    void display();
private:
    // string of sequence of visited states
    string stateID;
};

bool operator==(const StateSales &lhs, const StateSales &rhs);
bool operator<(const StateSales &lhs, const StateSales &rhs);
bool operator>(const StateSales &lhs, const StateSales &rhs);

namespace std {
    template<>
    struct hash<StateSales> {
        size_t operator()(StateSales const &s) const noexcept;
    };
}

class ActionSales {
public:
    // create an action from a number representing a direction
    explicit ActionSales(char dir);
    // returns the direction number
    char getAction();
private:
    // ID of next state to move to
    char moveTo;

};

class EnvironmentSales {
public:
    explicit EnvironmentSales();
//    // check if location is blocked on map
//    bool isBlocked(int x, int y);
    // stores to a vector the possible actions that can be taken from a state
    void getActions(StateSales &node, vector<ActionSales> &actions);
    // move a state to a new state according to action
    void applyAction(StateSales &s, ActionSales a);
    // returns the cost of an edge between two states
    double edgeCost(StateSales &node1, StateSales &node2);
//    // generate n different random locations on map
//    void placeRandom(int n, vector<int> &coor);
//    // generate n different random locations on map not including the coordinates in exclude vector
//    void placeRandom(int n, vector<int> &coor, vector<int> &exclude);
    // return number of cities
    int getNoCities();
    // return map
    vector<vector<double>> getMap();
//    bool UndoAction(StateSales &s, ActionSales a);
//    uint64_t GetStateHash(StateSales &node);
private:
    // pointer to the map
    vector<vector<double>> map;
};

class HeuristicSales {
public:
    // constructor. Takes in a vector of heuristic.
    // For zero heuristic (Dijkstra's algorithm), pass in an empty vector
    // If only use default Sales heuristic without any differential heuristic, pass in vector<vector<double>>{{0}}
    explicit HeuristicSales(int heuristic, EnvironmentSales environment);
    // returns the hcost between two states
    // this is the max hcost of all the heuristics in *hList
    double hCost(StateSales &node1, StateSales &node2);
    // return if heuristic is consistent
    bool isConsistent();
private:
    // h == 0 for zero heuristic; h == 1 for default heuristic
    int h;
    EnvironmentSales e;
};

#endif //PROJECT_SALESMAN_H
