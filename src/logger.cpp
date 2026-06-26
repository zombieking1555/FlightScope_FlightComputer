#include <SD.h>
#include <Adafruit_LSM6DSO32.h>

File logFile;

long lastFlush = millis();
//TODO: find correct pin
#define SD_CS_PIN 10

void initLogger() {
    SD.begin(SD_CS_PIN);

    int fileNum = 0;
    String fileName;

    do {
        fileName = "flight" + String(fileNum) + ".csv";
        fileNum++;
    } while (SD.exists(fileName));

    logFile = SD.open(fileName, FILE_WRITE);

    // Write CSV header
    logFile.println("time (ms),acceleration x (m/s^2),acceleration y (m/s^2),acceleration z (m/s^2),gyro x (rad),gyro y (rad),gyro z (rad),altitude (m)");
}

void loggerPeriodic(double altitudeMeters, sensors_event_t gyro, sensors_event_t accel) {
    // Read sensors

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
    if (millis() - lastFlush > 1000) {
        logFile.flush();
        lastFlush = millis();
    }
}

void endFlight() {
    logFile.close();
}