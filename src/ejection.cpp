#include "Arduino.h"
#include <Wire.h>
#include "led.h"
#include <flightState.h>

#define MOSFET_PIN 9


void initEjection(){
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);
}

void ejectionPeriodic(flight_state state){
   switch (state){
    case LOGGINGDOWN:
        digitalWrite(MOSFET_PIN, HIGH);
    break;
    default:
        digitalWrite(MOSFET_PIN, LOW);
   }
}
