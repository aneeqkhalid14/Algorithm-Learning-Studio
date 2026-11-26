#include "HashTable.h"

HashTable::HashTable(bool chaining) : chaining_(chaining)
{
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        chainCount_[i] = 0;
        probeData_[i] = 0;
        probeState_[i] = 0;
        for (int j = 0; j < HASH_CHAIN_MAX; ++j) chain_[i][j] = 0;
    }
}

void HashTable::recordState(HashStep *out, int &count, int capacity, int highlightBucket,
                             const int *probes, int probeCount, const QString &desc, const OperationStats &stats)
{
    if (count >= capacity) return;
    HashStep &s = out[count];
    s.isChaining = chaining_;
    s.highlightBucket = highlightBucket;
    s.probeCount = probeCount;
    for (int i = 0; i < probeCount && i < HASH_TABLE_SIZE; ++i) s.probeSequence[i] = probes[i];

    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (chaining_) {
            s.buckets[i].count = chainCount_[i];
            s.buckets[i].state = 0;
            for (int j = 0; j < chainCount_[i]; ++j) s.buckets[i].values[j] = chain_[i][j];
        } else {
            s.buckets[i].state = probeState_[i];
            s.buckets[i].count = (probeState_[i] == 1) ? 1 : 0;
            if (probeState_[i] == 1) s.buckets[i].values[0] = probeData_[i];
        }
    }
    s.description = desc;
    s.stats = stats;
    ++count;
}

bool HashTable::insert(int value, HashStep *out, int &count, int capacity)
{
    OperationStats stats;
    int idx = hashFunc(value);
    recordState(out, count, capacity, idx, nullptr, 0, QString("Hash(%1) = %1 %% %2 = %3.").arg(value).arg(HASH_TABLE_SIZE).arg(idx), stats);

    if (chaining_) {
        for (int i = 0; i < chainCount_[idx]; ++i) {
            stats.comparisons++;
            if (chain_[idx][i] == value) {
                recordState(out, count, capacity, idx, nullptr, 0, QString("%1 already exists in bucket %2 - no duplicate inserted.").arg(value).arg(idx), stats);
                return false;
            }
        }
        if (chainCount_[idx] >= HASH_CHAIN_MAX) {
            recordState(out, count, capacity, idx, nullptr, 0, QString("Bucket %1's chain is full - cannot insert.").arg(idx), stats);
            return false;
        }
        bool collided = chainCount_[idx] > 0;
        chain_[idx][chainCount_[idx]] = value;
        ++chainCount_[idx];
        stats.insertions++;
        if (collided) stats.collisions++;
        recordState(out, count, capacity, idx, nullptr, 0,
            collided ? QString("Collision at bucket %1 - appended %2 to the chain.").arg(idx).arg(value)
                     : QString("Inserted %1 into bucket %2.").arg(value).arg(idx), stats);
        return true;
    }

    // Linear probing, with tombstone reuse.
    int probes[HASH_TABLE_SIZE]; int pc = 0;
    int i = idx;
    int firstTombstone = -1;
    for (int tries = 0; tries < HASH_TABLE_SIZE; ++tries) {
        stats.probes++;
        probes[pc++] = i;
        if (probeState_[i] == 1 && probeData_[i] == value) {
            recordState(out, count, capacity, i, probes, pc, QString("%1 already exists at slot %2 - no duplicate inserted.").arg(value).arg(i), stats);
            return false;
        }
        if (probeState_[i] == 0) {
            int target = (firstTombstone != -1) ? firstTombstone : i;
            probeData_[target] = value;
            probeState_[target] = 1;
            stats.insertions++;
            if (tries > 0) stats.collisions++;
            recordState(out, count, capacity, target, probes, pc, QString("Slot %1 is free - inserted %2 here.").arg(target).arg(value), stats);
            return true;
        }
        if (probeState_[i] == 2 && firstTombstone == -1) firstTombstone = i;
        if (probeState_[i] == 1) {
            stats.collisions++;
            recordState(out, count, capacity, i, probes, pc, QString("Slot %1 is occupied - probing next slot.").arg(i), stats);
        }
        i = (i + 1) % HASH_TABLE_SIZE;
    }
    if (firstTombstone != -1) {
        probeData_[firstTombstone] = value;
        probeState_[firstTombstone] = 1;
        stats.insertions++;
        recordState(out, count, capacity, firstTombstone, probes, pc, QString("Reused deleted slot %1 for %2.").arg(firstTombstone).arg(value), stats);
        return true;
    }
    recordState(out, count, capacity, -1, probes, pc, "Table is full - cannot insert.", stats);
    return false;
}

