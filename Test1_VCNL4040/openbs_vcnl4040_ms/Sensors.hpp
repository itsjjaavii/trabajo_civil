#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VCNL4040.h>
#include <SparkFun_MS5803_I2C.h>

// Igual que en los autores:
// bit 0 -> ambient
// bit 1 -> backscatter/proximity
// bit 2 -> pressure
// bit 3 -> water temp
struct SensorSettings {
    uint8_t measFlags = 0b00001111;
};

class Sensors {
public:
    Sensors(ms5803_addr pressureAddress = ADDRESS_HIGH);

    bool hasPressureSensor;
    SensorSettings settings;

    Adafruit_VCNL4040 vcnl4040;
    MS5803 ms5803;

    void setMeasurementFlags(uint8_t flags);
    void begin(bool &vcnl_init, bool &ms5803_init, TwoWire &wirePort = Wire);

    void getReadings(
        uint16_t &tuAmbient,
        uint16_t &tuBackscatter,
        float &abs_P,
        float &water_temp
    );
};

#endif
