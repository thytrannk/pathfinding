//
// Created by Katie Tran on 2019-01-13.
//

#ifndef ASSIGNMENT1_PUZZLE_H
#define ASSIGNMENT1_PUZZLE_H

#include <vector>
#include <map>
#include <stdint.h>

using namespace std;

class StatePuzzle {
public:
//    // create an "empty" state of 0 everywhere (which represents no state). Use for parent of the initial state
//    StatePuzzle();
    // create a state from an array of values at locations
    explicit StatePuzzle(int state[16]);
    // create a state from a state ID
    explicit StatePuzzle(int id);
//    StatePuzzle(int state[16], vector<int> &pattern);
    // returns the ID of a state
    uint64_t getStateID() const;
    // returns the hash value of a state from its array of values at locations
    uint64_t hash(int state[16]);
//    uint32_t hash(int state[16], vector<int> &pattern);
    // get value at a location
    int getValue(int location);
    // swap values at two locations
    void swap(int location1, int location2);
    // returns the location of a value in a state
    int getLocation(int val);
    // prints out a state
    void display();
private:
    // hash key of state
    uint64_t stateID;
    // assign a value to a location of the puzzle
    void assignValue(int val, int location);
};

bool operator==(const StatePuzzle &lhs, const StatePuzzle &rhs);
bool operator<(const StatePuzzle &lhs, const StatePuzzle &rhs);
bool operator>(const StatePuzzle &lhs, const StatePuzzle &rhs);

namespace std {
    template<>
    struct hash<StatePuzzle> {
        size_t operator()(StatePuzzle const &s) const noexcept;
    };
}

class HeuristicPuzzle {
public:
    // constructor
    // For zero heuristic (Dijkstra's algorithm), pass 0
    // For default grid heuristic, pass non-zero
    explicit HeuristicPuzzle(int heuristic);
    // returns the hcost between two states
    // this is the max hcost of all the heuristics in *hList
    double hCost(StatePuzzle &node1, StatePuzzle &node2);
    // return if heuristic is consistent
    bool isConsistent();
private:
    // h == 0 for zero heuristic; h != 0 for default heuristic
    int h;
};

class ActionPuzzle {
public:
    // create an action from a character representing a direction
    explicit ActionPuzzle(char dir);
    // returns the direction character
    char getAction();
private:
    // slide direction ('u', 'd', 'l', 'r' for up, down, left, right)
    char slideDir;
};

template<class state>
class EnvironmentPuzzle {
public:
    // stores to a vector the possible actions that can be taken from a state
    void getActions(state &node, vector<ActionPuzzle> &actions);
    // move a state to a new state according to action
    void applyAction(state &s, ActionPuzzle a);
    // returns the cost of an edge between two states
    double edgeCost(state &state1, state &state2);
//    bool UndoAction(StatePuzzle &s, ActionPuzzle a);
//    uint64_t GetStateHash(StatePuzzle &node);
};

template<class state>
void EnvironmentPuzzle<state>::getActions(state &node, vector<ActionPuzzle> &actions) {
    int blankLoc = node.getLocation(0);
    bool top = false;
    bool bot = false;
    bool left = false;
    bool right = false;
    if (blankLoc % 4 == 0) {
        left = true;
    } else if (blankLoc % 4 == 3) {
        right = true;
    }
    if (blankLoc < 4) {
        top = true;
    } else if (blankLoc > 11) {
        bot = true;
    }
    if (!top) {
        actions.emplace_back('u');
    }
    if (!bot) {
        actions.emplace_back('d');
    }
    if (!left) {
        actions.emplace_back('l');
    }
    if (!right) {
        actions.emplace_back('r');
    }
}

template<class state>
void EnvironmentPuzzle<state>::applyAction(state &s, ActionPuzzle a) {
    int blankLoc = s.getLocation(0);
    if (a.getAction() == 'u') {
        s.swap(blankLoc, blankLoc - 4);
    } else if (a.getAction() == 'd') {
        s.swap(blankLoc, blankLoc + 4);
    } else if (a.getAction() == 'l') {
        s.swap(blankLoc, blankLoc - 1);
    } else { // 'r'
        s.swap(blankLoc, blankLoc + 1);
    }
}

template<class state>
double EnvironmentPuzzle<state>::edgeCost(state &state1, state &state2) {
    return 1;
}

#endif //ASSIGNMENT1_PUZZLE_H
