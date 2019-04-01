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
        uint64_t locationOnOpenHat = UINT64_MAX;
        node(state s, state p, bool isClosed, double g, double h, uint64_t locationOpen, uint64_t locationOpenHat)
                : thisState(s), parent(p), closed(isClosed), gcost(g), hcost(h), locationOnOpen(locationOpen), locationOnOpenHat(locationOpenHat) {}
        node(state s, state p, double g, double h) : node(s, p, false, g, h, UINT64_MAX, UINT64_MAX) {}
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
        node(state s, state p, bool isClosed, double cost, uint64_t location)
                : thisState(s), parent(p), closed(isClosed), gcost(cost), locationOnOpen(location) {}
        node(state s, state p, double cost) : node(s, p, false, cost, UINT64_MAX) {}
        node(state s, state p) : node(s, p, DBL_MAX) {}
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
};

template <class state, class action, class environment, class heuristic>
class NBS {
public:
    explicit NBS(state &s, state &g, heuristic &h);
    ~NBS();
    // find the path from start to goal state
    void getPath(environment &e, heuristic &h, state &s, state &g, vector<state> &path);
    // print out the path from start to goal state if found
    void displayPath(vector<state> &path);
private:
    typedef struct node {
        state thisState;
        state parentF;
        state parentB;
        bool closedF = false;
        bool closedB = false;
        double gcostF = DBL_MAX;
        double gcostB = DBL_MAX;
        uint64_t locationOnWaitingF = UINT64_MAX;
        uint64_t locationOnWaitingB = UINT64_MAX;
        uint64_t locationOnReadyF = UINT64_MAX;
        uint64_t locationOnReadyB = UINT64_MAX;
        node(state s, state pF, state pB, bool isClosedF, bool isClosedB, double costF, double costB,
                uint64_t locationWaitingF, uint64_t locationWaitingB, uint64_t locationReadyF, uint64_t locationReadyB)
                : thisState(s), parentF(pF), parentB(pB), closedF(isClosedF), closedB(isClosedB), gcostF(costF), gcostB(costB),
                locationOnWaitingF(locationWaitingF), locationOnWaitingB(locationWaitingB),
                locationOnReadyF(locationReadyF), locationOnReadyB(locationReadyB) {}
        node(state s, state pF, state pB, double costF, double costB) : node(s, pF, pB, false, false, costF, costB, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX) {}
        node(state s, state pF, state pB) : node(s, pF, pB, DBL_MAX, DBL_MAX) {}
        node(state s) : node(s, s, s) {}
    } Node;
    // hash map of states and index in state vector
    unordered_map<state, uint64_t> hashMap;
    // priority queue of waiting lists with key being pair<f-cost, h-cost>
    MinHeap<pair<double, double>, Node> waitingF;
    MinHeap<pair<double, double>, Node> waitingB;
    // priority queue of ready lists with key being g-cost
    MinHeap<double, Node> readyF;
    MinHeap<double, Node> readyB;
    // state vector
    vector<Node> stateVec;
    // lower bound of C
    double clb;
    // meeting state between forward and backward search
    state *meetingPt = nullptr;
    // select best pair from open list
    bool prepareBest();
    // expand node
    void expand(environment &e, heuristic &h, state &s, state &g, vector<action> &actions, uint64_t locationVec,
            bool forward, double &c, uint64_t &nodeUpdatedOpen, uint64_t &nodeUpdatedClosed);

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
//            cout << "Number of nodes generated: " << nodeGenerated << endl;
            cout << "Number of nodes expanded: " << nodeExpanded << endl;
            cout << "Number of nodes in open list: " << queue.size() << endl;
            cout << "Number of nodes in closed list: " << nodeExpanded - nodeReopened << endl;
            cout << "Number of times nodes are updated in the open list: " << nodeUpdated << endl;
            cout << "Number of times nodes are updated in the closed list: " << nodeReopened << endl;

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
    uint64_t nodeExpanded = 0;
    uint64_t nodeUpdated = 0;
    // insert start state into the queue, which is at index 0 in state vector
    open.insertKey(pair<pair<double, double>, uint64_t>{{h->hCost(s, g), h->hCost(s, g)}, 0}, &stateVec, false);
    openHat.insertKey(pair<pair<double, double>, uint64_t>{{weight * h->hCost(s, g), weight * h->hCost(s, g)}, 0}, &stateVec, true);
    vector<action> actions;
    actions.reserve(26);
    vector<state> childStates;
    childStates.reserve(26);
    double fIncumbent = DBL_MAX;
    double fHatIncumbent = DBL_MAX;
    while (bound * open.getMin().first < fIncumbent) {
        if (openHat.getMin().first < fHatIncumbent) {
            pair<pair<double, double>, uint64_t> n = openHat.extractMin(&stateVec, true);

        }
    }
}