bool HashTable::search(int value, HashStep *out, int &count, int capacity)
{
    OperationStats stats;
    int idx = hashFunc(value);
    recordState(out, count, capacity, idx, nullptr, 0, QString("Hash(%1) = %2.").arg(value).arg(idx), stats);

    if (chaining_) {
        for (int i = 0; i < chainCount_[idx]; ++i) {
            stats.comparisons++;
            stats.probes++;
            if (chain_[idx][i] == value) {
                recordState(out, count, capacity, idx, nullptr, 0, QString("Found %1 in bucket %2's chain.").arg(value).arg(idx), stats);
                return true;
            }
        }
        recordState(out, count, capacity, idx, nullptr, 0, QString("%1 was not found in bucket %2's chain.").arg(value).arg(idx), stats);
        return false;
    }

    int probes[HASH_TABLE_SIZE]; int pc = 0;
    int i = idx;
    for (int tries = 0; tries < HASH_TABLE_SIZE; ++tries) {
        stats.probes++;
        probes[pc++] = i;
        if (probeState_[i] == 0) {
            recordState(out, count, capacity, i, probes, pc, QString("Slot %1 is empty - %2 is not in the table.").arg(i).arg(value), stats);
            return false;
        }
        if (probeState_[i] == 1) {
            stats.comparisons++;
            if (probeData_[i] == value) {
                recordState(out, count, capacity, i, probes, pc, QString("Found %1 at slot %2.").arg(value).arg(i), stats);
                return true;
            }
        }
        recordState(out, count, capacity, i, probes, pc, QString("Slot %1 does not match - continue probing.").arg(i), stats);
        i = (i + 1) % HASH_TABLE_SIZE;
    }
    recordState(out, count, capacity, -1, probes, pc, QString("%1 was not found after a full cycle.").arg(value), stats);
    return false;
}

bool HashTable::remove(int value, HashStep *out, int &count, int capacity)
{
    OperationStats stats;
    int idx = hashFunc(value);
    recordState(out, count, capacity, idx, nullptr, 0, QString("Deleting %1 - Hash(%1) = %2.").arg(value).arg(idx), stats);

    if (chaining_) {
        for (int i = 0; i < chainCount_[idx]; ++i) {
            stats.comparisons++;
            if (chain_[idx][i] == value) {
                for (int j = i; j < chainCount_[idx] - 1; ++j) chain_[idx][j] = chain_[idx][j + 1];
                --chainCount_[idx];
                stats.deletions++;
                recordState(out, count, capacity, idx, nullptr, 0, QString("Removed %1 from bucket %2's chain.").arg(value).arg(idx), stats);
                return true;
            }
        }
        recordState(out, count, capacity, idx, nullptr, 0, QString("%1 was not found in bucket %2.").arg(value).arg(idx), stats);
        return false;
    }

    int probes[HASH_TABLE_SIZE]; int pc = 0;
    int i = idx;
    for (int tries = 0; tries < HASH_TABLE_SIZE; ++tries) {
        stats.probes++;
        probes[pc++] = i;
        if (probeState_[i] == 0) {
            recordState(out, count, capacity, i, probes, pc, QString("Slot %1 is empty - %2 is not in the table.").arg(i).arg(value), stats);
            return false;
        }
        if (probeState_[i] == 1) {
            stats.comparisons++;
            if (probeData_[i] == value) {
                probeState_[i] = 2; // tombstone, so later searches keep scanning past this slot
                stats.deletions++;
                recordState(out, count, capacity, i, probes, pc, QString("Removed %1 from slot %2 (marked as deleted).").arg(value).arg(i), stats);
                return true;
            }
        }
        recordState(out, count, capacity, i, probes, pc, QString("Slot %1 does not match - continue probing.").arg(i), stats);
        i = (i + 1) % HASH_TABLE_SIZE;
    }
    recordState(out, count, capacity, -1, probes, pc, QString("%1 was not found.").arg(value), stats);
    return false;
}

void HashTable::display(HashStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, nullptr, 0, "Current hash table contents.", stats);
}
