#ifndef ALTIMETER_H
#define ALTIMETER_H

void initAltimeter();

/**************************************************************************/
/*!
    @brief Reads new values and assigns them to internal variables. Returns success status

    Reads altitude (m), atmospheric pressure (hPa), and altitude(m). Returns true 
    on a successful read, and false otherwise.

    @return Success of value read
*/
/**************************************************************************/
bool altimeterPeriodic();

double getTempCelcius();
double getPressureKPA();
double getAltitudeMeters();
#endif