//
// Created by Katie Tran on 2019-03-23.
//

#ifndef ASSIGNMENT4_DIFFERENTIALHEURISTIC_H
#define ASSIGNMENT4_DIFFERENTIALHEURISTIC_H

#include <vector>
#include <unordered_map>
#include <cfloat>
#include <climits>
#include "minHeap.h"

template <class state, class action, class environment>
class DifferentialHeuristic {
public:
    DifferentialHeuristic() = default;
    void random(environment &e, int n, vector<vector<double>> &costs);
    vector<state> furthest(environment &e, int n, vector<vector<double>> &costs);
    void optimized(environment &e, int n, vector<vector<double>> &costs);
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
    // save cost of getting to closest pivot of all locations in cost vector, return furthest state
    state buildHeuristic(environment &e, vector<state> &pivots, vector<double> &cost);
    // return heuristic from i to j
    double diffCost(unsigned long i, unsigned long j, vector<double> &cost);
    // add the maximum heuristic between i and j from a set of heuristic to sum vector
    // Set of heuristic includes the ones in costs[c] where c is from 0 to chosen - 1 (the ones that have been picked from the pool)
    // and each candidate heuristic in costsFurthest vector
    // The maximum heuristic of set of chosen heuristics + costsFurthest[k] will be added to sum[k]
    // chosen: the index of additional heuristic that is being chosen from the pool
    void addMaxH(vector<vector<double>> &costsFurthest, unsigned long i, unsigned long j, int chosen, vector<vector<double>> &costs,
                 vector<double> &sum);
    // convert coordinate (x, y) or (x, y, z) to index in heuristic vector
    unsigned long toIndex(int x, int y, int xMap, int yMap);
    unsigned long toIndex(int x, int y, int z, int xMap, int yMap, int zMap);
};

template <class state, class action, class environment>
state DifferentialHeuristic<state, action, environment>::buildHeuristic(environment &e, vector<state> &pivots, vector<double> &cost) {
    // hash map of states and index in state vector
    unordered_map<state, uint64_t> hashMap;
    // priority queue of open list with key being g-cost and index to stateVec
    MinHeap<double, Node> queue;
    // state vector
    vector<Node> stateVec;
    // Do Dijkstra search around pivots
    state ret = pivots[0];
    for (int i = 0; i < pivots.size(); i++) {
        Node init(pivots[i], pivots[i], 0);
        hashMap.emplace(pivots[i], i);
        stateVec.emplace_back(init);
        // insert start state into the queue, which is at index 0 in state vector
        queue.insertKey(pair<double, uint64_t>{0, i}, &stateVec);
    }
    vector<action> actions;
    actions.reserve(26);
    while (queue.size() != 0) {
        pair<double, uint64_t> current = queue.extractMin(&stateVec);
        Node thisNode = stateVec[current.second];
        if (queue.size() == 0) {
            ret = thisNode.thisState;
        }
        stateVec[current.second].closed = true;
        e.getActions(thisNode.thisState, actions);
        for (auto &act : actions) {
            // go through each child node
            // thread's cancellation point
            pthread_testcancel();
            // make a copy of the current state
            state childState = thisNode.thisState;
            // move the childState according to action taken
            e.applyAction(childState, act);
            // check if this child node has been generated
            auto search = hashMap.find(childState);
            double tentative_gcost = thisNode.gcost + e.edgeCost(thisNode.thisState, childState);
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
                        stateVec[search->second].parent = thisNode.thisState;
                        stateVec[search->second].gcost = tentative_gcost;
                        queue.decreaseKey(stateVec[search->second].locationOnOpen,
                                          tentative_gcost, &stateVec);
                    }
                }
            } else {
                // not found (newly discovered node)
                Node childNode(childState, thisNode.thisState, tentative_gcost);
                // add node to state vector and hash table
                stateVec.emplace_back(childNode);
                hashMap.emplace(childState, stateVec.size() - 1);
                uint64_t location = queue.insertKey(pair<double, uint64_t>
                                                            {tentative_gcost,
                                                             stateVec.size() - 1}, &stateVec);
            }
        }
        actions.clear();
    }
    // save heuristic cost of each state to vector
    int dim = e.getDim();
    int xMap = 0;
    int yMap = 0;
    int zMap = 0;
    if (dim == 2) {
        // 2D grid
        e.getLength(xMap, yMap);
    } else if (dim == 3) {
        // 3D voxel
        e.getLength(xMap, yMap, zMap);
    }
    for (auto &n : stateVec) {
        state s = n.thisState;
        int x = s.getValue(0);
        int y = s.getValue(1);
        if (dim == 2) {
            // 2D grid
            cost[toIndex(x, y, xMap, yMap)] = n.gcost;
        } else if (dim == 3) {
            // 3D voxel
            int z = s.getValue(2);
            cost[toIndex(x, y, z, xMap, yMap, zMap)] = n.gcost;
        }
    }
    return ret;
}

