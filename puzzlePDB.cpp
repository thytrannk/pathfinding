//
// Created by Katie Tran on 2019-03-06.
//

#include "puzzlePDB.h"
//#include "math.h"
#include <iostream>

using namespace std;

//StatePuzzlePDB::StatePuzzlePDB(vector<int8_t> &stateArr, vector<int8_t> *patternVec) {
//    size = (int8_t) stateArr.size();
//    pattern = patternVec;
//    stateID = this->hash(stateArr);
//}

StatePuzzlePDB::StatePuzzlePDB(uint64_t rank, uint8_t dim, vector<int8_t> *patternVec) {
    size = dim;
    pattern = patternVec;
    stateID = rank;
}

StatePuzzlePDB::StatePuzzlePDB(const StatePuzzlePDB &state2) {
    size = state2.size;
    stateID = state2.stateID;
    pattern = state2.pattern;
}

uint64_t StatePuzzlePDB::getStateID() const {
    return stateID;
}

vector<int8_t>* StatePuzzlePDB::getPattern() const {
    return pattern;
}

uint8_t StatePuzzlePDB::getSize() const {
    return size;
}

void StatePuzzlePDB::dual(vector<int8_t> &stateArr, vector<int8_t> &dualArr) {
    bool flag;
    for (int p = 0; p < pattern->size(); p++) {
        flag = false;
        for (int8_t i = 0; i < size; i++) {
            if (stateArr[i] == (*pattern)[p]) {
                dualArr[p] = i;
                flag = true;
                break;
            }
        }
        if (!flag) {
            cerr << "Error with constructing dual." << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void StatePuzzlePDB::unrank(vector<int8_t> &arr) {
    cerr << "Unranking not implemented for this ranking method." << endl;
    exit(EXIT_FAILURE);
}

int StatePuzzlePDB::getLocation(int8_t val) {
    vector<int8_t> arr(size);
    this->unrank(arr);
    int loc;
    for (loc = 0; loc < size; loc++) {
        if (arr[loc] == val) {
            return loc;
        }
    }
    cerr << "Invalid location." << endl;
    exit(EXIT_FAILURE);
}

void StatePuzzlePDB::display() {
    vector<int8_t> arr(size);
    this->unrank(arr);
    for (int i = 0; i < 16; i++) {
        cout << (int) arr[i] << "   ";
    }
    cout << endl;
}

void StatePuzzlePDB::swap(int location1, int location2) {
    vector<int8_t> arr(size);
    this->unrank(arr);
    int8_t temp = arr[location1];
    arr[location1] = arr[location2];
    arr[location2] = temp;
    stateID = this->hash(arr);
}

bool operator==(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs) {
    return lhs.getStateID() == rhs.getStateID();
}

bool operator<(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs) {
    return lhs.getStateID() < rhs.getStateID();
}

bool operator>(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs) {
    return lhs.getStateID() > rhs.getStateID();
}

StatePuzzleLexi::StatePuzzleLexi(vector<int8_t> &stateArr, vector<int8_t> *patternVec) : StatePuzzlePDB(0, (int8_t) stateArr.size(), patternVec) {
//    size = (int8_t) stateArr.size();
//    pattern = patternVec;
    stateID = hash(stateArr);
}

StatePuzzleLexi::StatePuzzleLexi(uint64_t rank, uint8_t dim, vector<int8_t> *patternVec) : StatePuzzlePDB(rank, dim, patternVec) {
}

uint64_t StatePuzzleLexi::hash(vector<int8_t> &stateArr) {
    vector<int8_t> dualArr(pattern->size());
    dual(stateArr, dualArr);
    uint64_t ret = dualArr[0] * factorialDiv(size - 1, size - dualArr.size());
    for (int i = 1; i < dualArr.size(); i++) {
        for (int j = i; j < dualArr.size(); j++) {
            if (dualArr[j] > dualArr[i - 1]) {
                dualArr[j] -= 1;
            }
        }
        ret += dualArr[i] * factorialDiv(size - 1 - i, size - dualArr.size());
    }
    return ret;
}

void StatePuzzleLexi::unrank(vector<int8_t> &arr) {
    vector<int8_t> dual(pattern->size());
    uint64_t div = stateID;
    for (int i = size - pattern->size() + 1; i < size + 1; i++) {
        dual[size - i] = (int8_t) (div % i);
        div = div / i;
    }
//    for (int i = 0; i < dual.size(); i++) {
//        cout << (int) dual[i] << "   ";
//    }
//    cout << endl;
    for (int i = (int) dual.size() - 2; i >=0; i--) {
        for (int j = i + 1; j < dual.size(); j++) {
            if (dual[j] >= dual[i]) {
                dual[j] += 1;
            }
        }
    }
//    for (int i = 0; i < dual.size(); i++) {
//        cout << (int) dual[i] << "   ";
//    }
//    cout << endl;
    for (int i = 0; i < size; i++) {
        arr[i] = -1;
    }
    for (int i = 0; i < pattern->size(); i++) {
        arr[dual[i]] = (*pattern)[i];
    }
//    for (int i = 0; i < size; i++) {
//        cout << (int) arr[i] << "   ";
//    }
}

StatePuzzleMR::StatePuzzleMR(vector<int8_t> &stateArr, vector<int8_t> *patternVec) : StatePuzzlePDB(0, (int8_t) stateArr.size(), patternVec) {
//    size = (int8_t) stateArr.size();
//    pattern = patternVec;
    if (size != pattern->size()) {
        cerr << "MR ranking code only works when size of state is equal to size of pattern." << endl;
        exit(EXIT_FAILURE);
    }
    stateID = hash(stateArr);
}

uint64_t StatePuzzleMR::hash(vector<int8_t> &stateArr) {
    vector<int8_t> dualArr(pattern->size());
    dual(stateArr, dualArr);
    vector<int8_t> permutation = stateArr;
    return rank(size, permutation, dualArr);
}

uint64_t StatePuzzleMR::rank(uint8_t n, vector<int8_t> &stateArr, vector<int8_t> &dual) {
    if (n == 1) {
        return 0;
    }
    int8_t s = stateArr[n - 1];
    swapArr(stateArr, n - 1, dual[n - 1]);
    swapArr(dual, s, n - 1);
    return s + n * rank(n - 1, stateArr, dual);

}

void StatePuzzleMR::swapArr(vector<int8_t> &arr, uint8_t loc1, uint8_t loc2) {
    if (loc1 >= arr.size() || loc2 >= arr.size()) {
        cerr << "Accessing out of bound element." << endl;
        exit(EXIT_FAILURE);
    }
    int8_t temp = arr[loc1];
    arr[loc1] = arr[loc2];
    arr[loc2] = temp;
}