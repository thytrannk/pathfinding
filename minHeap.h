//
// Created by Katie Tran on 2019-02-10.
//

#ifndef ASSIGNMENT2_MINHEAP_H
#define ASSIGNMENT2_MINHEAP_H

#include <vector>
#include <iostream>
#include <cfloat>
#include <climits>
#include "searchAlgorithm.h"

using namespace std;

template <typename type, typename node>
class MinHeap {
public:
    // constructor
    MinHeap();
    // heapify a subtree with the root at index i. This will modify the queue location of the node in stateVec
    void minHeapify(uint64_t i, vector<node> *stateVec);
    // heapify a subtree with the root at index i. This will modify the queue location of the node in stateVec
    // hat = true when operating on open hat; false when operating on open
    void minHeapify(uint64_t i, vector<node> *stateVec, bool hat);
    // heapify a subtree with the root at index i. This will modify the queue location of the node in stateVec
    // waiting = true when operating on waiting queue; false when operating on ready queue
    // forward = true when operating on forward direction queue; false when operating on backward direction queue
    void minHeapify(uint64_t i, vector<node> *stateVec, bool waiting, bool forward);
    // return index of parent of element at index i
    uint64_t parent(uint64_t i);
    // return index of left child of element at index i
    uint64_t left(uint64_t i);
    // return index of right child of element at index i
    uint64_t right(uint64_t i);
    // return the minimum element and remove it from the heap. This will modify the queue location of the node in stateVec
    pair<type, uint64_t> extractMin(vector<node> *stateVec);
    // return the minimum element and remove it from the heap. This will modify the queue location of the node in stateVec
    // hat = true when operating on open hat; false when operating on open
    pair<type, uint64_t> extractMin(vector<node> *stateVec, bool hat);
    // return the minimum element and remove it from the heap. This will modify the queue location of the node in stateVec
    // waiting = true when operating on waiting queue; false when operating on ready queue
    // forward = true when operating on forward direction queue; false when operating on backward direction queue
    pair<type, uint64_t> extractMin(vector<node> *stateVec, bool waiting, bool forward);
    // decreases value of element with specified key to new_val. This will modify the queue location of the node in stateVec
    void decreaseKey(uint64_t i, type new_val, vector<node> *stateVec);
    // decreases value of element with specified key to new_val. This will modify the queue location of the node in stateVec
    // hat = true when operating on open hat; false when operating on open
    void decreaseKey(uint64_t i, type new_val, vector<node> *stateVec, bool hat);
    // decreases value of element with specified key to new_val. This will modify the queue location of the node in stateVec
    // waiting = true when operating on waiting queue; false when operating on ready queue
    // forward = true when operating on forward direction queue; false when operating on backward direction queue
    void decreaseKey(uint64_t i, type new_val, vector<node> *stateVec, bool waiting, bool forward);
    // return the minimum element
    pair<type, uint64_t> getMin();
//    // delete element at index i
//    void deleteElement(uint64_t i);
    // insert a new element, returns index of that element in heap. This will modify the queue location of the node in stateVec
    uint64_t insertKey(pair<type, uint64_t> elem, vector<node> *stateVec);
    // insert a new element, returns index of that element in heap. This will modify the queue location of the node in stateVec
    // hat = true when operating on open hat; false when operating on open
    uint64_t insertKey(pair<type, uint64_t> elem, vector<node> *stateVec, bool hat);
    // insert a new element, returns index of that element in heap. This will modify the queue location of the node in stateVec
    // waiting = true when operating on waiting queue; false when operating on ready queue
    // forward = true when operating on forward direction queue; false when operating on backward direction queue
    uint64_t insertKey(pair<type, uint64_t> elem, vector<node> *stateVec, bool waiting, bool forward);
    uint64_t size();
    void clear();
//    typename vector<pair<type, uint64_t>>::iterator getBegin();
//    typename vector<pair<type, uint64_t>>::iterator getEnd();
private:
    vector<pair<type, uint64_t>> heap; // pointer to array of elements in heap
    uint64_t heapSize; // number of elements in heap
    void swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec);
    void swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec, bool hat);
    void swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec, bool waiting, bool forward);
};

