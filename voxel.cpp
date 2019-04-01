//
// Created by Katie Tran on 2019-01-21.
//

#include "voxel.h"
#include <math.h>
#include <iostream>
#include <bitset>
#include <random>

using namespace std;

double voxelHeuristic(int deltaX, int deltaY, int deltaZ) {
    int min = 0;
    int max = 0;
    int mid = 0;
    if (deltaX > deltaY) {
        max = deltaX;
        min = deltaY;
    } else {
        max = deltaY;
        min = deltaX;
    }
    if (deltaZ > max) {
        mid = max;
        max = deltaZ;
    } else if (deltaZ < min) {
        mid = min;
        min = deltaZ;
    } else {
        mid = deltaZ;
    }
    return (sqrt(3) - sqrt(2)) * min + (sqrt(2) - 1) * mid + max;
}

//StateVoxel::StateVoxel() {
//    stateID = 0;
//}

StateVoxel::StateVoxel(int *state) {
    stateID = hash(state);
}

StateVoxel::StateVoxel(int id) {
    stateID = id;
}

uint64_t StateVoxel::getStateID() const {
    return this->stateID;
}

uint64_t StateVoxel::hash(int *state) {
    // among 64 bits, the first bit is 1 (representing an actual state)
    // the last 16 bits represents the z coordinate
    // the next last 16 bits represents the y coordinate
    // and the next last 16 bits represents the x coordinate
    uint64_t res = 0;
    for (int i = 0; i < 3; i++) {
        res |= (uint64_t) state[i] << (16 * (2 - i));
    }
    res |= (uint64_t) 1 << 63;
    return res;
}

int StateVoxel::getValue(int axis) {
    int res = ((int) (this->stateID >> (16 * (2 - axis)))) & ((int) (pow(2, 16) - 1));
    return res;
}

void StateVoxel::display() {
    for (int i = 0; i < 3; i++) {
        cout << getValue(i) << "   ";
    }
    cout << endl;
}

void StateVoxel::assignValue(int val, int axis) {
    // clearing that location
    for (int i = 0; i < 16; i++) {
        this->stateID &= ~(1ULL << (16 * (2 - axis) + i));
    }
    // setting bits
    this->stateID += (uint64_t) val << (16 * (2 - axis));
}

bool operator==(const StateVoxel &lhs, const StateVoxel &rhs) {
    return lhs.getStateID() == rhs.getStateID();
}

bool operator<(const StateVoxel &lhs, const StateVoxel &rhs) {
    return lhs.getStateID() < rhs.getStateID();
}

bool operator>(const StateVoxel &lhs, const StateVoxel &rhs) {
    return lhs.getStateID() > rhs.getStateID();
}

size_t hash<StateVoxel>::operator()(StateVoxel const &s) const noexcept {
    hash<uint64_t> a;
    return a(s.getStateID());
}

ActionVoxel::ActionVoxel(short int dir) {
    moveDir = dir;
}

short int ActionVoxel::getAction() {
    return this->moveDir;
}

bool EnvironmentVoxel::isBlocked(int x, int y, int z) {
    for (auto &blocks : *map) {
        if (blocks[0] == x && blocks[1] == y && blocks[2] == z) {
            return true;
        }
    }
    return false;
}

