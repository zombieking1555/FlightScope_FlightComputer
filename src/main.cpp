#include "Arduino.h"
#include "logger.h"
#include "altimeter.h"
#include "flightState.h"
#include "led.h"
#include <Wire.h>
#include <ejection.h>

flight_state state = INIT;
int initCounter = 0;
double launchAltitude;
double highestAltitude;
unsigned int landingCount = 0;
unsigned int apogeeCount = 0;

// Build the ground-test image with:
//   pio run -e rpipico2
// The rpipico2_flight environment is the production flight profile.
#ifndef GROUND_TEST_MODE
#define GROUND_TEST_MODE 1
#endif

namespace {
constexpr uint8_t ALTITUDE_FILTER_SIZE = 5;
constexpr uint16_t INIT_SAMPLE_COUNT = 500;
constexpr uint16_t LAUNCH_CONFIRMATION_COUNT = 10;
constexpr uint16_t APOGEE_CONFIRMATION_COUNT = 10;

#if GROUND_TEST_MODE
constexpr double LAUNCH_RISE_THRESHOLD = 0.8;
constexpr double MIN_CLIMB_FOR_APOGEE = 1.0;
constexpr double APOGEE_DROP_THRESHOLD = 0.5;
constexpr double LANDING_ALTITUDE_TOLERANCE = 0.75;
constexpr unsigned long LANDING_CONFIRMATION_MS = 3000;
#else
// These are deliberately separate from the ground-test values. They should
// be tuned with flight data and should eventually be combined with IMU launch
// confirmation before an actual flight.
constexpr double LAUNCH_RISE_THRESHOLD = 5.0;
constexpr double MIN_CLIMB_FOR_APOGEE = 10.0;
constexpr double APOGEE_DROP_THRESHOLD = 2.0;
constexpr double LANDING_ALTITUDE_TOLERANCE = 2.0;
constexpr unsigned long LANDING_CONFIRMATION_MS = 5000;
#endif

double altitudeSamples[ALTITUDE_FILTER_SIZE] = {};
uint8_t altitudeSampleCount = 0;
uint8_t altitudeSampleIndex = 0;
double filteredAltitude = 0.0;
double baselineSum = 0.0;
uint16_t baselineSampleCount = 0;
unsigned long landingStableSince = 0;
bool altitudeFilterReady = false;

double updateFilteredAltitude(double altitude)
{
  altitudeSamples[altitudeSampleIndex] = altitude;
  altitudeSampleIndex = (altitudeSampleIndex + 1) % ALTITUDE_FILTER_SIZE;
  if (altitudeSampleCount < ALTITUDE_FILTER_SIZE)
    altitudeSampleCount++;

  double sorted[ALTITUDE_FILTER_SIZE];
  for (uint8_t i = 0; i < altitudeSampleCount; i++)
    sorted[i] = altitudeSamples[i];

  for (uint8_t i = 1; i < altitudeSampleCount; i++) {
    double value = sorted[i];
    int8_t j = i - 1;
    while (j >= 0 && sorted[j] > value) {
      sorted[j + 1] = sorted[j];
      j--;
    }
    sorted[j + 1] = value;
  }

  filteredAltitude = sorted[altitudeSampleCount / 2];
  altitudeFilterReady = altitudeSampleCount == ALTITUDE_FILTER_SIZE;
  return filteredAltitude;
}

void resetDetectionCounters()
{
  landingCount = 0;
  apogeeCount = 0;
  landingStableSince = 0;
}
}

const char* flightStateName(flight_state currentState){
  switch(currentState){
  case INIT: return "INIT";
  case IDLE: return "IDLE";
  case LAUNCH: return "LAUNCH";
  case LANDING: return "LANDING";
  case LOGGINGUP: return "LOGGINGUP";
  case LOGGINGDOWN: return "LOGGINGDOWN";
  case FINISH: return "FINISH";
  default: return "UNKNOWN";
  }
}

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

void setup(){
  Serial.begin(115200);
       
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
  if(!initLogger() || !initAltimeter()){
    Serial.println("ERROR: One or more sensors failed to initialize.");
    while(1);
  }

  initLed();
  initEjection();

#if GROUND_TEST_MODE
  Serial.println("GROUND TEST MODE: ejection output enabled after apogee.");
#else
  Serial.println("FLIGHT MODE: ejection output enabled.");
#endif
  
}

