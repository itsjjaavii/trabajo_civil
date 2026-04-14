#include <Wire.h>
#include <EEPROM.h>
#include "Sensors.h"

// Selecciona la variante del MS5803 (1, 2, 5, 14, 30 bar)
static const uint8_t MS5803_VERSION = 14;

// Intervalo entre mediciones (0 = sin delay, medir continuamente)
long sleepDuration_ms = 5000;

// Etiquetas de columnas para el monitor serial
const char dataColumnLabels[] = "millis,ambient_light_x10,backscatter,pressure_x100,water_temp_x100";

// Estructura de datos por medicion
struct {
    uint32_t timeMillis;
    uint16_t ambientLight;      // getLux() * 10
    uint16_t backscatterLight;  // getProximity() raw
    uint32_t absPressure;       // mbar * 100
    int16_t  waterTemp;         // Celsius * 100

    void toString(char* buf) {
        sprintf(buf, "%lu,%u,%u,%lu,%i",
                timeMillis, ambientLight, backscatterLight,
                absPressure, waterTemp);
    }
} data;

// Flags de que medir: bit0=amb, bit1=backscatter, bit2=presion, bit3=temp
uint8_t measFlags   = 0b00001111;
uint8_t eepromAddr  = 4;

Sensors sensors(eepromAddr, MS5803_VERSION);

char messageBuffer[80];

// Estado de inicializacion
union {
    byte b;
    struct {
        bool turb : 1;
        bool pt   : 1;
    } module;
} startup;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    delay(200); // Espera a que los sensores arranquen

    bool vcnl_ok, ms5803_ok;
    sensors.setMeasurementFlags(measFlags);
    sensors.begin(vcnl_ok, ms5803_ok);

    startup.module.turb = vcnl_ok;
    startup.module.pt   = ms5803_ok;

    if (!vcnl_ok) {
        Serial.println("ERROR: VCNL4040 no encontrado. Revisa I2C.");
    }
    if (!ms5803_ok) {
        Serial.println("ERROR: MS5803 no encontrado. Revisa I2C.");
    }

    if (startup.b != 0b00000011) {
        Serial.println("Fallo en inicializacion. Detenido.");
        while (true); // Detiene si algun sensor fallo
    }

    Serial.println("Sensores OK.");
    Serial.println(dataColumnLabels);
}

void loop()
{
    data.timeMillis = millis();

    sensors.getReadings(
        data.ambientLight,
        data.backscatterLight,
        data.absPressure,
        data.waterTemp
    );

    data.toString(messageBuffer);
    Serial.println(messageBuffer);

    if (sleepDuration_ms > 0) {
        delay(sleepDuration_ms);
    }
}
