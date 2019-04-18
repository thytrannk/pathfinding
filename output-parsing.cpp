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

//int main(int argc, char **argv) {
//    if (argc != 2) {
//        usage(argv[0]);
//        exit(EXIT_FAILURE);
//    }
//    string fileName = argv[1];
//    ifstream outFile(fileName);
//    int totalNo = 0;
//    vector<uint64_t> nodeExpandedVec;
//    vector<uint64_t> nodeOpenVec;
//    vector<uint64_t> nodeClosedVec;
//    vector<uint64_t> nodeUpdatedOpenVec;
//    vector<uint64_t> nodeUpdatedClosedVec;
//    vector<uint64_t> nodeDuplicateVec;
//    vector<int> timeVec;
//    uint64_t nodeExpanded;
//    uint64_t nodeOpen;
//    uint64_t nodeClosed;
//    uint64_t nodeUpdatedOpen;
//    uint64_t nodeUpdatedClosed;
//    uint64_t nodeDuplicate;
//    int time;
//    int solved;
//    if (outFile.is_open()) {
//        string line;
//        while (getline(outFile, line)) {
//            // ignore empty lines
//            if (line.length() == 0) {
//                getline(outFile, line);
//            }
//            istringstream lineStream(line);
//            string firstWord;
//            getline(lineStream, firstWord, ' ');
//            if (firstWord == "Initial") {
//                totalNo++;
//                continue;
//            } else if (firstWord == "Problem" || firstWord == "Goal" || firstWord == "Cost" || firstWord == "Time") {
//                continue;
//            } else if (firstWord == "Solved") {
//                string ignore;
//                getline(lineStream, ignore, 'n');
//                lineStream >> time;
//                timeVec.emplace_back(time);
//                solved++;
//            } else if (firstWord == "Final") {
//                break;
//            }
//            string content;
//            getline(lineStream, content, ':');
//            if (content == "of nodes expanded") {
//                lineStream >> nodeExpanded;
//                nodeExpandedVec.emplace_back(nodeExpanded);
//            } else if (content == "of nodes in open list") {
//                lineStream >> nodeOpen;
//                nodeOpenVec.emplace_back(nodeOpen);
//            } else if (content == "of nodes in closed list") {
//                lineStream >> nodeClosed;
//                nodeClosedVec.emplace_back(nodeClosed);
//            } else if (content == "of times nodes are updated in the open list") {
//                lineStream >> nodeUpdatedOpen;
//                nodeUpdatedOpenVec.emplace_back(nodeUpdatedOpen);
//            } else if (content == "of times nodes are updated in the closed list") {
//                lineStream >> nodeUpdatedClosed;
//                nodeUpdatedClosedVec.emplace_back(nodeUpdatedClosed);
//            } else if (content == "of times a node that is already expanded is removed from the open list") {
//                lineStream >> nodeDuplicate;
//                nodeDuplicateVec.emplace_back(nodeDuplicate);
//            }
//        }
//        cout << "Total number of problems attempted: " << totalNo << endl;
//        cout << "Total number of problems solved: " << solved << endl;
//        if (!nodeExpandedVec.empty()) {
//            cout << "Number of nodes expanded:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeExpandedVec.begin(), nodeExpandedVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeExpandedVec.begin(), nodeExpandedVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeExpandedVec) << endl;
//        }
//        if (!nodeOpenVec.empty()) {
//            cout << "Number of nodes in open list:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeOpenVec.begin(), nodeOpenVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeOpenVec.begin(), nodeOpenVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeOpenVec) << endl;
//        }
//        if (!nodeClosedVec.empty()) {
//            cout << "Number of nodes in closed list:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeClosedVec.begin(), nodeClosedVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeClosedVec.begin(), nodeClosedVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeClosedVec) << endl;
//        }
//        if (!nodeUpdatedOpenVec.empty()) {
//            cout << "Number of times nodes are updated in the open list:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeUpdatedOpenVec.begin(), nodeUpdatedOpenVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeUpdatedOpenVec.begin(), nodeUpdatedOpenVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeUpdatedOpenVec) << endl;
//        }
//        if (!nodeDuplicateVec.empty()) {
//            cout << "Number of times a node that is already expanded is removed from the open list:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeDuplicateVec.begin(), nodeDuplicateVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeDuplicateVec.begin(), nodeDuplicateVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeDuplicateVec) << endl;
//        }
//        if (!nodeUpdatedClosedVec.empty()) {
//            cout << "Number of times nodes are updated in the closed list:" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(nodeUpdatedClosedVec.begin(), nodeUpdatedClosedVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(nodeUpdatedClosedVec.begin(), nodeUpdatedClosedVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<uint64_t>(nodeUpdatedClosedVec) << endl;
//        }
//        if (!timeVec.empty()) {
//            cout << "Time to solve a problem (in seconds):" << endl;
//            cout << "\t" << "Minimum: " << *(min_element(timeVec.begin(), timeVec.end())) << endl;
//            cout << "\t" << "Maximum: " << *(max_element(timeVec.begin(), timeVec.end())) << endl;
//            cout << "\t" << "Average: " << (int) vectAvg<int>(timeVec) << endl;
//        }
//    } else {
//        cerr << "Cannot open file." << endl;
//        exit(EXIT_FAILURE);
//    }
//    return 0;
//}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    string fileName = argv[1];
    ifstream outFile(fileName);