void EnvironmentVoxel::getActions(StateVoxel &node, vector<ActionVoxel> &actions) {
    int x = node.getValue(0);
    int y = node.getValue(1);
    int z = node.getValue(2);
    // 1 coordinate changes
    if (!isBlocked(x, y, z + 1) && z + 1 < zMap) {
        actions.emplace_back(4);
    }
    if (!isBlocked(x, y, z - 1) && z - 1 >= 0) {
        actions.emplace_back(22);
    }
    if (!isBlocked(x - 1, y, z) && x - 1 >= 0) {
        actions.emplace_back(12);
    }
    if (!isBlocked(x + 1, y, z) && x + 1 < xMap) {
        actions.emplace_back(14);
    }
    if (!isBlocked(x, y - 1, z) && y - 1 >= 0) {
        actions.emplace_back(10);
    }
    if (!isBlocked(x, y + 1, z) && y + 1 < yMap) {
        actions.emplace_back(16);
    }

    // 2 coordinates change
    if (!isBlocked(x, y - 1, z + 1) && y - 1 >= 0 && z + 1 < zMap) {
        if (!isBlocked(x, y, z + 1) && !isBlocked(x, y - 1, z)) {
            actions.emplace_back(1);
        }
    }
    if (!isBlocked(x, y + 1, z + 1) && y + 1 < yMap && z + 1 < zMap) {
        if (!isBlocked(x, y, z + 1) && !isBlocked(x, y + 1, z)) {
            actions.emplace_back(7);
        }
    }
    if (!isBlocked(x - 1, y, z + 1) && x - 1 >= 0 && z + 1 < zMap) {
        if (!isBlocked(x, y, z + 1) && !isBlocked(x - 1, y, z)) {
            actions.emplace_back(3);
        }
    }
    if (!isBlocked(x + 1, y, z + 1) && x + 1 < xMap && z + 1 < zMap) {
        if (!isBlocked(x, y, z + 1) && !isBlocked(x + 1, y, z)) {
            actions.emplace_back(5);
        }
    }
    if (!isBlocked(x, y - 1, z - 1) && y - 1 >= 0 && z - 1 >= 0) {
        if (!isBlocked(x, y, z - 1) && !isBlocked(x, y - 1, z)) {
            actions.emplace_back(19);
        }
    }
    if (!isBlocked(x, y + 1, z - 1) && y + 1 < yMap && z - 1 >= 0) {
        if (!isBlocked(x, y, z - 1) && !isBlocked(x, y + 1, z)) {
            actions.emplace_back(25);
        }
    }
    if (!isBlocked(x - 1, y, z - 1) && x - 1 >= 0 && z - 1 >= 0) {
        if (!isBlocked(x, y, z - 1) && !isBlocked(x - 1, y, z)) {
            actions.emplace_back(21);
        }
    }
    if (!isBlocked(x + 1, y, z - 1) && x + 1 < xMap && z - 1 >= 0) {
        if (!isBlocked(x, y, z - 1) && !isBlocked(x + 1, y, z)) {
            actions.emplace_back(23);
        }
    }
    if (!isBlocked(x - 1, y - 1, z) && x - 1 >= 0 && y - 1 >= 0) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y - 1, z)) {
            actions.emplace_back(9);
        }
    }
    if (!isBlocked(x + 1, y - 1, z) && x + 1 < xMap && y - 1 >= 0) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y - 1, z)) {
            actions.emplace_back(11);
        }
    }
    if (!isBlocked(x - 1, y + 1, z) && x - 1 >= 0 && y + 1 < yMap) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y + 1, z)) {
            actions.emplace_back(15);
        }
    }
    if (!isBlocked(x + 1, y + 1, z) && x + 1 < xMap && y + 1 < yMap) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y + 1, z)) {
            actions.emplace_back(17);
        }
    }
    // 3 coordinates change
    if (!isBlocked(x - 1, y - 1, z + 1) && x - 1 >= 0 && y - 1 >= 0 && z + 1 < zMap) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y - 1, z) && !isBlocked(x, y, z + 1)
                && !isBlocked(x - 1, y - 1, z) && !isBlocked(x, y - 1, z + 1) && !isBlocked(x - 1, y, z + 1)) {
            actions.emplace_back(0);
        }
    }
    if (!isBlocked(x + 1, y - 1, z + 1) && x + 1 < xMap && y - 1 >= 0 && z + 1 < zMap) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y - 1, z) && !isBlocked(x, y, z + 1)
                && !isBlocked(x + 1, y - 1, z) && !isBlocked(x, y - 1, z + 1) && !isBlocked(x + 1, y, z + 1)) {
            actions.emplace_back(2);
        }
    }
    if (!isBlocked(x - 1, y + 1, z + 1) && x - 1 >= 0 && y + 1 < yMap && z + 1 < zMap) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y + 1, z) && !isBlocked(x, y, z + 1)
                && !isBlocked(x - 1, y + 1, z) && !isBlocked(x, y + 1, z + 1) && !isBlocked(x - 1, y, z + 1)) {
            actions.emplace_back(6);
        }
    }
    if (!isBlocked(x + 1, y + 1, z + 1) && x + 1 < xMap && y + 1 < yMap && z + 1 < zMap) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y + 1, z) && !isBlocked(x, y, z + 1)
                && !isBlocked(x + 1, y + 1, z) && !isBlocked(x, y + 1, z + 1) && !isBlocked(x + 1, y, z + 1)) {
            actions.emplace_back(8);
        }
    }
    if (!isBlocked(x - 1, y - 1, z - 1) && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y - 1, z) && !isBlocked(x, y, z - 1)
                && !isBlocked(x - 1, y - 1, z) && !isBlocked(x, y - 1, z - 1) && !isBlocked(x - 1, y, z - 1)) {
            actions.emplace_back(18);
        }
    }
    if (!isBlocked(x + 1, y - 1, z - 1) && x + 1 < xMap && y - 1 >= 0 && z - 1 >= 0) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y - 1, z) && !isBlocked(x, y, z - 1)
                && !isBlocked(x + 1, y - 1, z) && !isBlocked(x, y - 1, z - 1) && !isBlocked(x + 1, y, z - 1)) {
            actions.emplace_back(20);
        }
    }
    if (!isBlocked(x - 1, y + 1, z - 1) && x - 1 >= 0 && y + 1 < yMap && z - 1 >= 0) {
        if (!isBlocked(x - 1, y, z) && !isBlocked(x, y + 1, z) && !isBlocked(x, y, z - 1)
                && !isBlocked(x - 1, y + 1, z) && !isBlocked(x, y + 1, z - 1) && !isBlocked(x - 1, y, z - 1)) {
            actions.emplace_back(24);
        }
    }
    if (!isBlocked(x + 1, y + 1, z - 1) && x + 1 < xMap && y + 1 < yMap && z - 1 >= 0) {
        if (!isBlocked(x + 1, y, z) && !isBlocked(x, y + 1, z) && !isBlocked(x, y, z - 1)
                && !isBlocked(x + 1, y + 1, z) && !isBlocked(x, y + 1, z - 1) && !isBlocked(x + 1, y, z - 1)) {
            actions.emplace_back(26);
        }
    }
}

