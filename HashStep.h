#ifndef HASHSTEP_H
#define HASHSTEP_H

#include <QString>
#include "../core/OperationStats.h"

const int HASH_TABLE_SIZE = 7; // small prime - keeps collisions frequent enough to see
const int HASH_CHAIN_MAX = 8;  // max items drawn per bucket chain (generous - a table of size 7
                                // with values that all collide could otherwise hit this during a demo)

struct HashBucket {
    int values[HASH_CHAIN_MAX]; // chaining: the chain's values; probing: only values[0] used
    int count = 0;              // chaining: chain length; probing: 1 if occupied, else 0
    int state = 0;              // probing only: 0 = empty, 1 = occupied, 2 = deleted (tombstone)
};

struct HashStep {
    HashBucket buckets[HASH_TABLE_SIZE];
    bool isChaining = true;
    int highlightBucket = -1;
    int probeSequence[HASH_TABLE_SIZE]; // bucket indices visited this step, in order (linear probing)
    int probeCount = 0;
    QString description;
    OperationStats stats;
};

#endif // HASHSTEP_H
