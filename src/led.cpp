#include "Arduino.h"
#include <Wire.h>
#include "led.h"

#define LED_PIN 15

float counter = 0;

void initLed() {
    pinMode(LED_PIN, OUTPUT);
}

void ledOn() {
    digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
    digitalWrite(LED_PIN, LOW);
}

void ledSlowBlink() {
    if(counter == 0){
        ledOn();
        counter++;
    } else if(counter == 50){
        ledOff();
        counter++;
    } else if(counter >= 100){
        counter = 0;
    } else {
        counter++;
    }
}

void ledFastBlink() {
    if(counter == 0){
        ledOn();
        counter++;
    } else if(counter == 25){
        ledOff();
        counter++;
    } else if(counter >= 50){
        counter = 0;
    } else {
        counter++;
    }
}

void ledRapidBlink() {
    if(counter == 0){
        ledOn();
        counter++;
    } else if(counter == 7){
        ledOff();
        counter++;
    } else if(counter >= 14){
        counter = 0;
    } else {
        counter++;
    }
}