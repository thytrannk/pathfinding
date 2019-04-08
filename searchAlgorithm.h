//
// Created by Katie Tran on 2019-01-13.
//

#ifndef ASSIGNMENT1_SEARCHALGORITHM_H
#define ASSIGNMENT1_SEARCHALGORITHM_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <unistd.h>
#include <cfloat>
#include <climits>
#include "minHeap.h"

#define EPSILON 0.000000001

double minimum(double a, double b, double c);
double maximum(double a, double b, double c);

namespace std {
    template<> class numeric_limits<pair<double, double>> {
    public:
        static pair<double, double> max() {
            return pair<double, double>{DBL_MAX, DBL_MAX};
        };
        static pair<double, double> min() {
            return pair<double, double>{DBL_MIN, DBL_MIN};
        };
    };
}

template <class state, class action, class environment, class heuristic>
class Astar {
public:
    explicit Astar(state &s, state &g, heuristic &h);
    // find the path from start to goal state
    void getPath(environment &e, heuristic &h, state &s, state &g, vector<state> &path);
    // print out the path from start to goal state if found
    void displayPath(vector<state> &path);
private:
    typedef struct node {
        state thisState;
        state parent;
        bool closed = false;
        double gcost = DBL_MAX;
        double hcost = 0;
        uint64_t locationOnOpen = UINT64_MAX;
        node(state s, state p, bool isClosed, double g, double h, uint64_t location)
                : thisState(s), parent(p), closed(isClosed), gcost(g), hcost(h), locationOnOpen(location) {}
        node(state s, state p, double g, double h) : node(s, p, false, g, h, UINT64_MAX) {}
        node(state s, state p) : node(s, p, DBL_MAX, 0) {}
    } Node;
    // hash map of states and index in state vector
    unordered_map<state, uint64_t> hashMap;
    // priority queue of open list with key being pair<f-cost, h-cost>
    MinHeap<pair<double, double>, Node> queue;
    // state vector
    vector<Node> stateVec;
};

template <class state, class action, class environment, class heuristic>
class WeightedAStar {
public:
    // constructor. b is bound; opt is true for optimistic search, false for weighted A*
    explicit WeightedAStar(state &s, state &g, heuristic &h, double b, bool opt);
    // find the path from start to goal state
    void getPath(environment &e, heuristic &h, state &s, state &g, vector<state> &path);
    // print out the path from start to goal state if found
    void displayPath(vector<state> &path);
private:
    typedef struct node {
        state thisState;
        state parent;
        bool closed = false;
        double gcost = DBL_MAX;
        uint64_t locationOnOpen = UINT64_MAX;
        uint64_t locationOnOpenHat = UINT64_MAX;
        node(state s, state p, bool isClosed, double g, uint64_t locationOpen, uint64_t locationOpenHat)
                : thisState(s), parent(p), closed(isClosed), gcost(g), locationOnOpen(locationOpen), locationOnOpenHat(locationOpenHat) {}
        node(state s, state p, double g) : node(s, p, false, g, UINT64_MAX, UINT64_MAX) {}
        node(state s, state p) : node(s, p, DBL_MAX, 0) {}
    } Node;
    // hash map of states and index in state vector
    unordered_map<state, uint64_t> hashMap;
    // priority queue of open list with key being pair<f-cost, h-cost>
    MinHeap<pair<double, double>, Node> open;
    MinHeap<pair<double, double>, Node> openHat;
    // state vector
    vector<Node> stateVec;
    double bound;
    double weight;
    bool optimistic;
    // find the path from start to goal state for optimistic search
    void getPathOpt(environment &e, heuristic &h, state &s, state &g, vector<state> &path);
    // find the path from start to goal state for weighted A*
    void getPathWeighted(environment &e, heuristic &h, state &s, state &g, vector<state> &path);
};

