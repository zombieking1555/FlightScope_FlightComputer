#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

void initAltimeter(){
    Serial.println("Adafruit BMP388 test");
    
    if(!bmp.begin_I2C()){
        Serial.println("Valid BMP3 Sensor Not Found.");
        while(1);
        
    }

    bool temp = bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bool pressure = bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bool iir = bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bool rate = bmp.setOutputDataRate(BMP3_ODR_50_HZ);
    
    if(temp && pressure && iir && rate){
    Serial.println("Altimeter configured successfully.");
    } else {
    Serial.println("Altimeter was not successfully configured.");
    }
}

/**************************************************************************/
/*!
    @brief Reads new values and assigns them to internal variables. Returns success status

    Reads altitude (m), atmospheric pressure (hPa), and altitude(m). Returns true on a successful read, and false otherwise.

    @return Success of value read
*/
/**************************************************************************/
bool altimeterPeriodic(){
    if(bmp.performReading()){
        return true;
    } else {
        Serial.print("BMP388 values read unsucessfully. Internally held values now stale.");
        return false;
    }
}

double getTempCelcius(){
    return bmp.temperature;
}

double getPressureHPA(){
    return bmp.pressure/100;
}

double getAltitudeMeters(){
    return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}