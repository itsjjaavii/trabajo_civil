// Librerias
#include <Wire.h>                   // I2C
#include <SparkFun_MS5803_I2C.h>    // Sensor de presion
#include <Adafruit_VCNL4040.h>      // Sensor de turbidez
#include "libreriaSleep.hpp"

#define SDA_I2C 33
#define SCL_I2C 35

// Para la comunicacion RS485
#define UART_TX_RS485 21
#define UART_RX_RS485 17

// Debug
#define DEBUG_PIN 39

// -------- Sensores --------

// MS5803
MS5803 pressureSensor(ADDRESS_HIGH);
double pressure_abs;

// VCNL4040
Adafruit_VCNL4040 turbiditySensor;

bool pressure_ok = false;
bool turbidity_ok = false;

uint16_t prox_raw  = 0;
uint16_t als_raw   = 0;
uint16_t white_raw = 0;
float lux_val      = 0.0;

void setup()
{
  // Bus I2C comun para ambos sensores
  Wire.begin(SDA_I2C, SCL_I2C);

  // Debug: si este pin esta en LOW, espera 3 s
  pinMode(DEBUG_PIN, INPUT_PULLUP);
  if (digitalRead(DEBUG_PIN) == LOW)
  {
    delay(3000);
  }

  // Serial USB y serial hacia RS485
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART_RX_RS485, UART_TX_RS485);

  // Si este pin realmente habilita algo externo, lo activamos
  pinMode(EN_EQUIPO_EXTERNO, OUTPUT);
  digitalWrite(EN_EQUIPO_EXTERNO, HIGH);

  delay(50);  // tiempo de estabilizacion

  // =========================================================
  // 1) Inicializar MS5803
  // =========================================================
  pressureSensor.reset();
  if (pressureSensor.begin() == 0)
  {
    pressure_ok = true;
    pressure_abs = pressureSensor.getPressure(ADC_4096);
  }
  else
  {
    pressure_ok = false;
    pressure_abs = -2;   // codigo de error heredado
  }

  // =========================================================
  // 2) Inicializar VCNL4040
  // =========================================================
  turbidity_ok = turbiditySensor.begin();

  if (turbidity_ok)
  {
    // Configuracion inicial simple
    turbiditySensor.setProximityHighResolution(true);
    turbiditySensor.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
    turbiditySensor.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_640MS);
    turbiditySensor.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
    turbiditySensor.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);

    delay(100);

    prox_raw  = turbiditySensor.getProximity();
    als_raw   = turbiditySensor.getAmbientLight();
    white_raw = turbiditySensor.getWhiteLight();
    lux_val   = turbiditySensor.getLux();
  }
  else
  {
    prox_raw  = 0xFFFF;
    als_raw   = 0xFFFF;
    white_raw = 0xFFFF;
    lux_val   = -1.0;
  }

  // =========================================================
  // Debug por USB
  // =========================================================
  Serial.println("===== LECTURAS =====");

  Serial.print("MS5803 pressure_abs (mbar)= ");
  Serial.println(pressure_abs);

  if (turbidity_ok)
  {
    Serial.println("VCNL4040 OK");
    Serial.print("PROX  = ");
    Serial.println(prox_raw);
    Serial.print("ALS   = ");
    Serial.println(als_raw);
    Serial.print("WHITE = ");
    Serial.println(white_raw);
    Serial.print("LUX   = ");
    Serial.println(lux_val);
  }
  else
  {
    Serial.println("VCNL4040 ERROR");
  }

  // =========================================================
  // Envio por Serial1 / RS485
  // =========================================================
  Serial1.print("PRES:");
  Serial1.println(pressure_abs);

  if (turbidity_ok)
  {
    Serial1.print("PROX:");
    Serial1.println(prox_raw);

    Serial1.print("ALS:");
    Serial1.println(als_raw);

    Serial1.print("WHITE:");
    Serial1.println(white_raw);

    Serial1.print("LUX:");
    Serial1.println(lux_val);
  }
  else
  {
    Serial1.println("VCNL4040_ERROR");
  }

  delay(50);

  // Apaga alimentacion externa si corresponde
  digitalWrite(EN_EQUIPO_EXTERNO, LOW);

  // Dormir
  sleepESP(600);
}

void loop()
{
  // Nada: al despertar vuelve a setup()
}
