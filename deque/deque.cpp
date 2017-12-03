#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <assert.h> // do we need a different assert thingy or like another assert file?

#include "deque.h"

DequeItem::DequeItem(int v) {
  this->v = v;
  next = prev = NULL;
}

Deque::Deque() {
  dummy = new DequeItem(0);
  assert(dummy != NULL);
  dummy->next = dummy;
  dummy->prev = dummy;
}

Deque::~Deque() {
  while (!isEmpty())
    deleteFront();
  delete dummy;
}

bool Deque::isEmpty() {
  return dummy->next == dummy && dummy->prev == dummy;
}

int Deque::front() {
  assert(!isEmpty());
  return dummy->next->v;
}

int Deque::back() {
  assert(!isEmpty());
  return dummy->prev->v;
}

void Deque::insertFront(int item) {
  DequeItem *tmp = new DequeItem(item);
  assert(tmp != NULL);
  tmp->prev = dummy;
  tmp->next = dummy->next;
  dummy->next->prev = tmp;
  dummy->next = tmp;
}

void Deque::insertBack(int item) {
  DequeItem *tmp = new DequeItem(item);
  assert(tmp != NULL);
  tmp->next = dummy;
  tmp->prev = dummy->prev;
  dummy->prev->next = tmp;
  dummy->prev = tmp;
}

void Deque::deleteFront() {
  assert(!isEmpty());
  DequeItem *tmp = dummy->next;
  dummy->next = tmp->next;
  dummy->next->prev = dummy;
  delete tmp;
}

void Deque::deleteBack() {
  assert(!isEmpty());
  DequeItem *tmp = dummy->prev;
  dummy->prev = tmp->prev;
  dummy->prev->next = dummy;
  delete tmp;
}
