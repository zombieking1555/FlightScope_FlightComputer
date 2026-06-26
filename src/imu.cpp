#include <Adafruit_LSM6DSO32.h>

Adafruit_LSM6DSO32 dso32;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

void initIMU(lsm6dso32_accel_range_t accel_range, lsm6ds_gyro_range_t gyro_range, lsm6ds_data_rate_t accelRate, lsm6ds_data_rate_t gyroRate){  
    Serial.println("Adafruit LSM6DSO32 Test\n");
    
    if(!dso32.begin_I2C()){
        while (1)
        {
            delay(10);
        }   
    }

    Serial.println("LSM6DO32 Found\n");

    dso32.setAccelRange(accel_range);
    Serial.print("LSM6DSO32 Accelerometer range set to: ");
    switch (dso32.getAccelRange()){
        case LSM6DSO32_ACCEL_RANGE_4_G:
        Serial.println("4G\n");
            break;
        case LSM6DSO32_ACCEL_RANGE_8_G:
        Serial.println("8G\n");
            break;
        case LSM6DSO32_ACCEL_RANGE_16_G:
        Serial.println("16G\n");
            break;
        case LSM6DSO32_ACCEL_RANGE_32_G:
            Serial.println("32G\n");
            break;
        default:
            dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
            Serial.println("OVERRIDE: Range undetected. Automatically set to " + dso32.getAccelRange());
    }

    dso32.setAccelDataRate(accelRate);
    Serial.print("Accelerometer data rate set to: ");
  switch (dso32.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  default:
    dso32.setAccelDataRate(LSM6DS_RATE_52_HZ);
    Serial.print("OVERRIDE: Undetected rate, automatically set to " + dso32.getAccelDataRate());
  break;
  }

    dso32.setGyroRange(gyro_range);
    Serial.print("LSM6DO32 Gyro range set to: ");
    switch (dso32.getGyroRange())
    {
    case LSM6DS_GYRO_RANGE_125_DPS:
        Serial.print("125 DPS");
        break;
    case LSM6DS_GYRO_RANGE_250_DPS:
        Serial.print("250 DPS");
        break;
    case LSM6DS_GYRO_RANGE_500_DPS:
        Serial.print("500 DPS");
        break;
    case LSM6DS_GYRO_RANGE_1000_DPS:
        Serial.print("1000 DPS");
        break;
    case LSM6DS_GYRO_RANGE_2000_DPS:
        Serial.print("2000DPS");
        break;
    default:
        dso32.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
        Serial.print("OVERRIDE: range undetected, auto set to " + dso32.getGyroRange());
        break;
    }

    dso32.setGyroDataRate(gyroRate);
    Serial.print("Gyro data rate set to: ");
  switch (dso32.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  default:
    dso32.setGyroDataRate(LSM6DS_RATE_52_HZ);
    Serial.println("OVERRIDE: rate undetected, auto set to " + dso32.getGyroDataRate());
  }

  Serial.println("============================================");
  Serial.println("IMU Successfully initialized.");
  Serial.println("============================================");
}

void imuPeriodic(){
    dso32.getEvent(&accel, &gyro, &temp);
}

sensors_event_t getGyroEvent(){
    return gyro;
}

sensors_event_t getAccelEvent(){
    return accel;
}

sensors_event_t getTempEvent(){
    return temp;
}