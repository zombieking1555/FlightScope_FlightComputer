#include "Arduino.h"
#include "logger.h"
#include "imu.h"
#include "altimeter.h"
#include "flightState.h"
#include "led.h"
#include <Adafruit_LSM6DSO32.h>
#include <Wire.h>
#include <ejection.h>

flight_state state = INIT;
int initCounter = 0;
double launchAltitude;
double highestAltitude;
double landingCount = 0;
double apogeeCount = 0;

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

void setup(){
  Serial.begin(115200);
    while (!Serial)
        delay(10);
       
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  

    Serial.println("Scanning I2C...");

    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);

        if (Wire.endTransmission() == 0) {
            Serial.print("Found I2C device at 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    Serial.println("I2C scan complete.");

  // initialize LED digital pin as an output.
  if(!initLogger() || !initAltimeter() || !initIMU(LSM6DSO32_ACCEL_RANGE_16_G, LSM6DS_GYRO_RANGE_1000_DPS, LSM6DS_RATE_1_66K_HZ, LSM6DS_RATE_1_66K_HZ)){
    Serial.println("ERROR: One or more sensors failed to initialize.");
    while(1);
  }

  initLed();
  initEjection();
  
}

void loop()
{
  noEjectionLoop();
}

void noEjectionLoop(){
  altimeterPeriodic();
  imuPeriodic();
  // Serial.print(" | Accel: " + String(getAccelEvent().acceleration.x) + ", " + String(getAccelEvent().acceleration.y) + ", " + String(getAccelEvent().acceleration.z));
  // Serial.println(" | Gyro: " + String(getGyroEvent().gyro.x) + ", " + String(getGyroEvent().gyro.y) + ", " + String(getGyroEvent().gyro.z));
  loggerPeriodic(getAltitudeMeters(), getGyroEvent(), getAccelEvent(), state);
  switch (state)
  {
  case INIT:
    ledRapidBlink();
    Serial.println(initCounter);
    initCounter++;
    if(initCounter > 500){
      launchAltitude = getAltitudeMeters();
      highestAltitude = launchAltitude;
      Serial.println("Flight initialized");
      state = IDLE;
    }

    break;
  case IDLE:
    
    ledSlowBlink();
    Serial.println("Altitude: " + String(getAltitudeMeters()) + " m - Launch Altitude: " + String(launchAltitude));
    // if(getAccelEvent().acceleration.z > 20){
    if(getAltitudeMeters() > launchAltitude + .3){
      state = LAUNCH;
    }
    break;
  case LAUNCH:
    Serial.println("Launch detected");
    state = LOGGINGUP;
    Serial.println("Logging started");
  break;
  case LOGGINGUP:
    
    ledFastBlink();
    double altitude = getAltitudeMeters();

    if(altitude > highestAltitude){
      highestAltitude = altitude;
    }

    if(altitude <= highestAltitude - .5){
      apogeeCount++;
      Serial.println("Apogee count: " + String(apogeeCount));
    } else {
      apogeeCount = 0;
    }

    if(apogeeCount >=10){
      Serial.println("Apogee detected");
      state = LOGGINGDOWN;
    }
  break;
  case LOGGINGDOWN:
    
    ledFastBlink();
    if(getAltitudeMeters() <= launchAltitude + .5){
      landingCount++;
      Serial.println("Landing count: " + String(landingCount));
    }
    else
      landingCount = 0;
    if(landingCount >= 500)
      state = LANDING;
    break;
  case LANDING:
    Serial.println("Landing detected");
    endFlight();
    state = FINISH;
    break;
  case FINISH:
  
    ledSlowBlink();
    Serial.println("Flight finished");
    break;
  }
  //logging loop delay
  delay(10);
}

  void mosfetTestingLoop(){
    ejectionPeriodic(state);
    noEjectionLoop();
  }