template <typename type, typename node>
MinHeap<type, node>::MinHeap() {
    heapSize = 0;
}

template <typename type, typename node>
void MinHeap<type, node>::minHeapify(uint64_t i, vector<node> *stateVec)
{
    uint64_t l = left(i);
    uint64_t r = right(i);
    uint64_t smallest = i;
    if (l < heapSize && heap[l].first < heap[i].first)
        smallest = l;
    if (r < heapSize && heap[r].first < heap[smallest].first)
        smallest = r;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest], stateVec);
        minHeapify(smallest, stateVec);
    }
}

template <typename type, typename node>
void MinHeap<type, node>::minHeapify(uint64_t i, vector<node> *stateVec, bool hat)
{
    uint64_t l = left(i);
    uint64_t r = right(i);
    uint64_t smallest = i;
    if (l < heapSize && heap[l].first < heap[i].first)
        smallest = l;
    if (r < heapSize && heap[r].first < heap[smallest].first)
        smallest = r;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest], stateVec, hat);
        minHeapify(smallest, stateVec, hat);
    }
}

template <typename type, typename node>
void MinHeap<type, node>::minHeapify(uint64_t i, vector<node> *stateVec, bool waiting, bool forward)
{
    uint64_t l = left(i);
    uint64_t r = right(i);
    uint64_t smallest = i;
    if (l < heapSize && heap[l].first < heap[i].first)
        smallest = l;
    if (r < heapSize && heap[r].first < heap[smallest].first)
        smallest = r;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest], stateVec, waiting, forward);
        minHeapify(smallest, stateVec, waiting, forward);
    }
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::parent(uint64_t i) {
    return (i - 1) / 2;
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::left(uint64_t i) {
    return 2 * i + 1;
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::right(uint64_t i) {
    return 2 * i + 2;
}

template <typename type, typename node>
pair<type, uint64_t> MinHeap<type, node>::extractMin(vector<node> *stateVec) {
    if (heapSize <= 0) {
        return pair<type, uint64_t>{numeric_limits<type>::max(), UINT64_MAX};
    }
    if (heapSize == 1) {
        pair<type, uint64_t> ret = heap[0];
        heap.clear();
        heapSize--;
        (*stateVec)[ret.second].locationOnOpen = UINT64_MAX;
        return ret;
    }
    pair<type, uint64_t> root = heap[0];
    heap[0] = heap[heapSize - 1];
    (*stateVec)[heap[heapSize - 1].second].locationOnOpen = 0;
    (*stateVec)[root.second].locationOnOpen = UINT64_MAX;
    heap.pop_back();
    heapSize--;
    minHeapify(0, stateVec);

    return root;
}

template <typename type, typename node>
pair<type, uint64_t> MinHeap<type, node>::extractMin(vector<node> *stateVec, bool hat) {
    if (heapSize <= 0) {
        return pair<type, uint64_t>{numeric_limits<type>::max(), UINT64_MAX};
    }
    if (heapSize == 1) {
        pair<type, uint64_t> ret = heap[0];
        heap.clear();
        heapSize--;
        if (hat) {
            (*stateVec)[ret.second].locationOnOpenHat = UINT64_MAX;
        } else {
            (*stateVec)[ret.second].locationOnOpen = UINT64_MAX;
        }
        return ret;
    }
    pair<type, uint64_t> root = heap[0];
    heap[0] = heap[heapSize - 1];
    if (hat) {
        (*stateVec)[heap[heapSize - 1].second].locationOnOpenHat = 0;
        (*stateVec)[root.second].locationOnOpenHat = UINT64_MAX;
    } else {
        (*stateVec)[heap[heapSize - 1].second].locationOnOpen = 0;
        (*stateVec)[root.second].locationOnOpen = UINT64_MAX;
    }
    heap.pop_back();
    heapSize--;
    minHeapify(0, stateVec, hat);

    return root;
}

template <typename type, typename node>
pair<type, uint64_t> MinHeap<type, node>::extractMin(vector<node> *stateVec, bool waiting, bool forward) {
    if (heapSize <= 0) {
        return pair<type, uint64_t>{numeric_limits<type>::max(), UINT64_MAX};
    }
    if (heapSize == 1) {
        pair<type, uint64_t> ret = heap[0];
        heap.clear();
        heapSize--;
        if (waiting && forward) {
            (*stateVec)[ret.second].locationOnWaitingF = UINT64_MAX;
        } else if (waiting && !forward) {
            (*stateVec)[ret.second].locationOnWaitingB = UINT64_MAX;
        } else if (!waiting && forward) {
            (*stateVec)[ret.second].locationOnReadyF = UINT64_MAX;
        } else {
            (*stateVec)[ret.second].locationOnReadyB = UINT64_MAX;
        }
        return ret;
    }
    pair<type, uint64_t> root = heap[0];
    heap[0] = heap[heapSize - 1];
    if (waiting && forward) {
        (*stateVec)[heap[heapSize - 1].second].locationOnWaitingF = 0;
        (*stateVec)[root.second].locationOnWaitingF = UINT64_MAX;
    } else if (waiting && !forward) {
        (*stateVec)[heap[heapSize - 1].second].locationOnWaitingB = 0;
        (*stateVec)[root.second].locationOnWaitingB = UINT64_MAX;
    } else if (!waiting && forward) {
        (*stateVec)[heap[heapSize - 1].second].locationOnReadyF = 0;
        (*stateVec)[root.second].locationOnReadyF = UINT64_MAX;
    } else {
        (*stateVec)[heap[heapSize - 1].second].locationOnReadyB = 0;
        (*stateVec)[root.second].locationOnReadyB = UINT64_MAX;
    }
    heap.pop_back();
    heapSize--;
    minHeapify(0, stateVec, waiting, forward);

    return root;
}

template <typename type, typename node>
void MinHeap<type, node>::decreaseKey(uint64_t i, type new_val, vector<node> *stateVec) {
    heap[i].first = new_val;
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec);
        i = parent(i);
    }
}

