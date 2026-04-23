#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_VCNL4040.h>
#include <SparkFun_MS5803_I2C.h>

// Equivalente a vcnl4010_freq pero para el VCNL4040:
// tiempo de integracion del LED de proximidad
// Valores validos de la libreria Adafruit:
//   VCNL4040_PS_IT_1T, _2T, _4T, _8T
typedef vcnl4040ProximityIntegration vcnl4040_ps_it;

// Configuracion persistente del sensor
struct SensorSettings {
    vcnl4040_ps_it  proximityIntTime  = VCNL4040_PS_IT_8T;       // Tiempo integracion proximidad
    vcnl4040LEDCurrent ledCurrent     = VCNL4040_LED_CURRENT_200MA; // Corriente LED IR
    vcnl4040AmbientIntegration alsIT  = VCNL4040_ALS_IT_80MS;    // Integracion luz ambiente
    uint8_t measFlags                 = 0b00001111;               // Bits: amb | back | pres | temp
};

class Sensors {
public:
    Sensors(uint16_t eepromAddr, uint8_t ms5803_version);

    uint16_t      eepromAddress;
    bool          hasPressureSensor;
    SensorSettings settings;

    Adafruit_VCNL4040 vcnl4040;
    MS5803            ms5803;

    void saveSettings();
    void loadSettings();

    void setProximityIntTime(vcnl4040_ps_it intTime);
    void setLEDCurrent(vcnl4040LEDCurrent current);
    void setAmbientIntTime(vcnl4040AmbientIntegration alsTime);
    void setMeasurementFlags(uint8_t flags);

    void begin(bool &vcnl_init, bool &ms5803_init);
    void getReadings(uint16_t &tuAmbient, uint16_t &tuBackscatter,
                     uint32_t &abs_P,     int16_t  &water_temp);
};

#endif // SENSORS_H
