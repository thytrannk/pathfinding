//
// Created by Katie Tran on 2019-03-22.
//

#include "grid.h"
#include <math.h>
#include <iostream>
#include <bitset>
#include <random>

using namespace std;

double distance(int deltaX, int deltaY) {
    int min = 0;
    int max = 0;
    if (deltaX > deltaY) {
        max = deltaX;
        min = deltaY;
    } else {
        max = deltaY;
        min = deltaX;
    }
    return (sqrt(2) - 1) * min + max;
}

//StateGrid::StateGrid() {
//    stateID = 0;
//}

StateGrid::StateGrid(int *state) {
    stateID = hash(state);
}

StateGrid::StateGrid(int id) {
    stateID = id;
}

uint64_t StateGrid::getStateID() const {
    return stateID;
}

uint64_t StateGrid::hash(int *state) {
    // among 64 bits, the first bit is 1 (representing an actual state)
    // the last 16 bits represents the y coordinate
    // and the next last 16 bits represents the x coordinate
    uint64_t res = 0;
    for (int i = 0; i < 2; i++) {
        res |= (uint64_t) state[i] << (16 * (1 - i));
    }
    res |= (uint64_t) 1 << 63;
    return res;
}

int StateGrid::getValue(int axis) {
    int res = ((int) (stateID >> (16 * (1 - axis)))) & ((int) (pow(2, 16) - 1));
    return res;
}

void StateGrid::display() {
    for (int i = 0; i < 2; i++) {
        cout << getValue(i) << "   ";
    }
    cout << endl;
}

void StateGrid::assignValue(int val, int axis) {
    // clearing that location
    for (int i = 0; i < 16; i++) {
        stateID &= ~(1ULL << (16 * (1 - axis) + i));
    }
    // setting bits
    stateID += (uint64_t) val << (16 * (1 - axis));
}

bool operator==(const StateGrid &lhs, const StateGrid &rhs) {
    return lhs.getStateID() == rhs.getStateID();
}

bool operator<(const StateGrid &lhs, const StateGrid &rhs) {
    return lhs.getStateID() < rhs.getStateID();
}

bool operator>(const StateGrid &lhs, const StateGrid &rhs) {
    return lhs.getStateID() > rhs.getStateID();
}

size_t hash<StateGrid>::operator()(StateGrid const &s) const noexcept {
    hash<uint64_t> a;
    return a(s.getStateID());
}

ActionGrid::ActionGrid(short int dir) {
    moveDir = dir;
}

short int ActionGrid::getAction() {
    return moveDir;
}

bool EnvironmentGrid::isBlocked(int x, int y) {
    for (auto &blocks : *map) {
        if (blocks[0] == x && blocks[1] == y) {
            return true;
        }
    }
    return false;
}

void EnvironmentGrid::getActions(StateGrid &node, vector<ActionGrid> &actions) {
    int x = node.getValue(0);
    int y = node.getValue(1);
    // 1 coordinate changes
    if (!isBlocked(x - 1, y) && x - 1 >= 0) {
        actions.emplace_back(3);
    }
    if (!isBlocked(x + 1, y) && x + 1 < xMap) {
        actions.emplace_back(5);
    }
    if (!isBlocked(x, y - 1) && y - 1 >= 0) {
        actions.emplace_back(1);
    }
    if (!isBlocked(x, y + 1) && y + 1 < yMap) {
        actions.emplace_back(7);
    }

    // 2 coordinates change
    if (!isBlocked(x - 1, y - 1) && x - 1 >= 0 && y - 1 >= 0) {
        if (!isBlocked(x - 1, y) && !isBlocked(x, y - 1)) {
            actions.emplace_back(0);
        }
    }
    if (!isBlocked(x + 1, y - 1) && x + 1 < xMap && y - 1 >= 0) {
        if (!isBlocked(x + 1, y) && !isBlocked(x, y - 1)) {
            actions.emplace_back(2);
        }
    }
    if (!isBlocked(x - 1, y + 1) && x - 1 >= 0 && y + 1 < yMap) {
        if (!isBlocked(x - 1, y) && !isBlocked(x, y + 1)) {
            actions.emplace_back(6);
        }
    }
    if (!isBlocked(x + 1, y + 1) && x + 1 < xMap && y + 1 < yMap) {
        if (!isBlocked(x + 1, y) && !isBlocked(x, y + 1)) {
            actions.emplace_back(8);
        }
    }
}