void EnvironmentVoxel::applyAction(StateVoxel &s, ActionVoxel a) {
    int dir = a.getAction();
    int x = s.getValue(0);
    int y = s.getValue(1);
    int z = s.getValue(2);
    if (dir < 9) {
        s.assignValue(z + 1, 2);
    } else if (dir > 17) {
        s.assignValue(z - 1, 2);
    }
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

double EnvironmentVoxel::edgeCost(StateVoxel &node1, StateVoxel &node2) {
    int deltaX = abs(node1.getValue(0) - node2.getValue(0));
    int deltaY = abs(node1.getValue(1) - node2.getValue(1));
    int deltaZ = abs(node1.getValue(2) - node2.getValue(2));
    int sum = deltaX + deltaY + deltaZ;
    if (sum == 1) {
        return 1;
    } else if (sum == 2) {
        return sqrt(2);
    } else if (sum == 3) {
        return sqrt(3);
    } else {
        cerr << "Illegal move detected." << endl;
        exit(EXIT_FAILURE);
    }
}

EnvironmentVoxel::EnvironmentVoxel(int x, int y, int z, vector<int *> *map_ptr) {
    map = map_ptr;
    xMap = x;
    yMap = y;
    zMap = z;
}

void EnvironmentVoxel::placeRandom(int n, vector<int> &coor) {
    int count = 0;
    random_device dev;
    mt19937 rng(dev());
//    mt19937 rng(20);
    uniform_int_distribution<mt19937::result_type> randX(0, (unsigned int) xMap - 1);
    uniform_int_distribution<mt19937::result_type> randY(0, (unsigned int) yMap - 1);
    uniform_int_distribution<mt19937::result_type> randZ(0, (unsigned int) zMap - 1);
    while (count < n) {
        int x = randX(rng);
        int y = randY(rng);
        int z = randZ(rng);
        if (isBlocked(x, y, z)) {
            // coordinate is blocked, generate another one
            continue;
        }
        bool flag = false;
        for (int i = 0; i < coor.size(); i += 3) {
            if (x == coor[i] && y == coor[i + 1] && z == coor[i + 2]) {
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
        coor.emplace_back(z);
        count++;
    }
}

void EnvironmentVoxel::placeRandom(int n, vector<int> &coor, vector<int> &exclude) {
    int count = 0;
    random_device dev;
    mt19937 rng(dev());
//    mt19937 rng(20);
    uniform_int_distribution<mt19937::result_type> randX(0, (unsigned int) xMap - 1);
    uniform_int_distribution<mt19937::result_type> randY(0, (unsigned int) yMap - 1);
    uniform_int_distribution<mt19937::result_type> randZ(0, (unsigned int) zMap - 1);
    while (count < n) {
        int x = randX(rng);
        int y = randY(rng);
        int z = randZ(rng);
        if (isBlocked(x, y, z)) {
            // coordinate is blocked, generate another one
            continue;
        }
        bool flag = false;
        for (int i = 0; i < coor.size(); i += 3) {
            if (x == coor[i] && y == coor[i + 1] && z == coor[i + 2]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            // coordinate has been generated before, generate another one
            continue;
        }
        for (int i = 0; i < exclude.size(); i += 3) {
            if (x == exclude[i] && y == exclude[i + 1] && z == exclude[i + 2]) {
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
        coor.emplace_back(z);
        count++;
    }
}

int EnvironmentVoxel::getDim() {
    return 3;
}

void EnvironmentVoxel::getLength(int &x, int &y, int &z) {
    x = xMap;
    y = yMap;
    z = zMap;
}

void EnvironmentVoxel::getLength(int &x, int &y) {
    cerr << "Error. Wrong dimension." << endl;
    exit(EXIT_FAILURE);
}

//double HeuristicAstarVoxel::hCost(StateVoxel &node1, StateVoxel &node2) {
//    int deltaX = abs(node1.getValue(0) - node2.getValue(0));
//    int deltaY = abs(node1.getValue(1) - node2.getValue(1));
//    int deltaZ = abs(node1.getValue(2) - node2.getValue(2));
//    return voxelHeuristic(deltaX, deltaY, deltaZ);
//}

HeuristicVoxel::HeuristicVoxel(vector<vector<double>> list, char lookupChar, int x, int y, int z) {
    hList = list;
    lookup = lookupChar;
    xMap = x;
    yMap = y;
    zMap = z;
}

double HeuristicVoxel::hCost(StateVoxel &node1, StateVoxel &node2) {
    double ret = 0;
    unsigned long index1 = (unsigned long) node1.getValue(2) * xMap * yMap + (unsigned long) node1.getValue(1) * xMap + node1.getValue(0);
    unsigned long index2 = (unsigned long) node2.getValue(2) * xMap * yMap + (unsigned long) node2.getValue(1) * xMap + node2.getValue(0);
    if (lookup == 'm') {
        // Maximum of all heuristic
        for (auto &vec : hList) {
            double temp = abs(vec[index1] - vec[index2]);
            if (temp > ret) {
                ret = temp;
            }
        }
    } else if (lookup == 's') {
        if (!hList.empty()) {
            // Look up from one heuristic only
            auto vec = hList[0];
            ret = abs(vec[index1] - vec[index2]);
        }
    } else {
        if (!hList.empty()) {
            // Random heuristic from the list
            random_device dev;
            mt19937 rng(dev());
            //        mt19937 rng(55);
            uniform_int_distribution<mt19937::result_type> rand(0, (unsigned int) hList.size() - 1);
            int indexH = rand(rng);
            auto vec = hList[indexH];
            ret = abs(vec[index1] - vec[index2]);
        }
    }
    return ret;
}

bool HeuristicVoxel::isConsistent() {
    if (lookup == 'r') {
        return false;
    } else {
        return true;
    }
}