template <class state, class action, class environment, class heuristic>
class IteratedDeepening {
public:
    // find the path from start to goal state
    void getPath(environment &e, heuristic &h, state &s, state &g, vector<state> &path, bool dfid);
    // print out the path from start to goal state if found
    void displayPath(vector<state> &path);
private:
    // depth limited search
    double dls(environment &e, heuristic &h, state &current, state &parent, state &g,
            double currentVal, double bound, bool &found, bool &remaining,
            uint64_t &nodeCount, std::vector<state> &path, bool dfid);
};


template<class state, class action, class environment, class heuristic>
Astar<state, action, environment, heuristic>::Astar(state &s, state &g, heuristic &h) {
    Node init(s, s, 0, h.hCost(s, g));
    hashMap.emplace(s, 0);
    stateVec.emplace_back(init);
}

template <class state, class action, class environment, class heuristic>
void Astar<state, action, environment, heuristic>::getPath(environment &e, heuristic &h, state &s, state &g,
                                                           vector<state> &path) {
    uint64_t nodeExpanded = 0;
    uint64_t nodeUpdated = 0;
    uint64_t nodeReopened = 0;
    // insert start state into the queue, which is at index 0 in state vector
    queue.insertKey(pair<pair<double, double>, uint64_t>{{stateVec[0].hcost, stateVec[0].hcost}, 0}, &stateVec);
    vector<action> actions;
    actions.reserve(26);
    vector<state> childStates;
    childStates.reserve(26);
    while (queue.size() != 0) {
        pair<pair<double, double>, uint64_t> current = queue.extractMin(&stateVec);
        if (stateVec[current.second].thisState == g) {
            // found the goal state
            // record the path from goal to start
            path.emplace_back(g);
            state temp = g;

            while (true) {
                uint64_t index = hashMap[temp];
                if (stateVec[index].parent == stateVec[index].thisState) {
                    break;
                } else {
                    path.emplace_back(stateVec[index].parent);
                }
                temp = stateVec[index].parent;
            }

            cout << "Cost of solution: " << stateVec[current.second].gcost << endl;
            cout << "Number of nodes generated: " << stateVec.size() << endl;
//            cout << "Number of nodes expanded: " << nodeExpanded << endl;
//            cout << "Number of nodes in open list: " << queue.size() << endl;
//            cout << "Number of nodes in closed list: " << nodeExpanded - nodeReopened << endl;
//            cout << "Number of times nodes are updated in the open list: " << nodeUpdated << endl;
//            cout << "Number of times nodes are updated in the closed list: " << nodeReopened << endl;

            return;
        }
        stateVec[current.second].closed = true;
        nodeExpanded++;

        // get possible actions to be taken and save to actions vector
        e.getActions(stateVec[current.second].thisState, actions);
        for (int i = 0; i < actions.size(); i++) {
            // go through each child node
            // make a copy of the current state
            childStates.emplace_back(stateVec[current.second].thisState);
            // move the childState according to action taken
            e.applyAction(childStates[i], actions[i]);
        }
        double bestH = 0;
        if (!h.isConsistent()) {
            // inconsistent heuristic
            for (auto &childState : childStates) {
                auto search = hashMap.find(childState);
                double childH;
                if (search != hashMap.end()) {
                    childH = stateVec[search->second].hcost;
                } else {
                    childH = h.hCost(childState, g);
                }
                bestH = max(bestH, childH - e.edgeCost(stateVec[current.second].thisState, childState));
            }
            stateVec[current.second].hcost = max(stateVec[current.second].hcost, bestH);
        }
        for (auto &childState : childStates) {
            // go through each child node
            // thread's cancellation point
            pthread_testcancel();
            // check if this child node has been generated
            auto search = hashMap.find(childState);
            double edge = e.edgeCost(stateVec[current.second].thisState, childState);
            double tentative_gcost = stateVec[current.second].gcost + edge;
            if (search != hashMap.end()) {
                // found, child node has been generated before
                if (stateVec[search->second].closed) {
                    // child node is in closed list
                    if (h.isConsistent()) {
                        continue;
                    } else {
                        // inconsistent heuristic
                        if (stateVec[search->second].hcost < bestH - edge) {
                            stateVec[search->second].hcost = bestH - edge;
                        }
                        // check if shorter path is found, if yes, reopen node
                        // plus epsilon to prevent small accuracies of type double's value resulting in unnecessary reopening
                        if (tentative_gcost + EPSILON >= stateVec[search->second].gcost) {
                            continue;
                        } else {
                            stateVec[search->second].closed = false;
                            stateVec[search->second].parent = stateVec[current.second].thisState;
                            stateVec[search->second].gcost = tentative_gcost;
                            queue.insertKey(pair<pair<double, double>, uint64_t>
                                    {{tentative_gcost + stateVec[search->second].hcost, stateVec[search->second].hcost},
                                     search->second}, &stateVec);
                            nodeReopened++;
                        }
                    }
                } else {
                    // child node is in open list
                    // plus epsilon to prevent small accuracies of type double's value resulting in unnecessary updating
                    if (tentative_gcost + EPSILON >= stateVec[search->second].gcost) {
                        continue;
                    } else {
                        stateVec[search->second].parent = stateVec[current.second].thisState;
                        stateVec[search->second].gcost = tentative_gcost;
                        queue.decreaseKey(stateVec[search->second].locationOnOpen,
                                pair<double, double>{tentative_gcost + stateVec[search->second].hcost,
                                                     stateVec[search->second].hcost}, &stateVec);
                        nodeUpdated++;
                    }
                    if (!h.isConsistent()) {
                        if (stateVec[search->second].hcost < bestH - edge) {
                            stateVec[search->second].hcost = bestH - edge;
                            queue.decreaseKey(stateVec[search->second].locationOnOpen,
                                              pair<double, double>{stateVec[search->second].gcost + bestH - edge,
                                                                   bestH - edge}, &stateVec);
                        }
                    }
                }
            } else {
                // not found (newly discovered node)
                Node childNode(childState, stateVec[current.second].thisState, tentative_gcost, max(h.hCost(childState, g), bestH - edge));
                // add node to state vector and hash table
                stateVec.emplace_back(childNode);
                hashMap.emplace(childState, stateVec.size() - 1);
                uint64_t location = queue.insertKey(pair<pair<double, double>, uint64_t>
                        {{childNode.gcost + childNode.hcost, childNode.hcost},
                         stateVec.size() - 1}, &stateVec);
            }
        }
        actions.clear();
        childStates.clear();
    }
}

