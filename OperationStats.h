#ifndef OPERATIONSTATS_H
#define OPERATIONSTATS_H

#include <QString>

// Plain counter class shared by every DSA module. Each algorithm/operation
// increments the counters that are relevant to it (not every field is used
// by every algorithm) so the GUI can show "actual execution statistics"
// distinct from the theoretical complexity text.
class OperationStats {
public:
    int comparisons   = 0;
    int swaps         = 0;
    int arrayAccesses = 0;
    int insertions    = 0;
    int deletions     = 0;
    int probes        = 0;   // linear probing / chaining traversal steps
    int collisions    = 0;
    int recursiveCalls = 0;

    void reset();
    QString summary() const; // human-readable one-line summary for the GUI
};

#endif // OPERATIONSTATS_H
