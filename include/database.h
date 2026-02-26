#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <sqlite3.h>
#include "event.h"
using namespace std;

class Database {
private:
    string dbPath;
    sqlite3* conn;

public:
    Database();
    ~Database();

    bool open(const string& filePath);
    void close();
    bool createTable();

    bool insertEvent(const Event& event);
    bool updateEvent(const Event& event);
    bool deleteEvent(int id);

    vector<Event> loadAllEvents();
    vector<Event> searchByYear(int year);
    vector<Event> searchByTheme(const string& theme);
};

#endif
