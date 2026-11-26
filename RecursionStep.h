#ifndef RECURSIONSTEP_H
#define RECURSIONSTEP_H

#include <QString>
#include "../core/OperationStats.h"

const int RECURSION_MAX_FRAMES = 20; // max call-stack depth shown

// One frozen moment of a recursive computation: the current call stack
// (bottom to top) plus a narration of what just happened - a call being
// pushed, a base case being hit, or a frame returning a value and popping.
struct RecursionStep {
    QString frames[RECURSION_MAX_FRAMES];
    int frameCount = 0;
    QString description;
    QString returnValueText; // non-empty only on a "this frame just returned" step
    OperationStats stats;
};

#endif // RECURSIONSTEP_H