//    string fileName2 = argv[2];
//    ifstream outFile2(fileName2);
    int totalNo = 0;
    vector<vector<double>> nodeExpandedVecOpt(9);
    vector<vector<double>> nodeExpandedVecWeight(9);
    vector<uint64_t> nodeExpandedVecAStar;

    uint64_t nodeExpanded;
    int index;
    int opt;

//    if (outFile.is_open()) {
//        string line;
//        while (getline(outFile, line)) {
//            // ignore empty lines
//            if (line.length() == 0) {
//                getline(outFile, line);
//            }
//            istringstream lineStream(line);
//            string firstWord;
//            getline(lineStream, firstWord, ' ');
//            if (firstWord == "Initial") {
//                continue;
//            } else if (firstWord == "Problem" || firstWord == "Goal" || firstWord == "Cost" || firstWord == "Time" || firstWord == "Solved") {
//                continue;
//            } else if (firstWord == "Final") {
//                break;
//            } else if (firstWord == "Number") {
//                string content;
//                getline(lineStream, content, ':');
//                lineStream >> nodeExpanded;
//                nodeExpandedVecAStar.emplace_back(nodeExpanded);
//            }
//        }
//    } else {
//        cerr << "Cannot open file." << endl;
//        exit(EXIT_FAILURE);
//    }

    if (outFile.is_open()) {
        string line;
        while (getline(outFile, line)) {
            // ignore empty lines
            if (line.length() == 0) {
                getline(outFile, line);
            }
            if (line == "Optimistic:") {
                opt = 0;
                continue;
            } else if (line == "Weighted A*:") {
                opt = 1;
                continue;
            } else if (line == "A*:") {
                opt = 2;
            }
            istringstream lineStream(line);
            string firstWord;
            getline(lineStream, firstWord, ' ');
            if (firstWord == "Problem") {
                totalNo++;
                continue;
            } else if (firstWord == "Initial" || firstWord == "Goal" || firstWord == "Cost" || firstWord == "Time" || firstWord == "Solved") {
                continue;
            } else if (firstWord == "Final") {
                break;
            } else if (firstWord == "Bound") {
                string content;
                getline(lineStream, content);
                if (content == "1.1:") {
                    index = 0;
                } else if (content == "1.2:") {
                    index = 1;
                } else if (content == "1.3:") {
                    index = 2;
                } else if (content == "1.4:") {
                    index = 3;
                } else if (content == "1.5:") {
                    index = 4;
                } else if (content == "1.6:") {
                    index = 5;
                } else if (content == "1.7:") {
                    index = 6;
                } else if (content == "1.8:") {
                    index = 7;
                } else if (content == "2.0:") {
                    index = 8;
                } else {
                    cout << content << endl;
                    exit(0);
                }
                continue;
            } else if (firstWord == "Number") {
                string content;
                getline(lineStream, content, ':');
                lineStream >> nodeExpanded;
                if (opt == 0) {
                    nodeExpandedVecOpt[index].emplace_back((double) nodeExpanded/nodeExpandedVecAStar[totalNo-1]);
                } else if (opt == 1) {
                    nodeExpandedVecWeight[index].emplace_back((double) nodeExpanded/nodeExpandedVecAStar[totalNo-1]);
                } else if (opt == 2) {
                    nodeExpandedVecAStar.emplace_back(nodeExpanded);
                }
            }
        }
        cout << "Total number of problems attempted: " << totalNo << endl;
        cout << "Optimistic:" << endl;
        for (int i = 0; i < 9; i++) {
            if (!nodeExpandedVecOpt[i].empty()) {
//                cout << i << "  Number of nodes expanded:" << endl;
//                cout << "\t" << "Minimum: " << *(min_element(nodeExpandedVecOpt[i].begin(), nodeExpandedVecOpt[i].end())) << endl;
//                cout << "\t" << "Maximum: " << *(max_element(nodeExpandedVecOpt[i].begin(), nodeExpandedVecOpt[i].end())) << endl;
                cout << i << "\t" << "Average: " << (double) vectAvg<double>(nodeExpandedVecOpt[i]) << endl;
            }
        }
        cout << "Weighted A*:" << endl;
        for (int i = 0; i < 9; i++) {
            if (!nodeExpandedVecWeight[i].empty()) {
//                cout << i << "  Number of nodes expanded:" << endl;
//                cout << "\t" << "Minimum: " << *(min_element(nodeExpandedVecWeight[i].begin(), nodeExpandedVecWeight[i].end())) << endl;
//                cout << "\t" << "Maximum: " << *(max_element(nodeExpandedVecWeight[i].begin(), nodeExpandedVecWeight[i].end())) << endl;
                cout << i << "\t" << "Average: " << (double) vectAvg<double>(nodeExpandedVecWeight[i]) << endl;
            }
        }
    } else {
        cerr << "Cannot open file." << endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}