template<class state, class action, class environment, class heuristic>
void Astar<state, action, environment, heuristic>::displayPath(vector<state> &path) {
    for (typename vector<state>::reverse_iterator it = path.rbegin(); it != path.rend(); it++) {
        it->display();
    }
}

template<class state, class action, class environment, class heuristic>
WeightedAStar<state, action, environment, heuristic>::WeightedAStar(state &s, state &g, heuristic &h, double b, bool opt) {
    bound = b;
    optimistic = opt;
    if (opt) {
        weight = 2 * bound - 1;
    } else {
        weight = bound;
    }
    Node init(s, s, 0);
    hashMap.emplace(s, 0);
    stateVec.emplace_back(init);
}

template <class state, class action, class environment, class heuristic>
void WeightedAStar<state, action, environment, heuristic>::getPath(environment &e, heuristic &h, state &s, state &g,
                                                           vector<state> &path) {
    if (optimistic) {
        getPathOpt(e, h, s, g, path);
    } else {
        getPathWeighted(e, h, s, g, path);
    }
}

template <class state, class action, class environment, class heuristic>
void WeightedAStar<state, action, environment, heuristic>::getPathOpt(environment &e, heuristic &h, state &s, state &g,
                                                                       vector<state> &path) {
    // insert start state into the queue, which is at index 0 in state vector
    open.insertKey(pair<pair<double, double>, uint64_t>{{h.hCost(s, g), h.hCost(s, g)}, 0}, &stateVec, false);
    openHat.insertKey(pair<pair<double, double>, uint64_t>{{weight * h.hCost(s, g), weight * h.hCost(s, g)}, 0}, &stateVec, true);
    vector<action> actions;
    actions.reserve(26);
    double fIncumbent = DBL_MAX;
    double fHatIncumbent = DBL_MAX;
    while (bound * open.getMin().first.first < fIncumbent) {
        pair<pair<double, double>, uint64_t> n;
        if (openHat.getMin().first.first < fHatIncumbent) {
            n = openHat.extractMin(&stateVec, true);
            open.deleteElement(stateVec[n.second].locationOnOpen, &stateVec, false);
        } else {
            n = open.extractMin(&stateVec, false);
            openHat.deleteElement(stateVec[n.second].locationOnOpenHat, &stateVec, true);
        }
        stateVec[n.second].closed = true;
        if (stateVec[n.second].thisState == g) {
            fIncumbent = stateVec[n.second].gcost;
            fHatIncumbent = stateVec[n.second].gcost;

        }
        e.getActions(stateVec[n.second].thisState, actions);
        for (auto &act : actions) {
            // go through each child node
            // thread's cancellation point
            pthread_testcancel();
            // make a copy of the current state
            state childState = stateVec[n.second].thisState;
            // move the childState according to action taken
            e.applyAction(childState, act);
            // check if this child node has been generated
            auto search = hashMap.find(childState);
            double tentative_gcost = stateVec[n.second].gcost + e.edgeCost(stateVec[n.second].thisState, childState);
            if (search != hashMap.end()) {
                // found, child node has been generated before
                if (!stateVec[search->second].closed) {
                    // duplicated in open
                    // plus epsilon to prevent small accuracies of type double's value resulting in unnecessary updating
                    if (tentative_gcost + EPSILON < stateVec[search->second].gcost) {
                        // better than the duplicate
                        stateVec[search->second].parent = stateVec[n.second].thisState;
                        stateVec[search->second].gcost = tentative_gcost;
                        open.decreaseKey(stateVec[search->second].locationOnOpen,
                                          pair<double, double>{tentative_gcost + h.hCost(childState, g),
                                                               h.hCost(childState, g)}, &stateVec, false);
                        openHat.decreaseKey(stateVec[search->second].locationOnOpenHat,
                                         pair<double, double>{tentative_gcost + weight * h.hCost(childState, g),
                                                              weight * h.hCost(childState, g)}, &stateVec, true);
                    }
                } else {
                    // duplicated in close
                    // plus epsilon to prevent small accuracies of type double's value resulting in unnecessary reopening
                    if (tentative_gcost + EPSILON < stateVec[search->second].gcost) {
                        stateVec[search->second].closed = false;
                        stateVec[search->second].parent = stateVec[n.second].thisState;
                        stateVec[search->second].gcost = tentative_gcost;
                        uint64_t a = open.insertKey(pair<pair<double, double>, uint64_t>
                                                {{tentative_gcost + h.hCost(childState, g), h.hCost(childState, g)},
                                                 search->second}, &stateVec, false);
                        uint64_t b = openHat.insertKey(pair<pair<double, double>, uint64_t>
                                               {{tentative_gcost + weight * h.hCost(childState, g), weight * h.hCost(childState, g)},
                                                search->second}, &stateVec, true);
                    }
                }
            } else {
                // not found (newly discovered node)
                Node childNode(childState, stateVec[n.second].thisState, tentative_gcost);
                // add node to state vector and hash table
                stateVec.emplace_back(childNode);
                hashMap.emplace(childState, stateVec.size() - 1);
                open.insertKey(pair<pair<double, double>, uint64_t> {{tentative_gcost + h.hCost(childState, g),
                                                                      h.hCost(childState, g)}, stateVec.size() - 1}, &stateVec, false);
                openHat.insertKey(pair<pair<double, double>, uint64_t>{{tentative_gcost + weight * h.hCost(childState, g),
                                                                        weight * h.hCost(childState, g)}, stateVec.size() - 1}, &stateVec, true);
            }
        }
        actions.clear();
    }
    // found the goal state
    // record the path from goal to start
    path.emplace_back(g);
    state temp = g;

    while (true) {
        uint64_t index = hashMap[temp];
        if (stateVec[index].parent == stateVec[index].thisState) {
            break;
        } else {
            path.emplace_back(stateVec[index].parent);
        }
        temp = stateVec[index].parent;
    }
    cout << "Cost of solution: " << fIncumbent << endl;
    cout << "Number of nodes generated: " << stateVec.size() << endl;
}

