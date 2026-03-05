#include <iostream>
#include <iomanip>
#include "event_list.h"
using namespace std;

// Initialize an empty linked list.
EventList::EventList() {
    head = nullptr;
}

// Release all nodes when the list object is destroyed.
EventList::~EventList() {
    clear();
}

// Delete every node and reset the list to an empty state.
void EventList::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* toDelete = current;
        current = current->next;
        delete toDelete;
    }
    head = nullptr;
}

// Check whether the list has no elements.
bool EventList::isEmpty() const {
    return head == nullptr;
}

// Insert the first element only if the list is currently empty.
void EventList::addFirst(const Event& event) {
    if (head != nullptr) {
        return;
    }
    head = new Node{event, nullptr};
}

// Push a new event to the beginning of the list.
void EventList::addAtStart(const Event& event) {
    Node* newNode = new Node{event, head};
    head = newNode;
}

// Append a new event to the end of the list.
void EventList::addAtEnd(const Event& event) {
    Node* newNode = new Node{event, nullptr};
    if (head == nullptr) {
        head = newNode;
        return;
    }

    Node* current = head;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = newNode;
}

// Insert an event while keeping the list ordered by date.
void EventList::addSortedByDate(const Event& event, bool ascending) {
    Node* newNode = new Node{event, nullptr};

    if (head == nullptr) {
        head = newNode;
        return;
    }

    auto isBefore = [ascending](const Event& left, const Event& right) {
        if (left.date.year != right.date.year) {
            return ascending ? left.date.year < right.date.year : left.date.year > right.date.year;
        }
        return ascending ? left.date.month < right.date.month : left.date.month > right.date.month;
    };

    if (isBefore(event, head->data)) {
        newNode->next = head;
        head = newNode;
        return;
    }

    Node* current = head;
    while (current->next != nullptr && !isBefore(event, current->next->data)) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
}

// Replace an event by ID while keeping the original ID unchanged.
bool EventList::editById(int id, const Event& updatedEvent) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.id == id) {
            current->data = updatedEvent;
            current->data.id = id;
            return true;
        }
        current = current->next;
    }
    return false;
}

// Remove the event with the given ID and relink the list.
bool EventList::deleteById(int id) {
    if (head == nullptr) {
        return false;
    }

    if (head->data.id == id) {
        Node* oldHead = head;
        head = head->next;
        delete oldHead;
        return true;
    }

    Node* current = head;
    while (current->next != nullptr && current->next->data.id != id) {
        current = current->next;
    }

    if (current->next == nullptr) {
        return false;
    }

    Node* toDelete = current->next;
    current->next = toDelete->next;
    delete toDelete;
    return true;
}

// Find the first event that matches a year and optional month.
Node* EventList::findByDate(int year, int month) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.date.year == year && (month == 0 || current->data.date.month == month)) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Find the first event whose theme exactly matches the value.
Node* EventList::findByTheme(const string& theme) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.theme == theme) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Provide direct access to the list head pointer.
Node* EventList::getHead() const {
    return head;
}

// Print all events in a readable table-like format.
void EventList::printAll() const {
    if (head == nullptr) {
        cout << "List is empty.\n";
        return;
    }

    cout << "\n=== Historical events ===\n";
    Node* current = head;
    while (current != nullptr) {
        const Event& e = current->data;
        cout << "ID: " << e.id << " | "
             << setw(2) << setfill('0') << e.date.month << "." << e.date.year << setfill(' ')
             << " | " << e.title << " | Theme: " << e.theme
             << " | Place: " << e.place
             << " | Leader: " << e.leader
             << " | Result: " << e.result << "\n";
        current = current->next;
    }
}