template <typename type, typename node>
void MinHeap<type, node>::decreaseKey(uint64_t i, type new_val, vector<node> *stateVec, bool hat) {
    heap[i].first = new_val;
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec, hat);
        i = parent(i);
    }
}

template <typename type, typename node>
void MinHeap<type, node>::decreaseKey(uint64_t i, type new_val, vector<node> *stateVec, bool waiting, bool forward) {
    heap[i].first = new_val;
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec, waiting, forward);
        i = parent(i);
    }
}

template <typename type, typename node>
pair<type, uint64_t> MinHeap<type, node>::getMin() {
    if (heapSize <= 0) {
        return pair<type, uint64_t>{numeric_limits<type>::max(), UINT64_MAX};
    }
    return heap[0];
}

//template <typename type, typename node>
//void MinHeap<type, node>::deleteElement(uint64_t i) {
//    decreaseKey(i, numeric_limits<type, node>::min());
//    extractMin();
//}

template <typename type, typename node>
uint64_t MinHeap<type, node>::insertKey(pair<type, uint64_t> elem, vector<node> *stateVec) {
    // insert new key at the end
    heapSize++;
    uint64_t i = heapSize - 1;
    heap.emplace_back(elem);
    (*stateVec)[elem.second].locationOnOpen = heapSize - 1;

    // fix the min heap property if needed
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec);
        i = parent(i);
    }
    return i;
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::insertKey(pair<type, uint64_t> elem, vector<node> *stateVec, bool hat) {
    // insert new key at the end
    heapSize++;
    uint64_t i = heapSize - 1;
    heap.emplace_back(elem);
    if (hat) {
        (*stateVec)[elem.second].locationOnOpenHat = heapSize - 1;
    } else {
        (*stateVec)[elem.second].locationOnOpen = heapSize - 1;
    }
    // fix the min heap property if needed
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec, hat);
        i = parent(i);
    }
    return i;
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::insertKey(pair<type, uint64_t> elem, vector<node> *stateVec, bool waiting, bool forward) {
    // insert new key at the end
    heapSize++;
    uint64_t i = heapSize - 1;
    heap.emplace_back(elem);
    if (waiting && forward) {
        (*stateVec)[elem.second].locationOnWaitingF = heapSize - 1;
    } else if (waiting && !forward) {
        (*stateVec)[elem.second].locationOnWaitingB = heapSize - 1;
    } else if (!waiting && forward) {
        (*stateVec)[elem.second].locationOnReadyF = heapSize - 1;
    } else {
        (*stateVec)[elem.second].locationOnReadyB = heapSize - 1;
    }
    // fix the min heap property if needed
    while (i != 0 && heap[parent(i)].first > heap[i].first) {
        swap(&heap[i], &heap[parent(i)], stateVec, waiting, forward);
        i = parent(i);
    }
    return i;
}

