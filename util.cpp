//
// Created by Katie Tran on 2019-03-09.
//

#include "util.h"
#include <cmath>
#include <cstdlib>

uint64_t factorialDiv(int n, int k) {
    // returns n!/k!
    uint64_t ret = 1;
    for (int i = k + 1; i < n + 1; i++) {
        ret *= i;
    }
    return ret;
}

uint64_t factorial(int n) {
    // returns n!
    uint64_t ret = 1;
    for (int i = 1; i < n + 1; i++) {
        ret *= i;
    }
    return ret;
}

int ManhattanDist(int start, int goal) {
    // Manhattan distance between 2 locations on a 4*4 grid 
    int xGoal = goal % 4;
    int xStart = start % 4;
    int yGoal = goal / 4;
    int yStart = start / 4;
    int xDist = abs(xGoal - xStart);
    int yDist = abs(yGoal - yStart);
    return xDist + yDist;
}