template <class state, class action, class environment, class heuristic>
void WeightedAStar<state, action, environment, heuristic>::getPathWeighted(environment &e, heuristic &h, state &s, state &g,
                                                           vector<state> &path) {

    // insert start state into the queue, which is at index 0 in state vector
    open.insertKey(pair<pair<double, double>, uint64_t>{{bound * h.hCost(s, g), bound * h.hCost(s, g)}, 0}, &stateVec);
    vector<action> actions;
    actions.reserve(26);
    while (open.size() != 0) {
        pair<pair<double, double>, uint64_t> current = open.extractMin(&stateVec);
        if (stateVec[current.second].thisState == g) {
            // found the goal state
            // record the path from goal to start
            path.emplace_back(g);
            state temp = g;

            while (true) {
                uint64_t index = hashMap[temp];
                if (stateVec[index].parent == stateVec[index].thisState) {
                    break;
                } else {
                    path.emplace_back(stateVec[index].parent);
                }
                temp = stateVec[index].parent;
            }

            cout << "Cost of solution: " << stateVec[current.second].gcost << endl;
            cout << "Number of nodes generated: " << stateVec.size() << endl;
//            cout << "Number of nodes expanded: " << nodeExpanded << endl;
//            cout << "Number of nodes in open list: " << queue.size() << endl;
//            cout << "Number of nodes in closed list: " << nodeExpanded << endl;
//            cout << "Number of times nodes are updated in the open list: " << nodeUpdated << endl;
//            cout << "Number of times nodes are updated in the closed list: 0" << endl;

            return;
        }

        stateVec[current.second].closed = true;

//        vector<action> actions;
        e.getActions(stateVec[current.second].thisState, actions);
        for (auto &act : actions) {
            // go through each child node
            // thread's cancellation point
            pthread_testcancel();
            // make a copy of the current state
            state childState = stateVec[current.second].thisState;
            // move the childState according to action taken
            e.applyAction(childState, act);
            // check if this child node has been generated
            auto search = hashMap.find(childState);
            double tentative_gcost = stateVec[current.second].gcost + e.edgeCost(stateVec[current.second].thisState, childState);
            if (search != hashMap.end()) {
                // found, child node has been generated before
                if (stateVec[search->second].closed) {
                    // child node is in closed list
                    continue;
                } else {
                    // child node is in open list
                    if (tentative_gcost >= stateVec[search->second].gcost) {
                        continue;
                    } else {
                        stateVec[search->second].parent = stateVec[current.second].thisState;
                        stateVec[search->second].gcost = tentative_gcost;
                        open.decreaseKey(stateVec[search->second].locationOnOpen,
                                          pair<double, double>{tentative_gcost + bound * h.hCost(childState, g),
                                                               bound * h.hCost(childState, g)}, &stateVec);
                    }
                }
            } else {
                // not found (newly discovered node)
                Node childNode(childState, stateVec[current.second].thisState, tentative_gcost);
                // add node to state vector and hash table
                stateVec.emplace_back(childNode);
                hashMap.emplace(childState, stateVec.size() - 1);
                open.insertKey(pair<pair<double, double>, uint64_t>{{tentative_gcost + bound * h.hCost(childState, g),
                                                                     bound * h.hCost(childState, g)}, stateVec.size() - 1}, &stateVec);
            }
        }
        actions.clear();
    }
}

