#include "Sensors.h"

Sensors::Sensors(uint16_t eepromAddr, uint8_t ms5803_version)
{
    eepromAddress = eepromAddr;

    if (ms5803_version > 0) {
        ms5803 = MS5803(ADDRESS_HIGH); // 0x77, igual que en tu firmware original
        hasPressureSensor = true;
    } else {
        hasPressureSensor = false;
    }
}

// ---- Setters (aplican en tiempo real y guardan en EEPROM) ----

void Sensors::setProximityIntTime(vcnl4040_ps_it intTime)
{
    settings.proximityIntTime = intTime;
    vcnl4040.setProximityIntegrationTime(intTime);
    saveSettings();
}

void Sensors::setLEDCurrent(vcnl4040LEDCurrent current)
{
    settings.ledCurrent = current;
    vcnl4040.setLEDCurrent(current);
    saveSettings();
}

void Sensors::setAmbientIntTime(vcnl4040AmbientIntegration alsTime)
{
    settings.alsIT = alsTime;
    vcnl4040.setAmbientIntegrationTime(alsTime);
    saveSettings();
}

void Sensors::setMeasurementFlags(uint8_t flags)
{
    settings.measFlags = flags;
    saveSettings();
}

// ---- EEPROM ----

void Sensors::loadSettings()
{
    EEPROM.get(eepromAddress, settings);
}

void Sensors::saveSettings()
{
    EEPROM.put(eepromAddress, settings);
}

// ---- Inicializacion ----

void Sensors::begin(bool &vcnl_init, bool &ms5803_init)
{
    vcnl_init = vcnl4040.begin();

    if (vcnl_init) {
        vcnl4040.setProximityIntegrationTime(settings.proximityIntTime);
        vcnl4040.setLEDCurrent(settings.ledCurrent);
        vcnl4040.setAmbientIntegrationTime(settings.alsIT);
    }

    if (hasPressureSensor) {
        ms5803.reset();
        ms5803_init = (ms5803.begin() == 0); // begin() retorna 0 si OK (SparkFun)
    } else {
        ms5803_init = true;
    }
}

// ---- Lectura ----

void Sensors::getReadings(uint16_t &tuAmbient, uint16_t &tuBackscatter,
                           uint32_t &abs_P,     int16_t  &water_temp)
{
    // Bit 0: luz ambiente (ALS)
    if (settings.measFlags & 0b00000001) {
        // getLux() retorna float; escalamos x10 para guardar como uint16 sin perder decimales
        tuAmbient = (uint16_t)(vcnl4040.getLux() * 10.0f);
    } else {
        tuAmbient = 0;
    }

    // Bit 1: backscatter / proximidad (turbidez)
    if (settings.measFlags & 0b00000010) {
        tuBackscatter = vcnl4040.getProximity(); // 0-65535
    } else {
        tuBackscatter = 0;
    }

    // Bits 2 y 3: presion y temperatura (MS5803)
    if (hasPressureSensor && ((settings.measFlags & 0b00001100) != 0)) {
        double pres = ms5803.getPressure(ADC_4096);
        double temp = ms5803.getTemperature(CELSIUS, ADC_4096);

        if (settings.measFlags & 0b00000100) {
            // Guardamos presion en unidades de 0.01 mbar para caber en uint32
            abs_P = (uint32_t)(pres * 100.0);
        } else {
            abs_P = 0;
        }

        if (settings.measFlags & 0b00001000) {
            // Guardamos temperatura en 0.01 °C para caber en int16
            water_temp = (int16_t)(temp * 100.0);
        } else {
            water_temp = -32768;
        }
    } else {
        abs_P      = 0;
        water_temp = -32768;
    }
}
