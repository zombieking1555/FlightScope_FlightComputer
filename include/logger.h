#ifndef LOGGER_H
#define LOGGER_H

#include "flightState.h"

bool initLogger();
void loggerPeriodic(double altitudeMeters, flight_state state);
void endFlight();

#endif // LOGGER_H