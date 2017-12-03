#ifndef DEQUE_H
#define DEQUE_H

struct DequeItem {
  DequeItem *prev, *next;
  int v;

  DequeItem(int v);
};

struct Deque {
  /** Dummy item, dummy->next = front, dummy->prev = back.
    Always non-NULL. */
  DequeItem *dummy;

  Deque();
  ~Deque();

  bool isEmpty();
  int front();
  int back();
  void insertFront(int item);
  void insertBack(int item);
  void deleteFront();
  void deleteBack();
};

#endif