template<class state, class action, class environment>
void DifferentialHeuristic<state, action, environment>::random(environment &e, int n, vector<vector<double>> &costs) {
    // generate n random locations
    vector<int> coor;
    e.placeRandom(n, coor);
    for (int i = 0; i < n; i++) {
        // save coordinates of random location to pivots vector
        vector<state> pivots;
        if (e.getDim() == 2) {
            int arr[2] = {coor[2 * i], coor[2 * i + 1]};
            pivots.emplace_back(arr);
        } else if (e.getDim() == 3) {
            int arr[3] = {coor[3 * i], coor[3 * i + 1], coor[3 * i + 2]};
            pivots.emplace_back(arr);
        }
        // do search and save heuristic cost to costs vector
        buildHeuristic(e, pivots, costs[i]);
    }
}

template<class state, class action, class environment>
vector<state> DifferentialHeuristic<state, action, environment>::furthest(environment &e, int n, vector<vector<double>> &costs) {
    // generate a random location
    vector<int> coor;
    e.placeRandom(1, coor);
    // save coordinates of random location to pivotsRand vector
    vector<state> pivotsRand;
    if (e.getDim() == 2) {
        int arr[2] = {coor[0], coor[1]};
        pivotsRand.emplace_back(arr);
    } else if (e.getDim() == 3) {
        int arr[3] = {coor[0], coor[1], coor[2]};
        pivotsRand.emplace_back(arr);
    }
    // find the furthest location from the random pivot
    vector<double> temp = costs[0];
    state s = buildHeuristic(e, pivotsRand, temp);
    vector<state> pivotsAll;
    for (int i = 0; i < n; i++) {
        // add furthest location to pivots vector
        vector<state> pivots;
        pivots.emplace_back(s);
        pivotsAll.emplace_back(s);
        // do search and save heuristic cost to costs vector
        buildHeuristic(e, pivots, costs[i]);
        // Find furthest point from all existing pivots
        s = buildHeuristic(e, pivotsAll, temp);
    }
    return pivotsAll;
}

