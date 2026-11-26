#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "ListStep.h"

struct SLLNode {
    int value;
    SLLNode *next;
};

// A genuine pointer-based singly linked list (no STL). Every operation
// records a ListStep (built by traversing the real node chain) after each
// pointer-level change, so the GUI can step through exactly how head/next
// pointers moved.
class SinglyLinkedList {
public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    void insertAtBeginning(int value, ListStep *out, int &count, int capacity);
    void insertAtEnd(int value, ListStep *out, int &count, int capacity);
    bool deleteFromBeginning(ListStep *out, int &count, int capacity);
    bool deleteFromEnd(ListStep *out, int &count, int capacity);
    void search(int value, ListStep *out, int &count, int capacity);
    void reverse(ListStep *out, int &count, int capacity);
    void traverse(ListStep *out, int &count, int capacity);

private:
    SLLNode *head_;
    void recordState(ListStep *out, int &count, int capacity, int hi, int si,
                      bool isNew, const QString &desc, const OperationStats &stats);
};

struct DLLNode {
    int value;
    DLLNode *prev;
    DLLNode *next;
};

// Same idea as SinglyLinkedList but with genuine prev/next pointers in both
// directions, so Delete-from-End and Reverse are O(1) traversal-aware /
// clearly different from the singly-linked versions - which is the point
// students are meant to observe.
class DoublyLinkedList {
public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insertAtBeginning(int value, ListStep *out, int &count, int capacity);
    void insertAtEnd(int value, ListStep *out, int &count, int capacity);
    bool deleteFromBeginning(ListStep *out, int &count, int capacity);
    bool deleteFromEnd(ListStep *out, int &count, int capacity);
    void search(int value, ListStep *out, int &count, int capacity);
    void reverse(ListStep *out, int &count, int capacity);
    void traverse(ListStep *out, int &count, int capacity);

private:
    DLLNode *head_;
    DLLNode *tail_;
    void recordState(ListStep *out, int &count, int capacity, int hi, int si,
                      bool isNew, const QString &desc, const OperationStats &stats);
};

#endif // LINKEDLIST_H
