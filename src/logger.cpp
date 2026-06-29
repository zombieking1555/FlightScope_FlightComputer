#include "flightState.h"
#include <SD.h>
#include <Adafruit_LSM6DSO32.h>

constexpr int CACHE_SIZE = 200;

FlightSample cache[CACHE_SIZE];

File logFile;

int cacheHead = 0;
bool cacheFull = false;

struct FlightSample {
    uint32_t time;

    float ax;
    float ay;
    float az;

    float gx;
    float gy;
    float gz;

    float altitude;
};

long lastFlush = millis();
// TODO: find correct pin
#define SD_CS_PIN 10

void initLogger()
{
    SD.begin(SD_CS_PIN);

    int fileNum = 0;
    String fileName;

    do
    {
        fileName = "flight" + String(fileNum) + ".csv";
        fileNum++;
    } while (SD.exists(fileName));

    logFile = SD.open(fileName, FILE_WRITE);

    // Write CSV header
    logFile.println("time (ms),acceleration x (m/s^2),acceleration y (m/s^2),acceleration z (m/s^2),gyro x (rad),gyro y (rad),gyro z (rad),altitude (m)");
}

void loggerPeriodic(double altitudeMeters, sensors_event_t gyro, sensors_event_t accel, flight_state state)
{

    switch (state)
    {
    case IDLE:
        FlightSample sample;
        sample.altitude = altitudeMeters;
        sample.ax = accel.acceleration.x;
        sample.ay = accel.acceleration.y;
        sample.az = accel.acceleration.z;
        sample.gx = gyro.gyro.x;
        sample.gy = gyro.gyro.y;
        sample.gz = gyro.gyro.z;
        sample.time = millis();

         cache[cacheHead] = sample;

        cacheHead = (cacheHead + 1) % CACHE_SIZE;

        if (cacheHead == 0)
            cacheFull = true;
        break;
    case LAUNCH:
        flushCache();
        break;
    case LOGGING:
        // Append one line
        logFile.print(millis());
        logFile.print(",");
        logFile.print(accel.acceleration.x);
        logFile.print(",");
        logFile.print(accel.acceleration.y);
        logFile.print(",");
        logFile.print(accel.acceleration.z);
        logFile.print(",");
        logFile.print(gyro.gyro.x);
        logFile.print(",");
        logFile.print(gyro.gyro.y);
        logFile.print(",");
        logFile.print(gyro.gyro.z);
        logFile.print(",");
        logFile.println(altitudeMeters);

        // Flush occasionally
        if (millis() - lastFlush > 1000)
        {
            logFile.flush();
            lastFlush = millis();
        }
        break;
    }
}

void flushCache() {

    if (cacheFull) {
        for (int i = cacheHead; i < CACHE_SIZE; i++)
            writeSample(cache[i]);
    }

    for (int i = 0; i < cacheHead; i++)
        writeSample(cache[i]);
}

void writeSample(const FlightSample& sample) {
    logFile.print(sample.time);
    logFile.print(",");
    logFile.print(sample.ax);
    logFile.print(",");
    logFile.print(sample.ay);
    logFile.print(",");
    logFile.print(sample.az);
    logFile.print(",");
    logFile.print(sample.gx);
    logFile.print(",");
    logFile.print(sample.gy);
    logFile.print(",");
    logFile.print(sample.gz);
    logFile.print(",");
    logFile.print(sample.altitude);
}

void endFlight()
{
    logFile.close();
}