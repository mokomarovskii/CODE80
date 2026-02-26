#include <iostream>
#include <vector>
#include "database.h"
#include "event_list.h"
#include "input.h"
#include "reports.h"
using namespace std;

void loadFromDbToList(Database& db, EventList& list) {
    list.clear();
    vector<Event> events = db.loadAllEvents();
    for (const Event& e : events) {
        list.addSortedByDate(e, true);
    }
}

void showMenu() {
    cout << "\n===== TIME LIST =====\n";
    cout << "1. Add first element\n";
    cout << "2. Add at start\n";
    cout << "3. Add at end\n";
    cout << "4. Add sorted by date\n";
    cout << "5. Edit by ID\n";
    cout << "6. Search by year\n";
    cout << "7. Search by theme\n";
    cout << "8. Delete by ID\n";
    cout << "9. Show all\n";
    cout << "10. Reports\n";
    cout << "0. Exit\n";
}

int main() {
    Database db;
    if (!db.open("events.db")) {
        return 1;
    }
    if (!db.createTable()) {
        return 1;
    }

    EventList list;
    loadFromDbToList(db, list);

    int choice;
    do {
        showMenu();
        choice = readInt("Choose option: ", 0, 10);

        if (choice >= 1 && choice <= 4) {
            Event e = readEventFromUser();
            if (!db.insertEvent(e)) {
                cout << "DB write error.\n";
            }
            loadFromDbToList(db, list);
            cout << "Added successfully.\n";
        } else if (choice == 5) {
            int id = readInt("ID to edit: ", 1, 1000000);
            Event e = readEventFromUser();
            e.id = id;
            if (db.updateEvent(e)) {
                loadFromDbToList(db, list);
                cout << "Updated successfully.\n";
            } else {
                cout << "Update failed.\n";
            }
        } else if (choice == 6) {
            int year = readInt("Year: ", 1, 3000);
            vector<Event> events = db.searchByYear(year);
            cout << "Found: " << events.size() << "\n";
            for (const Event& e : events) {
                cout << e.id << " | " << e.date.month << "." << e.date.year << " | " << e.title << "\n";
            }
        } else if (choice == 7) {
            string theme = readNonEmptyString("Theme: ");
            vector<Event> events = db.searchByTheme(theme);
            cout << "Found: " << events.size() << "\n";
            for (const Event& e : events) {
                cout << e.id << " | " << e.date.month << "." << e.date.year << " | " << e.title << "\n";
            }
        } else if (choice == 8) {
            int id = readInt("ID to delete: ", 1, 1000000);
            if (db.deleteEvent(id)) {
                loadFromDbToList(db, list);
                cout << "Deleted successfully.\n";
            } else {
                cout << "Record not found.\n";
            }
        } else if (choice == 9) {
            list.printAll();
        } else if (choice == 10) {
            reportAllTitles(list.getHead());
            string theme = readNonEmptyString("Theme for report 2: ");
            reportByTheme(list.getHead(), theme);
            reportBulgarianWins(list.getHead());
        }
    } while (choice != 0);

    cout << "Bye!\n";
    return 0;
}