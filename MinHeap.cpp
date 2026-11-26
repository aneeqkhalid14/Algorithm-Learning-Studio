#include "MinHeap.h"

MinHeap::MinHeap() : size_(0)
{
    for (int i = 0; i < HEAP_CAPACITY; ++i) data_[i] = 0;
}

void MinHeap::recordState(TreeStep *out, int &count, int capacity, int hi, int si,
                           const QString &desc, const OperationStats &stats)
{
    if (count >= capacity) return;
    TreeStep &s = out[count];
    s.nodeCount = size_;
    s.rootIndex = (size_ > 0) ? 0 : -1;
    for (int i = 0; i < size_; ++i) {
        s.nodes[i].value = data_[i];
        s.nodes[i].exists = true;
        s.nodes[i].left = (2 * i + 1 < size_) ? 2 * i + 1 : -1;
        s.nodes[i].right = (2 * i + 2 < size_) ? 2 * i + 2 : -1;
    }
    s.highlightIndex = hi;
    s.secondaryIndex = si;
    s.isNewNode = false;
    s.description = desc;
    s.stats = stats;
    ++count;
}

void MinHeap::siftUp(int idx, TreeStep *out, int &count, int capacity, OperationStats &stats)
{
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        stats.comparisons++;
        recordState(out, count, capacity, idx, parent, QString("Comparing %1 with parent %2.").arg(data_[idx]).arg(data_[parent]), stats);
        if (data_[idx] < data_[parent]) {
            int tmp = data_[idx]; data_[idx] = data_[parent]; data_[parent] = tmp;
            stats.swaps++;
            recordState(out, count, capacity, parent, idx, QString("Swapped - %1 is smaller than its parent, so it moves up.").arg(data_[parent]), stats);
            idx = parent;
        } else break;
    }
}

void MinHeap::siftDown(int idx, TreeStep *out, int &count, int capacity, OperationStats &stats)
{
    while (true) {
        int left = 2 * idx + 1, right = 2 * idx + 2, smallest = idx;
        if (left < size_) { stats.comparisons++; if (data_[left] < data_[smallest]) smallest = left; }
        if (right < size_) { stats.comparisons++; if (data_[right] < data_[smallest]) smallest = right; }
        if (smallest == idx) break;
        recordState(out, count, capacity, idx, smallest, QString("Child %1 is smaller - swapping down.").arg(data_[smallest]), stats);
        int tmp = data_[idx]; data_[idx] = data_[smallest]; data_[smallest] = tmp;
        stats.swaps++;
        idx = smallest;
    }
}

bool MinHeap::insert(int value, TreeStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (size_ >= HEAP_CAPACITY) {
        recordState(out, count, capacity, -1, -1, "Heap is full - cannot insert.", stats);
        return false;
    }
    data_[size_] = value;
    int idx = size_;
    ++size_;
    stats.insertions++;
    recordState(out, count, capacity, idx, -1, QString("Inserted %1 at the next free slot (index %2).").arg(value).arg(idx), stats);
    siftUp(idx, out, count, capacity, stats);
    recordState(out, count, capacity, -1, -1, "Insert complete - heap property restored.", stats);
    return true;
}

bool MinHeap::extractMin(TreeStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (size_ == 0) {
        recordState(out, count, capacity, -1, -1, "Heap is empty - nothing to extract.", stats);
        return false;
    }
    int minVal = data_[0];
    recordState(out, count, capacity, 0, -1, QString("Extracting minimum: %1 (the root).").arg(minVal), stats);
    data_[0] = data_[size_ - 1];
    --size_;
    stats.deletions++;
    if (size_ > 0) {
        recordState(out, count, capacity, 0, -1, QString("Moved the last element (%1) to the root.").arg(data_[0]), stats);
        siftDown(0, out, count, capacity, stats);
        recordState(out, count, capacity, -1, -1, "Extraction complete - heap property restored.", stats);
    } else {
        recordState(out, count, capacity, -1, -1, "Heap is now empty.", stats);
    }
    return true;
}

void MinHeap::display(TreeStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, "Current heap contents (array form, shown as a tree).", stats);
}
