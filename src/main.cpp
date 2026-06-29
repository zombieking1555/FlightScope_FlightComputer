#include "Arduino.h"
#include "logger.h"
#include "imu.h"
#include "altimeter.h"
#include "flightState.h"
#include <Adafruit_LSM6DSO32.h>

flight_state state = IDLE;
double launchAltitude;
double landingCount = 0;

double ledCount = 0;
// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

void setup(){
  Serial.begin(115200);
    while (!Serial)
        delay(10);
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  initAltimeter();
  initIMU(LSM6DSO32_ACCEL_RANGE_16_G, LSM6DS_GYRO_RANGE_1000_DPS, LSM6DS_RATE_1_66K_HZ, LSM6DS_RATE_1_66K_HZ);
  initLogger();
}

void loop()
{
  altimeterPeriodic();
  imuPeriodic();
  loggerPeriodic(getAltitudeMeters(), getGyroEvent(), getAccelEvent(), state);
  switch (state)
  {
  case IDLE:
    launchAltitude = getAltitudeMeters();
    if(getAccelEvent().acceleration.z > 20){
      state = LAUNCH;
    }
    break;
  case LAUNCH:
    state = LOGGING;
  break;
  case LOGGING:
    if(getAltitudeMeters() <= launchAltitude + .5)
      landingCount++;
    else
      landingCount = 0;
    if(landingCount >= 500)
      state = LANDING;
    break;
  case LANDING:
    endFlight();
    state = FINISH;
  case FINISH:
    break;
  }
  
  //LED Loop, one second on, one second off
  if(ledCount < 100){
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_BUILTIN, HIGH);
    ledCount++;
  }
  
  if(ledCount >= 100){
    // turn the LED off by making the voltage LOW
    digitalWrite(LED_BUILTIN, LOW);
    ledCount++;
    if(ledCount > 200)
      ledCount = 0;
  }
  
  //logging loop delay
  delay(10);
}