void noEjectionLoop(){
  static unsigned long reportLoopCount = 0;
  bool altitudeReadValid = altimeterPeriodic();
  double rawAltitude = getAltitudeMeters();
  double filteredAltitudeForLogging = filteredAltitude;

  if (altitudeReadValid)
    filteredAltitudeForLogging = updateFilteredAltitude(rawAltitude);

  loggerPeriodic(rawAltitude,
                 filteredAltitudeForLogging,
                 isEjectionOutputActive(),
                 state);

  // Never advance the state machine on a stale BMP388 sample.
  if (!altitudeReadValid) {
    Serial.println("Altitude read invalid; state detection skipped.");
    delay(10);
    return;
  }

  double altitudeForDetection = filteredAltitudeForLogging;
  if (!altitudeFilterReady) {
    delay(10);
    return;
  }
  reportLoopCount++;
  if(reportLoopCount >= 10){
    reportLoopCount = 0;
    Serial.print("Flight state: ");
    Serial.print(flightStateName(state));
    Serial.print(" | MOSFET pin 9: ");
    Serial.print(digitalRead(9) == HIGH ? "HIGH" : "LOW");
    Serial.print(" | Altitude: ");
    Serial.print(altitudeForDetection);
    Serial.println(" m");
  }
  switch (state)
  {
  case INIT:
    ledRapidBlink();
    baselineSum += altitudeForDetection;
    baselineSampleCount++;
    initCounter++;
    if(initCounter >= INIT_SAMPLE_COUNT){
      launchAltitude = baselineSum / baselineSampleCount;
      highestAltitude = launchAltitude;
      resetDetectionCounters();
      Serial.print("Flight initialized. Baseline altitude: ");
      Serial.println(launchAltitude);
      state = IDLE;
    }

    break;
  case IDLE:
    
    ledSlowBlink();
    Serial.println("Altitude: " + String(altitudeForDetection) + " m - Launch Altitude: " + String(launchAltitude));
    static unsigned int launchCount = 0;

    if(altitudeForDetection > launchAltitude + LAUNCH_RISE_THRESHOLD){
      launchCount++;
    } else {
      launchCount = 0;
    }

    if(launchCount >= LAUNCH_CONFIRMATION_COUNT){
      launchCount = 0;
      state = LAUNCH;
    }
    break;
  case LAUNCH:
    Serial.println("Launch detected");
    state = LOGGINGUP;
    Serial.println("Logging started");
  break;
  case LOGGINGUP:
    {
    ledFastBlink();
    if(altitudeForDetection > highestAltitude){
      highestAltitude = altitudeForDetection;
    }

    // Do not declare apogee unless the vehicle first achieved meaningful
    // altitude above the calibrated ground level.
    if(highestAltitude >= launchAltitude + MIN_CLIMB_FOR_APOGEE &&
       altitudeForDetection <= highestAltitude - APOGEE_DROP_THRESHOLD){
      apogeeCount++;
      Serial.println("Apogee count: " + String(apogeeCount));
    } else {
      apogeeCount = 0;
    }

    if(apogeeCount >= APOGEE_CONFIRMATION_COUNT){
      Serial.println("Apogee detected");
      apogeeCount = 0;
      landingStableSince = 0;
      state = LOGGINGDOWN;
    }
  
  break;
    }
  case LOGGINGDOWN:
    
    ledFastBlink();
    if(fabs(altitudeForDetection - launchAltitude) <= LANDING_ALTITUDE_TOLERANCE){
      if(landingStableSince == 0)
        landingStableSince = millis();
      landingCount++;
      Serial.println("Landing confirmation count: " + String(landingCount));
    } else {
      landingStableSince = 0;
      landingCount = 0;
    }

    if(landingStableSince != 0 &&
       millis() - landingStableSince >= LANDING_CONFIRMATION_MS)
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
    // ejectionPeriodic() only drives the MOSFET in LOGGINGDOWN, which is
    // entered after the confirmed apogee transition. It remains active in
    // both profiles so ground tests exercise the complete sequence.
    ejectionPeriodic(state);
    noEjectionLoop();
  }

void loop()
{
  mosfetTestingLoop();
}

