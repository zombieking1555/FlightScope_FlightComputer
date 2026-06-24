#ifndef IMU_H
#define IMU_H
#include <Adafruit_LSM6DSO32.h>

void initIMU(
    lsm6dso32_accel_range_t accel_range, 
    lsm6ds_gyro_range_t gyro_range, 
    lsm6ds_data_rate_t accelRate, 
    lsm6ds_data_rate_t gyroRate);

void periodic();

sensors_event_t getGyroEvent();

sensors_event_t getAccelEvent();

sensors_event_t getTempEvent();

#endif