template<class state, class action, class environment, class heuristic>
void WeightedAStar<state, action, environment, heuristic>::displayPath(vector<state> &path) {
    for (typename vector<state>::reverse_iterator it = path.rbegin(); it != path.rend(); it++) {
        it->display();
    }
}

template<class state, class action, class environment, class heuristic>
NBS<state, action, environment, heuristic>::NBS(state &s, state &g, heuristic &h) {
    // add start state to hash map and state vector
    Node start(s, s, s, 0, DBL_MAX);
    hashMap.emplace(s, 0);
    stateVec.emplace_back(start);

    // add goal state to hash map and state vector
    Node end(g, g, g, DBL_MAX, 0);
    hashMap.emplace(g, 1);
    stateVec.emplace_back(end);

    meetingPt = new state(s.getStateID());

    clb = 0;
}

template<class state, class action, class environment, class heuristic>
NBS<state, action, environment, heuristic>::~NBS() {
    delete meetingPt;
}

template<class state, class action, class environment, class heuristic>
bool NBS<state, action, environment, heuristic>::prepareBest() {
    pair<pair<double, double>, uint64_t> waitingFmin = waitingF.getMin();
    while (waitingFmin.first.first < clb) {
        // move node from waiting to ready queue
        waitingFmin = waitingF.extractMin(&stateVec, true, true);
        readyF.insertKey(pair<double, uint64_t>{(waitingFmin.first).first - (waitingFmin.first).second, waitingFmin.second}, &stateVec, false, true);
        waitingFmin = waitingF.getMin();

    }
    pair<pair<double, double>, uint64_t> waitingBmin = waitingB.getMin();
    while (waitingBmin.first.first < clb) {
        // move node from waiting to ready queue
        waitingBmin = waitingB.extractMin(&stateVec, true, false);
        readyB.insertKey(pair<double, uint64_t>{(waitingBmin.first).first - (waitingBmin.first).second, waitingBmin.second}, &stateVec, false, false);
        waitingBmin = waitingB.getMin();

    }
    while (true) {
        if (waitingF.size() == 0 && readyF.size() == 0) {
            return false;
        }
        if (waitingB.size() == 0 && readyB.size() == 0) {
            return false;
        }
        pair<double, uint64_t> readyFmin = readyF.getMin();
        pair<double, uint64_t> readyBmin = readyB.getMin();
        if (readyFmin.first + readyBmin.first <= clb) {
            return true;
        }
        // flag of whether a node has been moved from waiting to ready queue
        bool flag = false;
        waitingFmin = waitingF.getMin();
        if (waitingFmin.first.first <= clb) {
            // move node from waiting to ready queue
            waitingFmin = waitingF.extractMin(&stateVec, true, true);
            readyF.insertKey(pair<double, uint64_t>{(waitingFmin.first).first - (waitingFmin.first).second, waitingFmin.second}, &stateVec, false, true);
            flag = true;
        }
        waitingBmin = waitingB.getMin();
        if (waitingBmin.first.first <= clb) {
            // move node from waiting to ready queue
            waitingBmin = waitingB.extractMin(&stateVec, true, false);
            readyB.insertKey(pair<double, uint64_t>{(waitingBmin.first).first - (waitingBmin.first).second, waitingBmin.second}, &stateVec, false, false);
            flag = true;
        }
        if (!flag) {
            // increase C lower bound
            clb = minimum((waitingFmin.first).first, (waitingBmin.first).first, readyFmin.first + readyBmin.first);
        }
    }
}

