#ifndef INPUT_H
#define INPUT_H

#include "event.h"
using namespace std;

int readInt(const string& label, int minValue, int maxValue);
string readNonEmptyString(const string& label);
Event readEventFromUser();

#endif
