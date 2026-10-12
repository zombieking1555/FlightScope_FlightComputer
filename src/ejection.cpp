#include "Arduino.h"
#include <Wire.h>
#include "led.h"
#include <flightState.h>

#define MOSFET_PIN 9

int EJECTION_LENGTH = 50;


void initEjection(){
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);
}

void ejectionPeriodic(flight_state state){
   switch (state){
    case LOGGINGDOWN:
        if(EJECTION_LENGTH > 0){
            digitalWrite(MOSFET_PIN, HIGH);
        }

        EJECTION_LENGTH--;

        if(EJECTION_LENGTH <= 0){
            digitalWrite(MOSFET_PIN, LOW);
        }

    break;
    default:
        digitalWrite(MOSFET_PIN, LOW);
   }
}

bool isEjectionOutputActive(){
    return digitalRead(MOSFET_PIN) == HIGH;
}
