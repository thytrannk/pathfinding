//
// Created by Katie Tran on 2019-03-05.
//

#ifndef ASSIGNMENT3_PDB_H
#define ASSIGNMENT3_PDB_H

#include <vector>
#include <queue>
#include <iostream>
#include <climits>

using namespace std;

template <class state, class action, class environment>
class PDB {
public:
    // build tree
    void buildTreeDFID(environment *e, state &s, uint32_t sum, vector<uint8_t> &pdb);
    void buildTreeBFS(environment *e, state &s, uint32_t sum, vector<uint8_t> &pdb);
private:
    // depth limited search
    void dls(environment *e, state &current, state &parent, uint8_t depth, uint32_t &nodeCount, vector<uint8_t> &pdb);
    uint8_t bound;
};

template<class state, class action, class environment>
void PDB<state, action, environment>::buildTreeBFS(environment *e, state &s, uint32_t sum,
                                                               vector<uint8_t> &pdb) {
    uint32_t nodeExpanded = 0;
    uint8_t depth = 0;
    // open list queue, elements are pair of state and cost from start
    queue<pair<state, uint8_t>> open;
    open.emplace(s, 0);
    while (!open.empty()) {
        pair<state, uint8_t> current = open.front();
        open.pop();
        if (current.second > depth) {
            // enter next depth level
            cout << "Depth: " << (int) depth << "; Nodes: " << nodeExpanded << endl;
            depth++;
        }
        if (pdb[current.first.getStateID()] != UINT8_MAX) {
            // state has been discovered
            continue;
        }
        pdb[current.first.getStateID()] = current.second;
        nodeExpanded++;
        vector<action> actions;
        e->getActions(current.first, actions);

        for (auto &act : actions) {
            // go through each neighboring node
            // make a copy of the current state
            state childState = current.first;
            // move the childState according to action taken
            e->applyAction(childState, act);
//            if (pdb[childState.getStateID()] != UINT8_MAX) {
//                // child state has been discovered
//                continue;
//            }
            open.emplace(childState, current.second + 1);
        }
    }
}

template<class state, class action, class environment>
void PDB<state, action, environment>::buildTreeDFID(environment *e, state &s, uint32_t sum,
                                                               vector<uint8_t> &pdb) {
    pdb[0] = 0;
    uint32_t nodeExpanded = 1;
    for (bound = 0; bound < UINT8_MAX; bound++) {
        cout << "Depth: " << (int) bound << "; ";
        dls(e, s, s, bound, nodeExpanded, pdb);
        cout << "Nodes: " << nodeExpanded << endl;
        if (nodeExpanded == sum) {
            return;
        }
    }
    cerr << "Error: Reached maximum depth." << endl;
    exit(EXIT_FAILURE);
}

template<class state, class action, class environment>
void PDB<state, action, environment>::dls(environment *e, state &current, state &parent, uint8_t depth,
                                                uint32_t &nodeCount, vector<uint8_t> &pdb) {
    uint8_t cost = pdb[current.getStateID()];
    if (depth == 0) {
        if (cost == 0 && current.getStateID() != 0) {
            // New node discovered. Store heuristic into PDB vector
            pdb[current.getStateID()] = bound;
            nodeCount++;
        }
        return;
    }
    if (cost + depth != bound) {
        // Not the shortest path. Node has been found at a shorter path. Do not expand this path
        return;
    }
    vector<action> actions;
    e->getActions(current, actions);

    for (auto &act : actions) {
        // go through each neighboring node
        // make a copy of the current state
        state childState = current;
        // move the childState according to action taken
        e->applyAction(childState, act);
        if (childState == parent) {
            continue;
        }
        dls(e, childState, current, depth - 1, nodeCount, pdb);
    }
}

#endif //ASSIGNMENT3_PDB_H
