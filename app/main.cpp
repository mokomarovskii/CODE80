#include <iostream>
#include <vector>
#include "database.h"
#include "event_list.h"
#include "input.h"
#include "reports.h"
using namespace std;



// Seed the database with predefined events when it is empty.
void loadBulgarianWinsPreset(Database& db) {
    if (!db.loadAllEvents().empty()) {
        return;
    }

    struct PresetEvent {
        const char* theme;
        const char* title;
        int month;
        int year;
        const char* place;
        const char* leader;
        const char* participants;
        const char* result;
    };

    const PresetEvent preset[] = {
        {"Statehood", "Founding of Danubian Bulgaria", 7, 681, "Balkans", "Khan Asparuh", "Proto-Bulgarians and Slavs", "Bulgarian statehood internationally acknowledged by Byzantium."},
        {"Diplomacy", "Peace treaty with Byzantium", 10, 716, "Constantinople", "Khan Tervel", "First Bulgarian Empire and Byzantine Empire", "Long-term peace and tribute favorable to Bulgaria."},
        {"Religion", "Christianization of Bulgaria", 3, 864, "Pliska", "Knyaz Boris I", "Bulgarian state and church", "Adoption of Christianity unified and stabilized the country."},
        {"Culture", "Disciples of Cyril and Methodius received", 5, 886, "Pliska", "Knyaz Boris I", "Clement, Naum, Angelarius and Bulgarian clergy", "Foundation of major Slavic literary and educational centers."},
        {"Culture", "Preslav Literary School flourishes", 1, 893, "Preslav", "Tsar Simeon I", "Bulgarian scholars and clergy", "Old Bulgarian became a leading literary language in Orthodox Europe."},
        {"Statehood", "Simeon I proclaimed Tsar", 8, 913, "Constantinople", "Tsar Simeon I", "Bulgaria and Byzantium", "Bulgarian ruler elevated to imperial status."},
        {"Military", "Battle of Achelous", 8, 917, "Near Anchialos", "Tsar Simeon I", "Bulgaria vs Byzantine Empire", "Major Bulgarian victory securing regional dominance."},
        {"Statehood", "Samuil crowned Tsar", 6, 997, "Prespa", "Tsar Samuil", "Bulgarian nobility and clergy", "Continuity of Bulgarian state institutions affirmed."},
        {"Statehood", "Uprising of Asen and Peter", 10, 1185, "Tarnovo", "Theodore-Peter and Ivan Asen I", "Bulgarians and Vlachs", "Restoration of Bulgarian statehood (Second Bulgarian Empire)."},
        {"Military", "Battle of Adrianople", 4, 1205, "Adrianople", "Tsar Kaloyan", "Bulgaria and Cumans vs Latin Empire", "Crushing victory over the Latin Empire."},
        {"Military", "Battle of Klokotnitsa", 3, 1230, "Klokotnitsa", "Tsar Ivan Asen II", "Bulgaria vs Epirus", "Territorial and political peak of the Second Bulgarian Empire."},
        {"Defense", "Defense of Vidin Tsardom autonomy", 5, 1369, "Vidin", "Tsar Ivan Sratsimir", "Vidin Tsardom", "Temporary preservation of Bulgarian rule in the northwest."},
        {"National Revival", "Paisii writes Slavo-Bulgarian History", 1, 1762, "Hilandar", "Paisii Hilendarski", "Bulgarian clergy and intellectuals", "Catalyzed modern Bulgarian national consciousness."},
        {"Education", "First modern secular school in Gabrovo", 1, 1835, "Gabrovo", "Vasil Aprilov", "Bulgarian educators and students", "Start of modern Bulgarian secular education."},
        {"Church", "Bulgarian Exarchate established", 2, 1870, "Constantinople", "Bulgarian church leaders", "Ottoman authorities and Bulgarian Orthodox community", "Restoration of autonomous Bulgarian church governance."},
        {"Revolution", "April Uprising", 4, 1876, "Ottoman Bulgaria", "Georgi Benkovski and local committees", "Bulgarian revolutionaries", "Drew international attention to Bulgarian liberation cause."},
        {"Liberation", "Treaty of San Stefano", 3, 1878, "San Stefano", "Russian and Ottoman negotiators", "Great Powers, Ottoman Empire, Bulgarians", "Recognized a large autonomous Bulgarian entity."},
        {"Statehood", "Constitution of Tarnovo adopted", 4, 1879, "Tarnovo", "Constituent Assembly", "Bulgarian political elite", "Established liberal constitutional order."},
        {"Statehood", "Unification of Bulgaria", 9, 1885, "Plovdiv", "Alexander I of Battenberg", "Principality of Bulgaria and Eastern Rumelia", "National unification achieved despite external pressure."},
        {"Military", "Battle of Slivnitsa", 11, 1885, "Slivnitsa", "Prince Alexander I", "Bulgaria vs Kingdom of Serbia", "Successful defense of unification."},
        {"Diplomacy", "International recognition of Unification", 4, 1886, "Istanbul", "Bulgarian diplomacy", "Great Powers and Ottoman Empire", "Political settlement accepted de facto unification."},
        {"Independence", "Proclamation of Bulgarian Independence", 9, 1908, "Veliko Tarnovo", "Tsar Ferdinand I", "Bulgarian state and people", "Full sovereignty of Bulgaria proclaimed."},
        {"Military", "Capture of Edirne", 3, 1913, "Edirne", "Gen. Nikola Ivanov", "Bulgaria and allies vs Ottoman Empire", "Major strategic success in the First Balkan War."},
        {"Military", "Battle of Tutrakan", 9, 1916, "Tutrakan", "Gen. Panteley Kiselov", "Bulgaria and Germany vs Romania", "Significant Bulgarian success on the Romanian front."},
        {"Military", "Battle of Doiran", 9, 1918, "Doiran", "Gen. Vladimir Vazov", "Bulgaria vs British and Greek forces", "Successful defense with high international recognition."},
        {"Diplomacy", "Treaty of Craiova", 9, 1940, "Craiova", "Bulgarian and Romanian delegations", "Bulgaria and Romania", "Southern Dobrudja returned to Bulgaria peacefully."},
        {"Rescue", "Rescue of Bulgarian Jews", 3, 1943, "Kingdom of Bulgaria", "Dimitar Peshev and Bulgarian society", "Bulgarian institutions, church and citizens", "Deportation of most Bulgarian Jews was prevented."},
        {"International", "Bulgaria joins the United Nations", 12, 1955, "New York", "Bulgarian diplomacy", "UN member states", "Integration into global post-war institutions."},
        {"International", "Bulgaria joins NATO", 3, 2004, "Washington", "Bulgarian government", "NATO member states", "Security and strategic integration with Euro-Atlantic allies."},
        {"International", "Bulgaria joins the European Union", 1, 2007, "Brussels", "Bulgarian government", "EU member states", "Economic and political integration in the EU."}
    };

    for (const PresetEvent& item : preset) {
        Event e;
        e.theme = item.theme;
        e.title = item.title;
        e.date.month = item.month;
        e.date.year = item.year;
        e.place = item.place;
        e.leader = item.leader;
        e.participants = item.participants;
        e.result = std::string(item.result) + " Bulgaria victor in positive outcome.";
        db.insertEvent(e);
    }
}

// Reload linked-list data from the database in chronological order.
void loadFromDbToList(Database& db, EventList& list) {
    list.clear();
    vector<Event> events = db.loadAllEvents();
    for (const Event& e : events) {
        list.addSortedByDate(e, true);
    }
}

// Display the console menu with available actions.
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

// Run the console application loop for managing events.
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
                cout << "Database write error.\n";
            }
            loadFromDbToList(db, list);
            cout << "Added successfully\n";
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

    cout << "Work finished\n";
    return 0;
}