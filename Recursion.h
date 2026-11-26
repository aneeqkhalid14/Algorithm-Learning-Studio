#ifndef RECURSION_H
#define RECURSION_H

#include "RecursionStep.h"

// Genuinely recursive C++ functions (using the real call stack - no
// manual stack simulation), instrumented to record a step every time a
// call is pushed, a base case is hit, or a call returns and pops.
class Recursion {
public:
    static int factorial(int n, RecursionStep *outSteps, int capacity);
    static int fibonacci(int n, RecursionStep *outSteps, int capacity); // plain exponential recursion, intentionally not memoized - the point is to show repeated calls
};

#endif // RECURSION_H
