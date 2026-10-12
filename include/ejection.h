#include "flightState.h"
#ifndef EJECTION_H
#define EJECTION_H

void initEjection();

void ejectionPeriodic(flight_state state);
bool isEjectionOutputActive();

#endif // EJECTION_H
