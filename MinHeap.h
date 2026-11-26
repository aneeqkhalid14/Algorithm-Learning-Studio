#ifndef MINHEAP_H
#define MINHEAP_H

#include "TreeStep.h"

const int HEAP_CAPACITY = TREE_STEP_MAX;

// A genuine array-based Min Heap (no STL priority_queue). Children of
// index i live at 2i+1 and 2i+2 - the classic array representation of a
// complete binary tree, which is also exactly what TreeStep expects, so
// recordState() here is a direct, honest mapping (no reshuffling needed).
class MinHeap {
public:
    MinHeap();
    bool insert(int value, TreeStep *out, int &count, int capacity);
    bool extractMin(TreeStep *out, int &count, int capacity);
    void display(TreeStep *out, int &count, int capacity);

private:
    int data_[HEAP_CAPACITY];
    int size_;

    void recordState(TreeStep *out, int &count, int capacity, int hi, int si,
                      const QString &desc, const OperationStats &stats);
    void siftUp(int idx, TreeStep *out, int &count, int capacity, OperationStats &stats);
    void siftDown(int idx, TreeStep *out, int &count, int capacity, OperationStats &stats);
};

#endif // MINHEAP_H
