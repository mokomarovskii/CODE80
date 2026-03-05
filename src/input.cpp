#include <iostream>
#include <limits>
#include "input.h"
using namespace std;

// Read an integer from input and keep asking until it is valid.
int readInt(const string& label, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << label;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid number. Try again.\n";
            continue;
        }

        if (value < minValue || value > maxValue) {
            cout << "Number must be between " << minValue << " and " << maxValue << ".\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}

// Read a text value and reject empty input.
string readNonEmptyString(const string& label) {
    string value;
    while (true) {
        cout << label;
        getline(cin, value);
        if (!value.empty()) {
            return value;
        }
        cout << "Field cannot be empty.\n";
    }
}

// Read all event fields from the console and build an Event.
Event readEventFromUser() {
    Event e;
    e.id = 0;

    e.date.month = readInt("Month (1-12): ", 1, 12);
    e.date.year = readInt("Year (1-3000): ", 1, 3000);

    e.theme = readNonEmptyString("Theme: ");
    e.title = readNonEmptyString("Title: ");
    e.place = readNonEmptyString("Place: ");
    e.leader = readNonEmptyString("Leader: ");
    e.participants = readNonEmptyString("Participants: ");
    e.result = readNonEmptyString("Result: ");

    return e;
}
