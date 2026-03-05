#include <iostream>
#include "reports.h"
using namespace std;

// Print titles of all events in the list.
void reportAllTitles(Node* head) {
    cout << "\n[REPORT 1] All titles:\n";
    if (head == nullptr) {
        cout << "No data.\n";
        return;
    }

    Node* current = head;
    while (current != nullptr) {
        cout << "- " << current->data.title << "\n";
        current = current->next;
    }
}

// Print events whose theme contains the requested text.
void reportByTheme(Node* head, const string& theme) {
    cout << "\n[REPORT 2] Events by theme: " << theme << "\n";
    bool hasData = false;

    Node* current = head;
    while (current != nullptr) {
        if (current->data.theme.find(theme) != string::npos) {
            cout << "- " << current->data.title << " (" << current->data.date.month << "." << current->data.date.year << ")\n";
            hasData = true;
        }
        current = current->next;
    }

    if (!hasData) {
        cout << "No events found.\n";
    }
}

// Print events that describe a Bulgarian victory outcome.
void reportBulgarianWins(Node* head) {
    cout << "\n[REPORT 3] Events with Bulgaria victory:\n";
    bool hasData = false;

    Node* current = head;
    while (current != nullptr) {
        if (current->data.result.find("Bulg") != string::npos && current->data.result.find("victor") != string::npos) {
            cout << "- " << current->data.title << " | " << current->data.result << "\n";
            hasData = true;
        }
        current = current->next;
    }

    if (!hasData) {
        cout << "No events found.\n";
    }
}
