#include "Sensors.hpp"

Sensors::Sensors(ms5803_addr pressureAddress)
    : hasPressureSensor(true), ms5803(pressureAddress) {
}

void Sensors::setMeasurementFlags(uint8_t flags) {
    settings.measFlags = flags;
}

void Sensors::begin(bool &vcnl_init, bool &ms5803_init, TwoWire &wirePort) {
    // VCNL4040
    vcnl_init = vcnl4040.begin();

    if (vcnl_init) {
        // Configuración razonable para pruebas
        vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
        vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
        vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
    }

    // MS5803 SparkFun
    if (hasPressureSensor) {
        ms5803_init = (ms5803.begin(wirePort) == 0);
    } else {
        ms5803_init = true;
    }
}

void Sensors::getReadings(
    uint16_t &tuAmbient,
    uint16_t &tuBackscatter,
    float &abs_P,
    float &water_temp
) {
    // bit 0: ambient
    if (settings.measFlags & 0b00000001) {
        // Mapeo de "ambient" a lux del VCNL4040
        tuAmbient = (uint16_t)vcnl4040.getLux();
    } else {
        tuAmbient = 0;
    }

    // bit 1: backscatter
    if (settings.measFlags & 0b00000010) {
        // Mapeo de "backscatter" a proximidad
        tuBackscatter = vcnl4040.getProximity();
    } else {
        tuBackscatter = 0;
    }

    // bits 2 y 3: presión / temperatura
    if (hasPressureSensor && ((settings.measFlags & 0b00001100) != 0)) {

        if (settings.measFlags & 0b00000100) {
            abs_P = ms5803.getPressure(ADC_4096); // mbar
        } else {
            abs_P = 0.0f;
        }

        if (settings.measFlags & 0b00001000) {
            water_temp = ms5803.getTemperature(CELSIUS, ADC_4096);
        } else {
            water_temp = -999.0f;
        }
    } else {
        abs_P = 0.0f;
        water_temp = -999.0f;
    }
}