template <typename type, typename node>
uint64_t MinHeap<type, node>::size() {
    return heapSize;
}

template <typename type, typename node>
void MinHeap<type, node>::swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec) {
    pair<type, uint64_t> temp = *x;
    *x = *y;
    *y = temp;
    uint64_t tempLoc = (*stateVec)[x->second].locationOnOpen;
    (*stateVec)[x->second].locationOnOpen = (*stateVec)[y->second].locationOnOpen;
    (*stateVec)[y->second].locationOnOpen = tempLoc;
}

template <typename type, typename node>
void MinHeap<type, node>::swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec, bool hat) {
    pair<type, uint64_t> temp = *x;
    *x = *y;
    *y = temp;
    if (hat) {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnOpenHat;
        (*stateVec)[x->second].locationOnOpenHat = (*stateVec)[y->second].locationOnOpenHat;
        (*stateVec)[y->second].locationOnOpenHat = tempLoc;
    } else {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnOpen;
        (*stateVec)[x->second].locationOnOpen = (*stateVec)[y->second].locationOnOpen;
        (*stateVec)[y->second].locationOnOpen = tempLoc;
    }
}

template <typename type, typename node>
void MinHeap<type, node>::swap(pair<type, uint64_t> *x, pair<type, uint64_t> *y, vector<node> *stateVec, bool waiting, bool forward) {
    pair<type, uint64_t> temp = *x;
    *x = *y;
    *y = temp;
    if (waiting && forward) {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnWaitingF;
        (*stateVec)[x->second].locationOnWaitingF = (*stateVec)[y->second].locationOnWaitingF;
        (*stateVec)[y->second].locationOnWaitingF = tempLoc;
    } else if (waiting && !forward) {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnWaitingB;
        (*stateVec)[x->second].locationOnWaitingB = (*stateVec)[y->second].locationOnWaitingB;
        (*stateVec)[y->second].locationOnWaitingB = tempLoc;
    } else if (!waiting && forward) {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnReadyF;
        (*stateVec)[x->second].locationOnReadyF = (*stateVec)[y->second].locationOnReadyF;
        (*stateVec)[y->second].locationOnReadyF = tempLoc;
    } else {
        uint64_t tempLoc = (*stateVec)[x->second].locationOnReadyB;
        (*stateVec)[x->second].locationOnReadyB = (*stateVec)[y->second].locationOnReadyB;
        (*stateVec)[y->second].locationOnReadyB = tempLoc;
    }
}

template<typename type, typename node>
void MinHeap<type, node>::clear() {
    heap.clear();
    heapSize = 0;
}

//template<typename type, typename node>
//typename vector<pair<type, uint64_t>>::iterator MinHeap<type, node>::getBegin() {
//    return heap.begin();
//}
//
//template<typename type, typename node>
//typename vector<pair<type, uint64_t>>::iterator MinHeap<type, node>::getEnd() {
//    return heap.end();
//}


#endif //ASSIGNMENT2_MINHEAP_H
