#include <iostream>
#include <vector>
#include "event_list.h"
#include "database.h"
using namespace std;

bool testSortedInsert() {
    EventList list;

    Event e1{1, {5, 1876}, "Въстание", "Априлско въстание", "България", "Г. Бенковски", "народ", "героична борба"};
    Event e2{2, {3, 1878}, "Мир", "Санстефански договор", "Сан Стефано", "", "", "дипломатически успех"};
    Event e3{3, {1, 1912}, "Война", "Балканска война", "Балкани", "Цар Фердинанд", "армия", "победа за България"};

    list.addSortedByDate(e3);
    list.addSortedByDate(e1);
    list.addSortedByDate(e2);

    Node* head = list.getHead();
    if (head == nullptr) return false;
    if (head->data.id != 1) return false;
    if (head->next == nullptr || head->next->data.id != 2) return false;
    if (head->next->next == nullptr || head->next->next->data.id != 3) return false;

    return true;
}

bool testDatabaseInsertAndSearch() {
    Database db;
    if (!db.open("test_events.db")) return false;
    if (!db.createTable()) return false;

    Event e{0, {9, 1885}, "Съединение", "Съединението", "Пловдив", "Княз Александър", "народ", "победа за България"};
    if (!db.insertEvent(e)) return false;

    vector<Event> events = db.searchByYear(1885);
    db.close();

    return !events.empty();
}

int main() {
    int passed = 0;
    int total = 2;

    if (testSortedInsert()) {
        cout << "[PASS] testSortedInsert\n";
        passed++;
    } else {
        cout << "[FAIL] testSortedInsert\n";
    }

    if (testDatabaseInsertAndSearch()) {
        cout << "[PASS] testDatabaseInsertAndSearch\n";
        passed++;
    } else {
        cout << "[FAIL] testDatabaseInsertAndSearch\n";
    }

    cout << "Passed " << passed << " / " << total << " tests.\n";
    return (passed == total) ? 0 : 1;
}
