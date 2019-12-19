//
// Created by Katie Tran on 2019-03-09.
//

#ifndef ASSIGNMENT3_UTIL_H
#define ASSIGNMENT3_UTIL_H

#include <stdint.h>
#include <vector>

using namespace std;

uint64_t factorialDiv(int n, int k);
uint64_t factorial(int n);
int ManhattanDist(int start, int goal);

template<typename type>
double vectAvg(vector<type> vect);


template<typename type>
double vectAvg(vector<type> vect) {
    double sum = 0;
    for (auto& n : vect) {
        sum += n;
    }
    return sum/vect.size();
}

#endif //ASSIGNMENT3_UTIL_H