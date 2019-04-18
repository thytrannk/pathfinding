//
// Created by Katie Tran on 2019-02-14.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "util.h"

using namespace std;

void usage(char *programName) {
    cerr << "Usage: " << programName << " <fileName>" << endl
        << "where fileName is the name of the output file to be parsed." << endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    string fileName = argv[1];
    ifstream outFile(fileName);
    int totalNo = 0;
    vector<uint64_t> nodeExpandedVec;
    vector<uint64_t> nodeOpenVec;
    vector<uint64_t> nodeClosedVec;
    vector<uint64_t> nodeUpdatedOpenVec;
    vector<uint64_t> nodeUpdatedClosedVec;
    vector<uint64_t> nodeDuplicateVec;
    vector<int> timeVec;
    uint64_t nodeExpanded;
    uint64_t nodeOpen;
    uint64_t nodeClosed;
    uint64_t nodeUpdatedOpen;
    uint64_t nodeUpdatedClosed;
    uint64_t nodeDuplicate;
    int time;
    int solved;
    if (outFile.is_open()) {
        string line;
        while (getline(outFile, line)) {
            // ignore empty lines
            if (line.length() == 0) {
                getline(outFile, line);
            }
            istringstream lineStream(line);
            string firstWord;
            getline(lineStream, firstWord, ' ');
            if (firstWord == "Initial") {
                totalNo++;
                continue;
            } else if (firstWord == "Problem" || firstWord == "Goal" || firstWord == "Cost" || firstWord == "Time") {
                continue;
            } else if (firstWord == "Solved") {
                string ignore;
                getline(lineStream, ignore, 'n');
                lineStream >> time;
                timeVec.emplace_back(time);
                solved++;
            } else if (firstWord == "Final") {
                break;
            }
            string content;
            getline(lineStream, content, ':');
            if (content == "of nodes expanded") {
                lineStream >> nodeExpanded;
                nodeExpandedVec.emplace_back(nodeExpanded);
            } else if (content == "of nodes in open list") {
                lineStream >> nodeOpen;
                nodeOpenVec.emplace_back(nodeOpen);
            } else if (content == "of nodes in closed list") {
                lineStream >> nodeClosed;
                nodeClosedVec.emplace_back(nodeClosed);
            } else if (content == "of times nodes are updated in the open list") {
                lineStream >> nodeUpdatedOpen;
                nodeUpdatedOpenVec.emplace_back(nodeUpdatedOpen);
            } else if (content == "of times nodes are updated in the closed list") {
                lineStream >> nodeUpdatedClosed;
                nodeUpdatedClosedVec.emplace_back(nodeUpdatedClosed);
            } else if (content == "of times a node that is already expanded is removed from the open list") {
                lineStream >> nodeDuplicate;
                nodeDuplicateVec.emplace_back(nodeDuplicate);
            }
        }
        cout << "Total number of problems attempted: " << totalNo << endl;
        cout << "Total number of problems solved: " << solved << endl;
        if (!nodeExpandedVec.empty()) {
            cout << "Number of nodes expanded:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeExpandedVec.begin(), nodeExpandedVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeExpandedVec.begin(), nodeExpandedVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeExpandedVec) << endl;
        }
        if (!nodeOpenVec.empty()) {
            cout << "Number of nodes in open list:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeOpenVec.begin(), nodeOpenVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeOpenVec.begin(), nodeOpenVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeOpenVec) << endl;
        }
        if (!nodeClosedVec.empty()) {
            cout << "Number of nodes in closed list:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeClosedVec.begin(), nodeClosedVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeClosedVec.begin(), nodeClosedVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeClosedVec) << endl;
        }
        if (!nodeUpdatedOpenVec.empty()) {
            cout << "Number of times nodes are updated in the open list:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeUpdatedOpenVec.begin(), nodeUpdatedOpenVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeUpdatedOpenVec.begin(), nodeUpdatedOpenVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeUpdatedOpenVec) << endl;
        }
        if (!nodeDuplicateVec.empty()) {
            cout << "Number of times a node that is already expanded is removed from the open list:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeDuplicateVec.begin(), nodeDuplicateVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeDuplicateVec.begin(), nodeDuplicateVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeDuplicateVec) << endl;
        }
        if (!nodeUpdatedClosedVec.empty()) {
            cout << "Number of times nodes are updated in the closed list:" << endl;
            cout << "\t" << "Minimum: " << *(min_element(nodeUpdatedClosedVec.begin(), nodeUpdatedClosedVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(nodeUpdatedClosedVec.begin(), nodeUpdatedClosedVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeUpdatedClosedVec) << endl;
        }
        if (!timeVec.empty()) {
            cout << "Time to solve a problem (in seconds):" << endl;
            cout << "\t" << "Minimum: " << *(min_element(timeVec.begin(), timeVec.end())) << endl;
            cout << "\t" << "Maximum: " << *(max_element(timeVec.begin(), timeVec.end())) << endl;
            cout << "\t" << "Average: " << (int) vectAvg<int>(timeVec) << endl;
        }
    } else {
        cerr << "Cannot open file." << endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}