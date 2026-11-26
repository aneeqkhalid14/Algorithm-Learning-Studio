#include "LinkedList.h"

// ======================= SinglyLinkedList =======================

SinglyLinkedList::SinglyLinkedList() : head_(nullptr) {}

SinglyLinkedList::~SinglyLinkedList()
{
    while (head_) { SLLNode *n = head_->next; delete head_; head_ = n; }
}

void SinglyLinkedList::recordState(ListStep *out, int &count, int capacity, int hi, int si,
                                    bool isNew, const QString &desc, const OperationStats &stats)
{
    if (count >= capacity) return;
    ListStep &s = out[count];
    s.size = 0;
    SLLNode *c = head_;
    while (c && s.size < LIST_STEP_MAX) {
        s.values[s.size] = c->value;
        s.nextOf[s.size] = (c->next && s.size + 1 < LIST_STEP_MAX) ? s.size + 1 : -1;
        s.prevOf[s.size] = -1;
        ++s.size;
        c = c->next;
    }
    s.isDoubly = false;
    s.headSlot = (s.size > 0) ? 0 : -1;
    s.highlightIndex = hi;
    s.secondaryIndex = si;
    s.isNewNode = isNew;
    s.description = desc;
    s.stats = stats;
    ++count;
}

void SinglyLinkedList::insertAtBeginning(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, QString("Inserting %1 at the beginning.").arg(value), stats);
    SLLNode *node = new SLLNode{value, head_};
    head_ = node;
    stats.insertions++;
    recordState(out, count, capacity, 0, -1, true,
        QString("New node (%1) is now the head; its next pointer points to the previous head.").arg(value), stats);
}

void SinglyLinkedList::insertAtEnd(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, QString("Inserting %1 at the end.").arg(value), stats);
    SLLNode *node = new SLLNode{value, nullptr};
    if (!head_) {
        head_ = node;
        stats.insertions++;
        recordState(out, count, capacity, 0, -1, true, QString("List was empty - new node (%1) becomes the head.").arg(value), stats);
        return;
    }
    SLLNode *cur = head_;
    int idx = 0;
    while (cur->next) {
        stats.arrayAccesses++;
        recordState(out, count, capacity, idx, -1, false,
            QString("Walking the list to find the last node (currently at position %1).").arg(idx), stats);
        cur = cur->next;
        ++idx;
    }
    cur->next = node;
    stats.insertions++;
    recordState(out, count, capacity, idx + 1, idx, true, QString("Linked new node (%1) after the last node.").arg(value), stats);
}

bool SinglyLinkedList::deleteFromBeginning(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (!head_) {
        recordState(out, count, capacity, -1, -1, false, "List is empty - nothing to delete.", stats);
        return false;
    }
    recordState(out, count, capacity, 0, -1, false, QString("Deleting head node (%1).").arg(head_->value), stats);
    SLLNode *old = head_;
    head_ = head_->next;
    delete old;
    stats.deletions++;
    recordState(out, count, capacity, head_ ? 0 : -1, -1, false, "Head pointer moved to the next node.", stats);
    return true;
}

bool SinglyLinkedList::deleteFromEnd(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (!head_) {
        recordState(out, count, capacity, -1, -1, false, "List is empty - nothing to delete.", stats);
        return false;
    }
    if (!head_->next) {
        recordState(out, count, capacity, 0, -1, false, QString("Deleting only node (%1).").arg(head_->value), stats);
        delete head_;
        head_ = nullptr;
        stats.deletions++;
        recordState(out, count, capacity, -1, -1, false, "List is now empty.", stats);
        return true;
    }
    SLLNode *cur = head_;
    int idx = 0;
    while (cur->next->next) {
        stats.arrayAccesses++;
        recordState(out, count, capacity, idx, -1, false,
            QString("Walking to find the second-last node (currently at position %1).").arg(idx), stats);
        cur = cur->next;
        ++idx;
    }
    recordState(out, count, capacity, idx + 1, idx, false, QString("Deleting last node (%1).").arg(cur->next->value), stats);
    delete cur->next;
    cur->next = nullptr;
    stats.deletions++;
    recordState(out, count, capacity, idx, -1, false, "Second-last node is now the last node; its next pointer is null.", stats);
    return true;
}

void SinglyLinkedList::search(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, QString("Searching for %1 from the head.").arg(value), stats);
    SLLNode *cur = head_;
    int idx = 0;
    while (cur) {
        stats.comparisons++;
        if (cur->value == value) {
            recordState(out, count, capacity, idx, -1, false, QString("Found %1 at position %2!").arg(value).arg(idx), stats);
            return;
        }
        recordState(out, count, capacity, idx, -1, false, QString("Position %1 (%2) does not match.").arg(idx).arg(cur->value), stats);
        cur = cur->next;
        ++idx;
    }
    recordState(out, count, capacity, -1, -1, false, QString("%1 was not found in the list.").arg(value), stats);
}

