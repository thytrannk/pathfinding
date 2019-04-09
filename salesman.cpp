//
// Created by Katie Tran on 2019-04-07.
//

#include "salesman.h"
#include <iostream>
#include <random>
#include <algorithm>

#define NO_CITIES 12

StateSales::StateSales(int id) {
    // id == 0: empty state (all 0)
    // id == 1: root state (first char = 'A')
    // id == -1: goal state (all '!')
    stateID = string(NO_CITIES + 1, '0');
    if (id == 0) {
        return;
    } else if (id == 1) {
        // root node, pick the first city in the list as the starting city
        stateID[0] = 'A';
        return;
    } else if (id == -1) {
        // representation of a goal state
        stateID = string(NO_CITIES + 1, '!');
    } else {
        cerr << "Invalid call to StateSales constructor." << endl;
        exit(EXIT_FAILURE);
    }
}

string StateSales::getStateID() const {
    return stateID;
}

int StateSales::getNoCities() const {
    return stateID.size() - 1;
}

int StateSales::getValue(int loc) {
    return stateID[loc];
}

void StateSales::display() {
    cout << stateID << endl;
}

void StateSales::assignValue(char val, int loc) {
    stateID[loc] = val;
}

bool operator==(const StateSales &lhs, const StateSales &rhs) {
    if (rhs.getStateID()[0] == '!') {
        // checking if lhs is a goal
        string str = lhs.getStateID();
        if (str[str.size() - 1] == 'A') {
            // has returned to origin
            return true;
        } else {
            return false;
        }
    } else {
        return lhs.getStateID() == rhs.getStateID();
    }
}

bool operator<(const StateSales &lhs, const StateSales &rhs) {
    return lhs.getStateID() < rhs.getStateID();
}

bool operator>(const StateSales &lhs, const StateSales &rhs) {
    return lhs.getStateID() > rhs.getStateID();
}

size_t hash<StateSales>::operator()(StateSales const &s) const noexcept {
    hash<string> a;
    return a(s.getStateID());
}

ActionSales::ActionSales(char dir) {
    moveTo = dir;
}

char ActionSales::getAction() {
    return moveTo;
}

EnvironmentSales::EnvironmentSales() {
    int count = 0;
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> rand(0, 99);
    while (count < NO_CITIES) {
        vector<double> point;
        // x-coordinate
        point.emplace_back((double) rand(rng) / 100);
        // y-coordinate
        point.emplace_back((double) rand(rng) / 100);
        bool flag = false;
        for (int i = 0; i < map.size(); i += 2) {
            if (point[0] == map[i][0] && point[1] == map[i][1]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            // coordinate has been generated before, generate another one
            continue;
        }
        // good coordinates
        map.emplace_back(point);
        count++;
    }
}

void EnvironmentSales::getActions(StateSales &node, vector<ActionSales> &actions) {
    string id = node.getStateID();
    for (int i = 0; i < map.size(); i++) {
        // iterate through each city in map
        bool flag = true; // whether to add city to list of actions
        for (auto &c : id) {
            if (c == 'A' + i) {
                // city has been visited
                flag = false;
                break;
            } else if (c == '0') {
                // no more visited cities to check
                break;
            }
        }
        if (flag) {
            actions.emplace_back('A' + i);
        }
    }
    if (actions.empty()) {
        // has visited all cities. must go back to origin
        actions.emplace_back('A');
    }
}

void EnvironmentSales::applyAction(StateSales &s, ActionSales a) {
    string id = s.getStateID();
    for (int i = 0; i < id.size(); i++) {
        if (id[i] == '0') {
            s.assignValue(a.getAction(), i);
            return;
        }
    }
}

double EnvironmentSales::edgeCost(StateSales &node1, StateSales &node2) {
    string id = node2.getStateID();
    int i;
    for (i = 0; i < id.size(); i++) {
        if (id[i] == '0') {
            break;
        }
    }
    if (i < 2) {
        cerr << "Invalid edge detected." << endl;
        exit(EXIT_FAILURE);
    }
    int s = id[i - 2] - 'A';
    int g = id[i - 1] - 'A';
    double deltaX = abs(map[g][0] - map[s][0]);
    double deltaY = abs(map[g][1] - map[s][1]);
    return sqrt(deltaX * deltaX + deltaY * deltaY);
}

int EnvironmentSales::getNoCities() {
    return map.size();
}

vector<vector<double>> EnvironmentSales::getMap() {
    return map;
}

HeuristicSales::HeuristicSales(int heuristic, EnvironmentSales environment) {
    h = heuristic;
    e = environment;
    vector<vector<double>> map = e.getMap();
    for (int i = 0; i < e.getNoCities(); i++) {
        for (int j = i + 1; j < e.getNoCities(); j++) {
            double dist = sqrt((map[i][0] - map[j][0]) * (map[i][0] - map[j][0]) + (map[i][1] - map[j][1]) * (map[i][1] - map[j][1]));
            edgeList.emplace_back(pair<double, int>{dist, i * e.getNoCities() + j});
        }

    }
    sort(edgeList.begin(), edgeList.end());
}

double HeuristicSales::hCost(StateSales &node1, StateSales &node2) {
    double ret = 0;
    if (h == 0) {
        // zero heuristic, used for Dijkstra algorithm
        return ret;
    }
    // return minimum spanning tree heuristic (Prim's algorithm)
    // get list of cities that haven't been visited
    string id = node1.getStateID();
    vector<int> unvisited;
    for (int i = 0; i < NO_CITIES; i++) {
        bool flag = true; // whether cities is unvisited
        for (char j : id) {
            if (j == 'A' + i) {
                flag = false;
                break;
            } else if (j == '0') {
                break;
            }
        }
        if (flag) {
            unvisited.emplace_back(i);
        }
    }
    // add the last visited city to the list
    int lastVisited;
    for (lastVisited = 0; lastVisited < id.size(); lastVisited++) {
        if (id[lastVisited] == '0') {
            break;
        }
    }
    lastVisited--;
    unvisited.emplace_back(id[lastVisited] - 'A');
    if (id[lastVisited] != 'A') {
        // add origin to the list of cities to be visited
        unvisited.emplace_back(0);
    }

    int size = unvisited.size();
    vector<int> covered{unvisited[size - 1]};
    unvisited.pop_back();
    while (covered.size() < size) {
        for (auto &edge : edgeList) {
            int u = edge.second % e.getNoCities();
            int v = edge.second / e.getNoCities();
            auto find1 = find(unvisited.begin(), unvisited.end(), u);
            if (find1 != unvisited.end()) {
                // u is in unvisited
                auto find2 = find(covered.begin(), covered.end(), v);
                if (find2 != covered.end()) {
                    // v is in covered
                    covered.emplace_back(u);
                    unvisited.erase(find1);
                    ret += edge.first;
                    break;
                }
            }
        }
    }
    return ret;
}

bool HeuristicSales::isConsistent() {
    return true;
}
