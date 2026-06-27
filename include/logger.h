#ifndef LOGGER_H
#define LOGGER_H
#include <Adafruit_LSM6DSO32.h>

void initLogger();
void loggerPeriodic(double altitudeMeters, sensors_event_t gyro, sensors_event_t accel, flight_state state);
void endFlight();

#endif LOGGER_H