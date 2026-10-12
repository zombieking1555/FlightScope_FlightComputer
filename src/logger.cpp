#include "flightState.h"
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

struct FlightSample {
    uint32_t time;
    flight_state state;
    double rawAltitude;
    double filteredAltitude;
    bool ejectionOutputActive;
};

constexpr int CACHE_SIZE = 200;

FlightSample cache[CACHE_SIZE];
File logFile;

int cacheHead = 0;
bool cacheFull = false;

long lastFlush = 0;

// TODO: find correct pin
#define SD_CS_PIN 17

void writeSample(const FlightSample& sample);
void flushCache();

static const char* flightStateName(flight_state state)
{
    switch (state) {
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

bool initLogger()
{
    SPI.setRX(16);
    SPI.setCS(SD_CS_PIN);
    SPI.setSCK(18);
    SPI.setTX(19);
    SPI.begin();
    if(!SD.begin(SD_CS_PIN)) {
        Serial.println("SD initialization FAILED");
        return false;
    } else {
        Serial.println("SD initialization SUCCESS.");
    }

    int fileNum = 0;
    String fileName;

    do
    {
        fileName = "flight" + String(fileNum) + ".csv";
        fileNum++;
    } while (SD.exists(fileName));

    logFile = SD.open(fileName, FILE_WRITE);

    if (logFile) {
        logFile.println("time (ms),raw altitude (m),filtered altitude (m),ejection output,flight state");
    }
    return true;
}

void loggerPeriodic(double rawAltitudeMeters,
                    double filteredAltitudeMeters,
                    bool ejectionOutputActive,
                    flight_state state)
{
    switch (state)
    {
    case IDLE:
    {
        FlightSample sample;
        sample.rawAltitude = rawAltitudeMeters;
        sample.filteredAltitude = filteredAltitudeMeters;
        sample.ejectionOutputActive = ejectionOutputActive;
        sample.state = state;
        sample.time = millis();

        cache[cacheHead] = sample;

        cacheHead = (cacheHead + 1) % CACHE_SIZE;

        if (cacheHead == 0)
            cacheFull = true;
        break;
    }
    case LAUNCH:
        flushCache();
        break;
    case LOGGINGUP:
    case LOGGINGDOWN:
        logFile.print(millis());
        logFile.print(",");
        logFile.print(rawAltitudeMeters);
        logFile.print(",");
        logFile.print(filteredAltitudeMeters);
        logFile.print(",");
        logFile.print(ejectionOutputActive ? "HIGH" : "LOW");
        logFile.print(",");
        logFile.println(flightStateName(state));

        // Flush occasionally
        if (millis() - lastFlush > 1000)
        {
            logFile.flush();
            lastFlush = millis();
        }
        break;
    case LANDING:
    case FINISH:
        break;
    }
}

void writeSample(const FlightSample& sample) {
    logFile.print(sample.time);
    logFile.print(",");
    logFile.print(sample.rawAltitude);
    logFile.print(",");
    logFile.print(sample.filteredAltitude);
    logFile.print(",");
    logFile.print(sample.ejectionOutputActive ? "HIGH" : "LOW");
    logFile.print(",");
    logFile.println(flightStateName(sample.state));
}

void flushCache() {
    if (cacheFull) {
        for (int i = cacheHead; i < CACHE_SIZE; i++)
            writeSample(cache[i]);
    }

    for (int i = 0; i < cacheHead; i++)
        writeSample(cache[i]);
}

void endFlight()
{
    logFile.close();
}