template<class state, class action, class environment, class heuristic>
void WeightedAStar<state, action, environment, heuristic>::displayPath(vector<state> &path) {
    for (typename vector<state>::reverse_iterator it = path.rbegin(); it != path.rend(); it++) {
        it->display();
    }
}

template <class state, class action, class environment, class heuristic>
void IteratedDeepening<state, action, environment, heuristic>::getPath(environment &e, heuristic &h, state &s, state &g,
                                                          vector<state> &path, bool dfid) {
    bool found = false;
    bool remaining = true;
//    uint64_t nodeExpanded = 0;
    uint64_t nodeGenerated = 0;
    double bound;
    if (dfid) {
        bound = -1;
    } else { // IDAstar
        bound = h.hCost(s, g);
    }
    while (bound < DBL_MAX) {
//        nodeGenerated = 0;
        bound = dls(e, h, s, s, g, 0, bound, found, remaining, nodeGenerated, path, dfid);
        if (found) {
            // reached goal state
            cout << "Cost of solution: " << bound << endl;
//            cout << "Number of nodes expanded: " << nodeExpanded << endl;
            cout << "Number of nodes generated: " << nodeGenerated << endl;
            return;
        } else if (!remaining) {
            cout << "Cannot find path." << endl;
            return;
        }
    }
    cout << "Cannot find path." << endl;
}

