#include <iostream>
#include <string>
#include "database.h"
using namespace std;

namespace {
bool execSql(sqlite3* db, const char* sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) {
            cerr << "SQLite error: " << errMsg << "\n";
            sqlite3_free(errMsg);
        }
        return false;
    }
    return true;
}

Event rowToEvent(sqlite3_stmt* stmt) {
    Event e;
    e.id = sqlite3_column_int(stmt, 0);
    e.date.month = sqlite3_column_int(stmt, 1);
    e.date.year = sqlite3_column_int(stmt, 2);

    const unsigned char* theme = sqlite3_column_text(stmt, 3);
    const unsigned char* title = sqlite3_column_text(stmt, 4);
    const unsigned char* place = sqlite3_column_text(stmt, 5);
    const unsigned char* leader = sqlite3_column_text(stmt, 6);
    const unsigned char* participants = sqlite3_column_text(stmt, 7);
    const unsigned char* result = sqlite3_column_text(stmt, 8);

    e.theme = theme ? reinterpret_cast<const char*>(theme) : "";
    e.title = title ? reinterpret_cast<const char*>(title) : "";
    e.place = place ? reinterpret_cast<const char*>(place) : "";
    e.leader = leader ? reinterpret_cast<const char*>(leader) : "";
    e.participants = participants ? reinterpret_cast<const char*>(participants) : "";
    e.result = result ? reinterpret_cast<const char*>(result) : "";
    return e;
}
}

Database::Database() : dbPath("events.db"), conn(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open(const string& filePath) {
    dbPath = filePath;
    close();

    int rc = sqlite3_open(dbPath.c_str(), &conn);
    if (rc != SQLITE_OK) {
        cerr << "Error: cannot open sqlite database.\n";
        close();
        return false;
    }

    return createTable();
}

void Database::close() {
    if (conn) {
        sqlite3_close(conn);
        conn = nullptr;
    }
}

bool Database::createTable() {
    if (!conn) return false;

    const char* sql =
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "month INTEGER NOT NULL,"
        "year INTEGER NOT NULL,"
        "theme TEXT NOT NULL,"
        "title TEXT NOT NULL,"
        "place TEXT NOT NULL,"
        "leader TEXT NOT NULL,"
        "participants TEXT NOT NULL,"
        "result TEXT NOT NULL"
        ");";

    return execSql(conn, sql);
}

bool Database::insertEvent(const Event& event) {
    if (!conn) return false;

    const char* sql =
        "INSERT INTO events(month, year, theme, title, place, leader, participants, result) "
        "VALUES(?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, event.date.month);
    sqlite3_bind_int(stmt, 2, event.date.year);
    sqlite3_bind_text(stmt, 3, event.theme.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, event.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, event.place.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, event.leader.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, event.participants.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, event.result.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::updateEvent(const Event& event) {
    if (!conn) return false;

    const char* sql =
        "UPDATE events SET month=?, year=?, theme=?, title=?, place=?, leader=?, participants=?, result=? "
        "WHERE id=?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, event.date.month);
    sqlite3_bind_int(stmt, 2, event.date.year);
    sqlite3_bind_text(stmt, 3, event.theme.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, event.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, event.place.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, event.leader.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, event.participants.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, event.result.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, event.id);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    int changes = sqlite3_changes(conn);
    sqlite3_finalize(stmt);
    return ok && changes > 0;
}

bool Database::deleteEvent(int id) {
    if (!conn) return false;

    const char* sql = "DELETE FROM events WHERE id=?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    int changes = sqlite3_changes(conn);
    sqlite3_finalize(stmt);
    return ok && changes > 0;
}

vector<Event> Database::loadAllEvents() {
    vector<Event> events;
    if (!conn) return events;

    const char* sql =
        "SELECT id, month, year, theme, title, place, leader, participants, result "
        "FROM events ORDER BY year ASC, month ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return events;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        events.push_back(rowToEvent(stmt));
    }

    sqlite3_finalize(stmt);
    return events;
}

vector<Event> Database::searchByYear(int year) {
    vector<Event> events;
    if (!conn) return events;

    const char* sql =
        "SELECT id, month, year, theme, title, place, leader, participants, result "
        "FROM events WHERE year = ? ORDER BY year ASC, month ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return events;

    sqlite3_bind_int(stmt, 1, year);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        events.push_back(rowToEvent(stmt));
    }

    sqlite3_finalize(stmt);
    return events;
}

vector<Event> Database::searchByTheme(const string& theme) {
    vector<Event> events;
    if (!conn) return events;

    const char* sql =
        "SELECT id, month, year, theme, title, place, leader, participants, result "
        "FROM events WHERE theme LIKE ? ORDER BY year ASC, month ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) return events;

    string pattern = "%" + theme + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        events.push_back(rowToEvent(stmt));
    }

    sqlite3_finalize(stmt);
    return events;
}