template<class state, class action, class environment, class heuristic>
void NBS<state, action, environment, heuristic>::expand(environment &e, heuristic &h, state &s, state &g,
        vector<action> &actions, uint64_t locationVec, bool forward, double &c, uint64_t &nodeUpdatedOpen, uint64_t &nodeUpdatedClosed) {
    Node uNode = stateVec[locationVec];
    // move node to closed list
    if (forward) {
        stateVec[locationVec].closedF = true;
    } else {
        stateVec[locationVec].closedB = true;
    }
    e.getActions(uNode.thisState, actions);
    for (auto &act : actions) {
        // go through each child node
        // thread's cancellation point
        pthread_testcancel();
        // make a copy of the current state
        state childState = uNode.thisState;
        // move the childState according to action taken
        e.applyAction(childState, act);
        // check if this child node has been generated
        auto search = hashMap.find(childState);
        bool found = false;
        uint64_t locationVecChild;
        double cChild;
        // calculating gcost of child node
        if (forward) {
            cChild = uNode.gcostF + e.edgeCost(uNode.thisState, childState);
        } else {
            cChild = uNode.gcostB + e.edgeCost(uNode.thisState, childState);
        }
        if (search != hashMap.end()) {
            found = true;
            locationVecChild = search->second;
            // found, child node has been generated before
            if (forward) {
                if (stateVec[search->second].locationOnWaitingB != UINT64_MAX || stateVec[search->second].locationOnReadyB != UINT64_MAX) {
                    // two directions meet
                    if (cChild  + stateVec[search->second].gcostB < c) {
                        // found shorter path from start to goal
                        c = cChild + stateVec[search->second].gcostB;
                        *meetingPt = stateVec[search->second].thisState;
                    }
                }
                if (stateVec[search->second].locationOnWaitingF != UINT64_MAX || stateVec[search->second].locationOnReadyF != UINT64_MAX ||
                        stateVec[search->second].closedF) {
                    if (stateVec[search->second].gcostF <= cChild) {
                        continue;
                    } else {
                        // update new gcost of node
                        stateVec[search->second].parentF = uNode.thisState;
                        stateVec[search->second].gcostF = cChild;
                        if (stateVec[search->second].locationOnWaitingF != UINT64_MAX) {
                            waitingF.decreaseKey(stateVec[search->second].locationOnWaitingF,
                                                 pair<double, double>{cChild + h.hCost(childState, g),
                                                                      h.hCost(childState, g)}, &stateVec, true, true);
                            nodeUpdatedOpen++;
                            continue;
                        } else if (stateVec[search->second].locationOnReadyF != UINT64_MAX) {
                            readyF.decreaseKey(stateVec[search->second].locationOnReadyF, cChild, &stateVec, false,
                                               true);
                            nodeUpdatedOpen++;
                            continue;
                        } else if (stateVec[search->second].closedF) {
                            stateVec[search->second].closedF = false;
                            nodeUpdatedClosed++;
                        }
                    }
                } else {
                    // node has been discovered while searching in opposite direction
                    stateVec[search->second].parentF = uNode.thisState;
                    stateVec[search->second].gcostF = cChild;
                }
            } else {
                if (stateVec[search->second].locationOnWaitingF != UINT64_MAX || stateVec[search->second].locationOnReadyF != UINT64_MAX) {
                    // two directions meet
                    if (cChild  + stateVec[search->second].gcostF < c) {
                        // found shorter path from start to goal
                        c = cChild + stateVec[search->second].gcostF;
                        *meetingPt = stateVec[search->second].thisState;
                    }
                }
                if (stateVec[search->second].locationOnWaitingB != UINT64_MAX || stateVec[search->second].locationOnReadyB != UINT64_MAX ||
                        stateVec[search->second].closedB) {
                    if (stateVec[search->second].gcostB <= cChild) {
                        continue;
                    } else {
                        // update new gcost of node
                        stateVec[search->second].parentB = uNode.thisState;
                        stateVec[search->second].gcostB = cChild;
                        if (stateVec[search->second].locationOnWaitingB != UINT64_MAX) {
                            waitingB.decreaseKey(stateVec[search->second].locationOnWaitingB,
                                                 pair<double, double>{cChild + h.hCost(childState, s),
                                                                      h.hCost(childState, s)}, &stateVec, true, false);
                            nodeUpdatedOpen++;
                            continue;
                        } else if (stateVec[search->second].locationOnReadyB != UINT64_MAX) {
                            readyB.decreaseKey(stateVec[search->second].locationOnReadyB, cChild, &stateVec, false,
                                               false);
                            nodeUpdatedOpen++;
                            continue;
                        } else if (stateVec[search->second].closedB) {
                            stateVec[search->second].closedB = false;
                            nodeUpdatedClosed++;
                        }
                    }
                } else {
                    // node has been discovered while searching in opposite direction
                    stateVec[search->second].parentB = uNode.thisState;
                    stateVec[search->second].gcostB = cChild;
                }
            }
        }
        if (!found) {
            // create new node
            Node childNode(childState);
            if (forward) {
                childNode.parentF = uNode.thisState;
                childNode.gcostF = cChild;
            } else {
                childNode.parentB = uNode.thisState;
                childNode.gcostB = cChild;
            }
            // add node to state vector and hash table
            stateVec.emplace_back(childNode);
            hashMap.emplace(childState, stateVec.size() - 1);
            locationVecChild = stateVec.size() - 1;
        }
        // insert node into waiting queue, both when node is new or node has been discovered but in opposite direction
        if (forward) {
            uint64_t location = waitingF.insertKey(pair<pair<double, double>, uint64_t>
                                                           {{cChild + h.hCost(childState, g), h.hCost(childState, g)},
                                                            locationVecChild}, &stateVec, true, true);
        } else {
            uint64_t location = waitingB.insertKey(pair<pair<double, double>, uint64_t>
                                                           {{cChild + h.hCost(childState, s), h.hCost(childState, s)},
                                                            locationVecChild}, &stateVec, true, false);
        }
    }
    actions.clear();
}

