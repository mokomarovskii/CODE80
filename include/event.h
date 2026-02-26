#ifndef EVENT_H
#define EVENT_H

#include <string>
using namespace std;

struct Date {
    int month;
    int year;
};

struct Event {
    int id;
    Date date;
    string theme;
    string title;
    string place;
    string leader;
    string participants;
    string result;
};

#endif