template<class state, class action, class environment, class heuristic>
double IteratedDeepening<state, action, environment, heuristic>::dls(environment &e, heuristic &h,
        state &current, state &parent, state &g, double currentVal, double bound, bool &found, bool &remaining,
        uint64_t &nodeCount, std::vector<state> &path, bool dfid) {
    double value;
    if (dfid) {
        value = currentVal;
    } else {
        value = currentVal + h.hCost(current, g);
    }
    if (value > bound || (!dfid && value == bound)) {
        if (current == g) {
            // found goal state
            path.emplace_back(current);
            found = true;
            return value;
        } else if (value > bound) {
            // have not found goal state, but there are nodes with greater depth
            found = false;
            remaining = true;
            return value;
        }
    }

    double minF = DBL_MAX;
    bool any_remaining = false;
    vector<action> actions;
    e.getActions(current, actions);
//    nodeCount++;

    for (auto &act : actions) {
        // go through each neighboring node
        // thread's cancellation point
        pthread_testcancel();
        // make a copy of the current state
        state childState = current;
        // move the childState according to action taken
        e.applyAction(childState, act);
        if (childState == parent) {
            continue;
        }
        nodeCount++;
        double newBound;
        if (dfid) {
            newBound = dls(e, h, childState, current, g, currentVal + 1, bound, found, remaining, nodeCount, path, true);
        } else {
            double edgeCost = e.edgeCost(current, childState);
            newBound = dls(e, h, childState, current, g, currentVal + edgeCost, bound, found, remaining, nodeCount, path, false);
        }
        if (found) {
            path.emplace_back(current);
            return newBound;
        }
        if (remaining) {
            // there are nodes with greater depth
            any_remaining = true;
        }
        if (!dfid && newBound < minF) {
            minF = newBound;
        }
    }
    found = false;
    remaining = any_remaining;
    if (dfid) {
        return bound + 1;
    } else {
        return minF;
    }
}

template<class state, class action, class environment, class heuristic>
void IteratedDeepening<state, action, environment, heuristic>::displayPath(vector<state> &path) {
    for (typename vector<state>::reverse_iterator it = path.rbegin(); it != path.rend(); it++) {
        it->display();
    }
}

double minimum(double a, double b, double c) {
    double min = a;
    if (b < min) {
        min = b;
    }
    if (c < min) {
        min = c;
    }
    return min;
}

double maximum(double a, double b, double c) {
    double max = a;
    if (b > max) {
        max = b;
    }
    if (c > max) {
        max = c;
    }
    return max;
}

#endif //ASSIGNMENT1_SEARCHALGORITHM_H
