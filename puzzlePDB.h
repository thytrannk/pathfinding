//
// Created by Katie Tran on 2019-03-06.
//

#ifndef ASSIGNMENT3_PUZZLEPDB_H
#define ASSIGNMENT3_PUZZLEPDB_H

#include <vector>
#include <map>
#include <stdint.h>
#include "util.h"

using namespace std;

class StatePuzzlePDB {
public:
    // create a state from an array of values at locations and pointer to the pattern vector
//    StatePuzzlePDB(vector<int8_t> &stateArr, vector<int8_t> *patternVec);
    // create a state from the rank, dimension of the puzzle, and pointer to the pattern vector
    StatePuzzlePDB(uint64_t rank, uint8_t dim, vector<int8_t> *patternVec);
    // destructor
    virtual ~StatePuzzlePDB() = default;
    // copy constructor
    StatePuzzlePDB(const StatePuzzlePDB &state2);
    // returns the pointer to state array
    uint64_t getStateID() const;
    // returns the pointer to pattern
    vector<int8_t> *getPattern() const;
    // returns the size of puzzle
    uint8_t getSize() const;
    // returns the hash value of a state from its array of values at locations
    // default: returns 0
    virtual uint64_t hash(vector<int8_t> &stateArr) {return 0;}
    // save the dual of stateArray into dualArr
    void dual(vector<int8_t> &stateArr, vector<int8_t> &dualArr);
    // save the unranked state array into arr
    virtual void unrank(vector<int8_t> &arr);
    // get value at a location
//    int8_t getValue(int location);
    // swap values at two locations
    void swap(int location1, int location2);
    // returns the location of a value in a state
    int getLocation(int8_t val);
    // prints out a state
    void display();
protected:
    // state array
    uint64_t stateID;
    // pointer to vector of pattern
    vector<int8_t> *pattern;
    // number of elements
    uint8_t size;
};

bool operator==(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs);
bool operator<(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs);
bool operator>(const StatePuzzlePDB &lhs, const StatePuzzlePDB &rhs);

class StatePuzzleLexi : public StatePuzzlePDB {
public:
    // create a state from an array of values at locations and pointer to the pattern vector
    StatePuzzleLexi(vector<int8_t> &stateArr, vector<int8_t> *patternVec);
    // create a state from the rank, dimension of the puzzle, and pointer to the pattern vector
    StatePuzzleLexi(uint64_t rank, uint8_t dim, vector<int8_t> *patternVec);
    // destructor
    ~StatePuzzleLexi() override = default;
    // returns the hash value of a state from its array of values at locations
    // hash function: lexicographical ranking (using mixed radix)
    uint64_t hash(vector<int8_t> &stateArr) override;
    // save the unranked state array into arr
    void unrank(vector<int8_t> &arr) override;
};

class StatePuzzleMR : public StatePuzzlePDB {
public:
    // create a state from an array of values at locations and pointer to the pattern vector
    // Only works when size of stateArr is equal to size of *patternVec (full permutation only)
    StatePuzzleMR(vector<int8_t> &stateArr, vector<int8_t> *patternVec);
    // destructor
    ~StatePuzzleMR() override = default;
    // returns the hash value of a state from its array of values at locations
    // hash function: MR
    uint64_t hash(vector<int8_t> &stateArr) override;
private:
    uint64_t rank(uint8_t n, vector<int8_t> &stateArr, vector<int8_t> &dual);
    void swapArr(vector<int8_t> &arr, uint8_t loc1, uint8_t loc2);
};

#endif //ASSIGNMENT3_PUZZLEPDB_H
