//
// Created by Katie Tran on 2019-03-05.
//

#include <iostream>
#include <fstream>
#include "puzzle.h"
#include "puzzlePDB.h"
#include "pdb.h"

using namespace std;

void usage(char *programName) {
    cerr << "Usage: " << programName << " <which_pattern> <output_file_name>" << endl
         << "where" << endl
         << "which_pattern: \"1\" for tiles 0-7, \"2\" for 0,8-9,12-13, and \"3\" for 0,10-11,14-15" << endl
         << "output_file_name: name of the output file to store the pattern database" << endl;
}

//class Base {
//public:
//    virtual void func() {
//        cout << "Base" << endl;
//    }
//    void another() {
//        func();
//    }
//};
//
//class Derived : public Base {
//public:
//    void func() override {
//        cout << "Derived" << endl;
//    }
//};

int main(int argc, char **argv) {
//    Base *test = new Derived();
//    cout << "func" << endl;
//    test->func();
//    cout << "another" << endl;
//    test->another();
//    return 0;
    if (argc != 3) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    string fileName = argv[2];
    ofstream myFile(fileName);
    if (myFile.is_open()) {
        vector<int8_t> stateArr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        vector<int8_t> pattern;
        if (atoi(argv[1]) == 1) {
            pattern = {0, 1, 2, 3, 4, 5, 6, 7};
        } else if (atoi(argv[1]) == 2) {
            pattern = {0, 8, 9, 12, 13};
        } else if (atoi(argv[1]) == 3) {
            pattern = {0, 10, 11, 14, 15};
        } else {
            cerr << "Invalid input." << endl;
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        uint32_t sum = factorialDiv(16, (int) (16 - pattern.size()));
        vector<uint8_t> pdbArr(sum, UINT8_MAX);
        StatePuzzleLexi initialState(stateArr, &pattern);
        auto *e = new EnvironmentPuzzle<StatePuzzleLexi>;
        PDB<StatePuzzleLexi, ActionPuzzle, EnvironmentPuzzle<StatePuzzleLexi>> pdb{};
        pdb.buildTreeBFS(e, initialState, sum, pdbArr);
        delete e;

        for (auto &h : pdbArr) {
            myFile << (int) h << " ";
        }
        myFile.close();
        return 0;
    } else {
        cerr << "Cannot open file." << endl;
        exit(EXIT_FAILURE);
    }
}