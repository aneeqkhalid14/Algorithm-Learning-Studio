#ifndef LISTSTEP_H
#define LISTSTEP_H

#include <QString>
#include "../core/OperationStats.h"

const int LIST_STEP_MAX = 10; // max nodes the visualizer draws

// One frozen moment of a linked list operation. Each node gets a stable
// "slot" (its position in "values"). nextOf[i]/prevOf[i] say which slot
// that node's real next/prev pointer currently targets (-1 = null) -
// recorded straight from the real pointers in LinkedList.cpp, which is
// what lets Reverse be shown accurately even while the list is briefly
// not forward-traversable from head mid-operation.
struct ListStep {
    int values[LIST_STEP_MAX];
    int nextOf[LIST_STEP_MAX];
    int prevOf[LIST_STEP_MAX]; // only meaningful when isDoubly is true
    int size = 0;
    bool isDoubly = false;
    int headSlot = -1;        // which slot the head pointer currently targets
    int highlightIndex = -1;  // node currently being visited/changed
    int secondaryIndex = -1;  // a second node involved (e.g. link target)
    bool isNewNode = false;   // highlightIndex is a brand-new node this step
    QString description;
    OperationStats stats;
};

#endif // LISTSTEP_H