void SinglyLinkedList::traverse(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, "Beginning traversal from the head.", stats);
    SLLNode *cur = head_;
    int idx = 0;
    while (cur) {
        stats.arrayAccesses++;
        recordState(out, count, capacity, idx, -1, false, QString("Visiting position %1 (value %2).").arg(idx).arg(cur->value), stats);
        cur = cur->next;
        ++idx;
    }
    recordState(out, count, capacity, -1, -1, false, "Traversal complete.", stats);
}

void SinglyLinkedList::reverse(ListStep *out, int &count, int capacity)
{
    OperationStats stats;

    // Pass 1: capture the original node order as stable visual "slots".
    SLLNode *slotNode[LIST_STEP_MAX];
    int n = 0;
    for (SLLNode *c = head_; c && n < LIST_STEP_MAX; c = c->next) slotNode[n++] = c;
    int values[LIST_STEP_MAX];
    int nextOf[LIST_STEP_MAX];
    for (int i = 0; i < n; ++i) { values[i] = slotNode[i]->value; nextOf[i] = (i + 1 < n) ? i + 1 : -1; }

    auto emitStep = [&](int hi, int si, int headSlot, const QString &desc) {
        if (count >= capacity) return;
        ListStep &s = out[count];
        s.size = n; s.isDoubly = false; s.headSlot = headSlot;
        for (int i = 0; i < n; ++i) { s.values[i] = values[i]; s.nextOf[i] = nextOf[i]; s.prevOf[i] = -1; }
        s.highlightIndex = hi; s.secondaryIndex = si; s.isNewNode = false;
        s.description = desc; s.stats = stats;
        ++count;
    };

    emitStep(-1, -1, 0, "Starting reversal - each node's next pointer will be flipped backward.");

    // Pass 2: perform the REAL pointer reversal, recording a step after
    // every actual pointer flip.
    SLLNode *prev = nullptr, *cur = head_;
    int prevSlot = -1, i = 0;
    while (cur) {
        SLLNode *nextNode = cur->next;
        cur->next = prev;          // the real pointer operation
        stats.swaps++;
        nextOf[i] = prevSlot;
        emitStep(i, prevSlot, 0, QString("Flipped node %1's next pointer to point back to the previous node.").arg(values[i]));
        prev = cur; prevSlot = i; cur = nextNode; ++i;
    }
    head_ = prev;
    emitStep(-1, -1, prevSlot, "Reversal complete - head now points to the former last node.");
}

// ======================= DoublyLinkedList =======================

DoublyLinkedList::DoublyLinkedList() : head_(nullptr), tail_(nullptr) {}

DoublyLinkedList::~DoublyLinkedList()
{
    while (head_) { DLLNode *n = head_->next; delete head_; head_ = n; }
}

void DoublyLinkedList::recordState(ListStep *out, int &count, int capacity, int hi, int si,
                                    bool isNew, const QString &desc, const OperationStats &stats)
{
    if (count >= capacity) return;
    ListStep &s = out[count];
    s.size = 0;
    DLLNode *c = head_;
    while (c && s.size < LIST_STEP_MAX) {
        s.values[s.size] = c->value;
        s.nextOf[s.size] = (c->next && s.size + 1 < LIST_STEP_MAX) ? s.size + 1 : -1;
        s.prevOf[s.size] = (s.size > 0) ? s.size - 1 : -1;
        ++s.size;
        c = c->next;
    }
    s.isDoubly = true;
    s.headSlot = (s.size > 0) ? 0 : -1;
    s.highlightIndex = hi;
    s.secondaryIndex = si;
    s.isNewNode = isNew;
    s.description = desc;
    s.stats = stats;
    ++count;
}

void DoublyLinkedList::insertAtBeginning(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, QString("Inserting %1 at the beginning.").arg(value), stats);
    DLLNode *node = new DLLNode{value, nullptr, head_};
    if (head_) head_->prev = node; else tail_ = node;
    head_ = node;
    stats.insertions++;
    recordState(out, count, capacity, 0, -1, true,
        QString("New node (%1) is the new head; old head's prev pointer now points back to it.").arg(value), stats);
}

void DoublyLinkedList::insertAtEnd(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false,
        QString("Inserting %1 at the end (direct via the tail pointer - O(1)).").arg(value), stats);
    DLLNode *node = new DLLNode{value, tail_, nullptr};
    if (tail_) tail_->next = node; else head_ = node;
    tail_ = node;
    stats.insertions++;
    recordState(out, count, capacity, -1, -1, true, QString("New node (%1) linked after the previous tail.").arg(value), stats);
    if (count > 0) out[count - 1].highlightIndex = out[count - 1].size - 1;
}

