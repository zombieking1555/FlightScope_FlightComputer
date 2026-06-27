#include "Arduino.h"
#include "logger.h"
#include "imu.h"
#include "altimeter.h"
#include "flightState.h"
#include <Adafruit_LSM6DSO32.h>

flight_state state = IDLE;
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
  switch (state)
  {
  case IDLE:
    /* code */
    break;
  case LOGGING:
    /* code */
    break;
  case FINISH:
    /* code */
    break;
  
  default:
    break;
  }
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);

  // wait for a second
  delay(1000);

  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(1000);
}