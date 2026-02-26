#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include "event.h"

struct Node {
    Event data;
    Node* next;
};

class EventList {
private:
    Node* head;

public:
    EventList();
    ~EventList();

    void clear();
    bool isEmpty() const;

    void addFirst(const Event& event);
    void addAtStart(const Event& event);
    void addAtEnd(const Event& event);
    void addSortedByDate(const Event& event, bool ascending = true);

    bool editById(int id, const Event& updatedEvent);
    bool deleteById(int id);

    Node* findByDate(int year, int month = 0) const;
    Node* findByTheme(const string& theme) const;
    Node* getHead() const;

    void printAll() const;
};

#endif