void EnvironmentGrid::applyAction(StateGrid &s, ActionGrid a) {
    int dir = a.getAction();
    int x = s.getValue(0);
    int y = s.getValue(1);
    if (dir % 3 == 0) {
        s.assignValue(x - 1, 0);
    } else if (dir % 3 == 2) {
        s.assignValue(x + 1, 0);
    }
    if (dir % 9 < 3) {
        s.assignValue(y - 1, 1);
    } else if (dir % 9 > 5) {
        s.assignValue(y + 1, 1);
    }
}

double EnvironmentGrid::edgeCost(StateGrid &node1, StateGrid &node2) {
    int deltaX = abs(node1.getValue(0) - node2.getValue(0));
    int deltaY = abs(node1.getValue(1) - node2.getValue(1));
    int sum = deltaX + deltaY;
    if (sum == 1) {
        return 1;
    } else if (sum == 2) {
        return sqrt(2);
    } else {
        node1.display();
        node2.display();
        cerr << "Illegal move detected." << endl;
        exit(EXIT_FAILURE);
    }
}

EnvironmentGrid::EnvironmentGrid(int x, int y, vector<int *> *map_ptr) {
    map = map_ptr;
    xMap = x;
    yMap = y;
}

void EnvironmentGrid::placeRandom(int n, vector<int> &coor) {
    int count = 0;
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> randX(0, (unsigned int) xMap - 1);
    uniform_int_distribution<mt19937::result_type> randY(0, (unsigned int) yMap - 1);
    while (count < n) {
        int x = randX(rng);
        int y = randY(rng);
        if (isBlocked(x, y)) {
            // coordinate is blocked, generate another one
            continue;
        }
        bool flag = false;
        for (int i = 0; i < coor.size(); i += 2) {
            if (x == coor[i] && y == coor[i + 1]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            // coordinate has been generated before, generate another one
            continue;
        }
        // good coordinates
        coor.emplace_back(x);
        coor.emplace_back(y);
        count++;
    }
}

void EnvironmentGrid::placeRandom(int n, vector<int> &coor, vector<int> &exclude) {
    int count = 0;
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> randX(0, (unsigned int) xMap - 1);
    uniform_int_distribution<mt19937::result_type> randY(0, (unsigned int) yMap - 1);
    while (count < n) {
        int x = randX(rng);
        int y = randY(rng);
        if (isBlocked(x, y)) {
            // coordinate is blocked, generate another one
            continue;
        }
        bool flag = false;
        for (int i = 0; i < coor.size(); i += 2) {
            if (x == coor[i] && y == coor[i + 1]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            // coordinate has been generated before, generate another one
            continue;
        }
        for (int i = 0; i < exclude.size(); i += 2) {
            if (x == exclude[i] && y == exclude[i + 1]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            // coordinate is excluded, generate another one
            continue;
        }
        // good coordinates
        coor.emplace_back(x);
        coor.emplace_back(y);
        count++;
    }
}

int EnvironmentGrid::getDim() {
    return 2;
}

void EnvironmentGrid::getLength(int &x, int &y) {
    x = xMap;
    y = yMap;
}

void EnvironmentGrid::getLength(int &x, int &y, int &z) {
    cerr << "Error. Try to read non-existent dimension." << endl;
    exit(EXIT_FAILURE);
}

//double HeuristicAstarGrid::hCost(StateGrid &node1, StateGrid &node2) {
//    int deltaX = abs(node1.getValue(0) - node2.getValue(0));
//    int deltaY = abs(node1.getValue(1) - node2.getValue(1));
//    return distance(deltaX, deltaY);
//}

HeuristicGrid::HeuristicGrid(int heuristic) {
    h = heuristic;
}

double HeuristicGrid::hCost(StateGrid &node1, StateGrid &node2) {
    double ret = 0;
    if (h == 0) {
        // zero heuristic, used for Dijkstra algorithm
        return ret;
    }
    // grid heuristic
    int deltaX = abs(node1.getValue(0) - node2.getValue(0));
    int deltaY = abs(node1.getValue(1) - node2.getValue(1));
    ret = distance(deltaX, deltaY);
    return ret;
}

bool HeuristicGrid::isConsistent() {
    return true;
}