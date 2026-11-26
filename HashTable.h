#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "HashStep.h"

// A genuine hash table (no STL unordered_map) supporting two collision
// strategies, chosen at construction. Chaining stores each bucket's
// colliding keys in a small fixed-size array (a simplified stand-in for a
// linked chain, kept compact for this project's line budget); probing
// resolves collisions by scanning forward through the array itself, using
// a tombstone marker so deletions don't break later searches.
class HashTable {
public:
    explicit HashTable(bool chaining);

    bool insert(int value, HashStep *out, int &count, int capacity);
    bool search(int value, HashStep *out, int &count, int capacity);
    bool remove(int value, HashStep *out, int &count, int capacity);
    void display(HashStep *out, int &count, int capacity);

private:
    bool chaining_;

    int chain_[HASH_TABLE_SIZE][HASH_CHAIN_MAX];
    int chainCount_[HASH_TABLE_SIZE];

    int probeData_[HASH_TABLE_SIZE];
    int probeState_[HASH_TABLE_SIZE]; // 0 empty, 1 occupied, 2 deleted

    int hashFunc(int value) const { return ((value % HASH_TABLE_SIZE) + HASH_TABLE_SIZE) % HASH_TABLE_SIZE; }
    void recordState(HashStep *out, int &count, int capacity, int highlightBucket,
                      const int *probes, int probeCount, const QString &desc, const OperationStats &stats);
};

#endif // HASHTABLE_H