bool DoublyLinkedList::deleteFromBeginning(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (!head_) {
        recordState(out, count, capacity, -1, -1, false, "List is empty - nothing to delete.", stats);
        return false;
    }
    recordState(out, count, capacity, 0, -1, false, QString("Deleting head node (%1).").arg(head_->value), stats);
    DLLNode *old = head_;
    head_ = head_->next;
    if (head_) head_->prev = nullptr; else tail_ = nullptr;
    delete old;
    stats.deletions++;
    recordState(out, count, capacity, head_ ? 0 : -1, -1, false, "Head pointer moved on; its prev pointer set to null.", stats);
    return true;
}

bool DoublyLinkedList::deleteFromEnd(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    if (!tail_) {
        recordState(out, count, capacity, -1, -1, false, "List is empty - nothing to delete.", stats);
        return false;
    }
    recordState(out, count, capacity, -1, -1, false,
        QString("Deleting tail node (%1) directly via the tail pointer - O(1).").arg(tail_->value), stats);
    if (count > 0) out[count - 1].highlightIndex = out[count - 1].size - 1;
    DLLNode *old = tail_;
    tail_ = tail_->prev;
    if (tail_) tail_->next = nullptr; else head_ = nullptr;
    delete old;
    stats.deletions++;
    recordState(out, count, capacity, -1, -1, false, "Tail pointer moved to the previous node.", stats);
    return true;
}

void DoublyLinkedList::search(int value, ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, QString("Searching for %1 from the head.").arg(value), stats);
    DLLNode *cur = head_;
    int idx = 0;
    while (cur) {
        stats.comparisons++;
        if (cur->value == value) {
            recordState(out, count, capacity, idx, -1, false, QString("Found %1 at position %2!").arg(value).arg(idx), stats);
            return;
        }
        recordState(out, count, capacity, idx, -1, false, QString("Position %1 (%2) does not match.").arg(idx).arg(cur->value), stats);
        cur = cur->next;
        ++idx;
    }
    recordState(out, count, capacity, -1, -1, false, QString("%1 was not found in the list.").arg(value), stats);
}

void DoublyLinkedList::traverse(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    recordState(out, count, capacity, -1, -1, false, "Beginning traversal from the head.", stats);
    DLLNode *cur = head_;
    int idx = 0;
    while (cur) {
        stats.arrayAccesses++;
        recordState(out, count, capacity, idx, -1, false, QString("Visiting position %1 (value %2).").arg(idx).arg(cur->value), stats);
        cur = cur->next;
        ++idx;
    }
    recordState(out, count, capacity, -1, -1, false, "Traversal complete.", stats);
}

void DoublyLinkedList::reverse(ListStep *out, int &count, int capacity)
{
    OperationStats stats;
    DLLNode *slotNode[LIST_STEP_MAX];
    int n = 0;
    for (DLLNode *c = head_; c && n < LIST_STEP_MAX; c = c->next) slotNode[n++] = c;
    int values[LIST_STEP_MAX], nextOf[LIST_STEP_MAX], prevOf[LIST_STEP_MAX];
    for (int i = 0; i < n; ++i) {
        values[i] = slotNode[i]->value;
        nextOf[i] = (i + 1 < n) ? i + 1 : -1;
        prevOf[i] = (i - 1 >= 0) ? i - 1 : -1;
    }

    auto emitStep = [&](int hi, int headSlot, const QString &desc) {
        if (count >= capacity) return;
        ListStep &s = out[count];
        s.size = n; s.isDoubly = true; s.headSlot = headSlot;
        for (int i = 0; i < n; ++i) { s.values[i] = values[i]; s.nextOf[i] = nextOf[i]; s.prevOf[i] = prevOf[i]; }
        s.highlightIndex = hi; s.secondaryIndex = -1; s.isNewNode = false;
        s.description = desc; s.stats = stats;
        ++count;
    };

    emitStep(-1, 0, "Starting reversal - swapping each node's next and prev pointers.");

    for (int i = 0; i < n; ++i) {
        DLLNode *node = slotNode[i];
        DLLNode *tmp = node->next;
        node->next = node->prev;   // the real pointer operation
        node->prev = tmp;
        stats.swaps++;
        int newNext = prevOf[i];
        int newPrev = nextOf[i];
        nextOf[i] = newNext;
        prevOf[i] = newPrev;
        emitStep(i, 0, QString("Swapped next/prev pointers for node %1.").arg(values[i]));
    }
    DLLNode *oldHead = head_;
    head_ = tail_;
    tail_ = oldHead;
    emitStep(-1, n > 0 ? n - 1 : -1, "Reversal complete - head and tail pointers swapped.");
}