template<class state, class action, class environment>
void DifferentialHeuristic<state, action, environment>::optimized(environment &e, int n, vector<vector<double>> &costs) {
    unsigned long spaceSize = costs[0].size();
    // dim is either 2 or 3 for 2D/3D domain
    int dim = e.getDim();

    int xMap = 0;
    int yMap = 0;
    int zMap = 0;
    if (dim == 2) {
        e.getLength(xMap, yMap);
    } else if (dim == 3) {
        e.getLength(xMap, yMap, zMap);
    }

    // vector to save costs of 3*n heuristic built using furthest placement strategy
    vector<vector<double>> costsFurthest((unsigned long) 3 * n, costs[0]);
    // generate 3*n heuristics using furthest placement strategy
    vector<state> pivotsAll = furthest(e, 3 * n, costsFurthest);

    // sampling 50 states from the state space, excluding the pivots
    vector<int> coor;
    vector<int> exclude;
    for (auto &p : pivotsAll) {
        exclude.emplace_back(p.getValue(0));
        exclude.emplace_back(p.getValue(1));
        if (dim == 3) {
            exclude.emplace_back(p.getValue(2));
        }
    }
    e.placeRandom(50, coor, exclude);

    for (int chosen = 0; chosen < n; chosen++) {
        cout << "chosen " << chosen << endl;
        // vector to save sum of max heuristic of all locations for each remaining candidate
        // eg. there are 2 chosen pivots and 28 remaining candidates
        // then sum[c] with c from 0 to 27 would store the sum of heuristic of all pairs of i,j (i < j) of candidate c
        // where each heuristic h(i,j) is the maximum of heuristic from 2 chosen pivots and candidate c
        vector<double> sum = vector<double>(costsFurthest.size(), 0);

        unsigned long i = 0;
        unsigned long j = 0;
        // compute max heuristic between every 2 points using existing pivots + each remaining candidate

        // coor vector is alternating x -> y (-> z) of each random location
        for (int iX = 0; iX < coor.size() - dim; iX += dim) {
            for (int jX = iX + dim; jX < coor.size(); jX += dim) {
                if (dim == 2) {
                    // 2D grid
                    i = toIndex(coor[iX], coor[iX + 1], xMap, yMap);
                    j = toIndex(coor[jX], coor[jX + 1], xMap, yMap);
                } else if (dim == 3) {
                    // 3D voxel
                    i = toIndex(coor[iX], coor[iX + 1], coor[iX + 2], xMap, yMap, zMap);
                    j = toIndex(coor[jX], coor[jX + 1], coor[jX + 2], xMap, yMap, zMap);
                }
                addMaxH(costsFurthest, i, j, chosen, costs, sum);
            }
        }
        // find the best candidate among the pool, which is the one that gives largest sum of heuristic values when combined with existing pivots
        auto maxIterator = max_element(sum.begin(), sum.end());
        auto maxIndex = distance(sum.begin(), maxIterator);
        // find the location in the costsFurthest vector
        auto costsFurthestIter = costsFurthest.begin();
        advance(costsFurthestIter, maxIndex);
        // copy the heuristic of the best candidate to the costs vector
        costs[chosen] = costsFurthest[maxIndex];
        // erase the best candidate from the pool of candidates
        costsFurthest.erase(costsFurthestIter);
    }
}

template<class state, class action, class environment>
void DifferentialHeuristic<state, action, environment>::addMaxH(vector<vector<double>> &costsFurthest, unsigned long i, unsigned long j,
                                                                int chosen, vector<vector<double>> &costs,
                                                                vector<double> &sum) {
    if (costsFurthest[0][i] == DBL_MAX || costsFurthest[0][j] == DBL_MAX) {
        // either i or j is a blocked location, ignore
        return;
    }
    for (int candidate = 0; candidate < costsFurthest.size(); candidate++) {
        double maximum = diffCost(i, j, costsFurthest[candidate]);
        for (int current = 0; current < chosen; current++) {
            double temp = diffCost(i, j, costs[current]);
            if (temp > maximum) {
                maximum = temp;
            }
        }
        sum[candidate] += maximum;
    }
}

template<class state, class action, class environment>
double DifferentialHeuristic<state, action, environment>::diffCost(unsigned long i, unsigned long j, vector<double> &cost) {
    return abs(cost[i] - cost[j]);
}

template<class state, class action, class environment>
unsigned long DifferentialHeuristic<state, action, environment>::toIndex(int x, int y, int xMap, int yMap) {
    return (unsigned long) y * xMap + x;
}

template<class state, class action, class environment>
unsigned long DifferentialHeuristic<state, action, environment>::toIndex(int x, int y, int z, int xMap, int yMap, int zMap) {
    return (unsigned long) z * xMap * yMap + (unsigned long) y * xMap + x;
}

#endif //ASSIGNMENT4_DIFFERENTIALHEURISTIC_H