template <class state, class action, class environment, class heuristic>
void NBS<state, action, environment, heuristic>::getPath(environment &e, heuristic &h, state &s, state &g,
                                                           vector<state> &path) {
    if (s == g) {
        path.emplace_back(g);
        cout << "Cost of solution: 0" << endl;
        cout << "Number of nodes expanded: 0" << endl;
        cout << "Number of nodes in open list: 0" << endl;
        cout << "Number of nodes in closed list: 0" << endl;
        cout << "Number of times nodes are updated in the open list: 0" << endl;
        cout << "Number of times nodes are updated in the closed list: 0" << endl;
        return;
    }
    uint64_t nodeExpanded = 0;
    uint64_t nodeUpdatedOpen = 0;
    uint64_t nodeUpdatedClosed = 0;
    double c = DBL_MAX;
    // insert start state into the waitingF queue, which is at index 0 in state vector
    waitingF.insertKey(pair<pair<double, double>, uint64_t>{{h.hCost(s, g), h.hCost(s, g)}, 0}, &stateVec, true, true);
    // insert goal state into the waitingB queue, which is at index 1 in state vector
    waitingB.insertKey(pair<pair<double, double>, uint64_t>{{h.hCost(g, s), h.hCost(g, s)}, 1}, &stateVec, true, false);
    vector<action> actions;
    actions.reserve(26);

    while (prepareBest()) {
        pair<double, uint64_t> u = readyF.getMin();
        pair<double, uint64_t> v = readyB.getMin();
        double lb = maximum(u.first + h.hCost(stateVec[u.second].thisState, g), v.first + h.hCost(stateVec[v.second].thisState, s), u.first + v.first);
        if (lb >= c) {
            // found path
            // record the path from goal to start
            path.emplace_back(*meetingPt);
            state temp = *meetingPt;

            while (true) {
                // insert states from meeting point to the start state into the path vector
                // each state is inserted to the end of the vector
                uint64_t index = hashMap[temp];
                if (stateVec[index].parentF == stateVec[index].thisState) {
                    break;
                } else {
                    path.emplace_back(stateVec[index].parentF);
                }
                temp = stateVec[index].parentF;
            }
            temp = *meetingPt;
            while (true) {
                // insert states from meeting point to the end state into the path vector
                // each state is inserted to the start of the vector
                uint64_t index = hashMap[temp];
                if (stateVec[index].parentB == stateVec[index].thisState) {
                    break;
                } else {
                    path.emplace(path.begin(), stateVec[index].parentB);
                }
                temp = stateVec[index].parentB;
            }
            cout << "Cost of solution: " << c << endl;
            cout << "Number of nodes expanded: " << nodeExpanded << endl;
            cout << "Number of nodes in open list: " << waitingF.size() + readyF.size() + waitingB.size() + readyB.size() << endl;
            cout << "Number of nodes in closed list: " << nodeExpanded - nodeUpdatedClosed << endl;
            cout << "Number of times nodes are updated in the open list: " << nodeUpdatedOpen << endl;
            cout << "Number of times nodes are updated in the closed list: " << nodeUpdatedClosed << endl;
            return;
        }
        // expand u and v
        u = readyF.extractMin(&stateVec, false, true);
        expand(e, h, s, g, actions, u.second, true, c, nodeUpdatedOpen, nodeUpdatedClosed);
        v = readyB.extractMin(&stateVec, false, false);
        expand(e, h, s, g, actions, v.second, false, c, nodeUpdatedOpen, nodeUpdatedClosed);
        nodeExpanded += 2;
    }
}

template<class state, class action, class environment, class heuristic>
void NBS<state, action, environment, heuristic>::displayPath(vector<state> &path) {
    for (typename vector<state>::reverse_iterator it = path.rbegin(); it != path.rend(); it++) {
        it->display();
    }
}

template <class state, class action, class environment, class heuristic>
void IteratedDeepening<state, action, environment, heuristic>::getPath(environment &e, heuristic &h, state &s, state &g,
                                                          vector<state> &path, bool dfid) {
    bool found = false;
    bool remaining = true;
    uint64_t nodeExpanded = 0;
    double bound;
    if (dfid) {
        bound = -1;
    } else { // IDAstar
        bound = h.hCost(s, g);
    }
    while (bound < DBL_MAX) {
        bound = dls(e, h, s, s, g, 0, bound, found, remaining, nodeExpanded, path, dfid);
        if (found) {
            // reached goal state
            cout << "Cost of solution: " << bound << endl;
            cout << "Number of nodes expanded: " << nodeExpanded << endl;
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
    nodeCount++;

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
