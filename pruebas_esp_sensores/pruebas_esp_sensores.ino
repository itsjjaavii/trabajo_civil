// Librerias
#include <Wire.h>                    // Para I2C
#include <SparkFun_MS5803_I2C.h>     // Para el sensor MS5803
#include <Adafruit_VCNL4040.h>       // Para el VCNL4040
#include "libreriaSleep.hpp"

#define I2C_SDA 33
#define I2C_SCL 35

// Para la comunicacion
#define UART2_TX 21
#define UART2_RX 17

// Sensores
MS5803 sensor(ADDRESS_HIGH);   // Si no aparece, probar con ADDRESS_LOW
Adafruit_VCNL4040 vcnl;

double pressure_abs = -2.0;

bool ms5803_ok = false;
bool vcnl4040_ok = false;

void scanI2C()
{
  Serial.println("Escaneando bus I2C...");
  uint8_t found = 0;

  for (uint8_t addr = 1; addr < 127; addr++)
  {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("Dispositivo I2C encontrado en 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
      delay(1);
    }
  }

  if (found == 0)
  {
    Serial.println("No se encontraron dispositivos I2C.");
  }

  Serial.println("----------------------------");
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);

  //pinMode(LED_BUILT_IN, OUTPUT);
  //digitalWrite(LED_BUILT_IN, 1);

  /* pinMode(39, INPUT_PULLUP);
  if (digitalRead(39) == LOW)   // Para debug, lo conectaste a VCC para ver las lecturas
  {
    delay(3000);
  } */

  // Inicializo una sola vez el bus I2C compartido
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(30);

  Serial.println();
  Serial.println("=== INICIO PRUEBA MS5803 + VCNL4040 ===");

  scanI2C();

  // -------- MS5803 --------
  uint8_t ms_status = sensor.begin(Wire);
  if (ms_status == 0)
  {
    ms5803_ok = true;
    pressure_abs = sensor.getPressure(ADC_4096);

    Serial.println("MS5803 inicializado correctamente.");
    Serial.print("Primera lectura de presion: ");
    Serial.println(pressure_abs, 3);
  }
  else
  {
    ms5803_ok = false;
    pressure_abs = -2.0;

    Serial.print("ERROR al inicializar MS5803. begin() devolvio: ");
    Serial.println(ms_status);
  }

  // -------- VCNL4040 --------
  if (vcnl.begin(0x60, &Wire))
  {
    vcnl4040_ok = true;

    // Configuracion para una prueba sensible
    vcnl.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
    vcnl.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
    vcnl.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);

    Serial.println("VCNL4040 inicializado correctamente.");
  }
  else
  {
    vcnl4040_ok = false;
    Serial.println("ERROR al inicializar VCNL4040.");
  }

  Serial.println("----------------------------");
  //digitalWrite(LED_BUILT_IN, 0);
}

void loop()
{
  static unsigned long lastRead = 0;

  if (millis() - lastRead >= 500)
  {
    lastRead = millis();

    uint16_t proximity  = 0;
    uint16_t lux        = 0;
    uint16_t whiteLight = 0;

    // Leo MS5803
    if (ms5803_ok)
    {
      pressure_abs = sensor.getPressure(ADC_4096);
    }

    // Leo VCNL4040
    if (vcnl4040_ok)
    {
      proximity  = vcnl.getProximity();
      lux        = vcnl.getLux();
      whiteLight = vcnl.getWhiteLight();
    }

    // Monitor serial
    Serial.print("MS5803 Pressure=");
    if (ms5803_ok) Serial.print(pressure_abs, 3);
    else           Serial.print("ERROR");

    Serial.print(" | VCNL4040 Prox=");
    if (vcnl4040_ok) Serial.print(proximity);
    else             Serial.print("ERROR");

    Serial.print(" | Lux=");
    if (vcnl4040_ok) Serial.print(lux);
    else             Serial.print("ERROR");

    Serial.print(" | White=");
    if (vcnl4040_ok) Serial.println(whiteLight);
    else             Serial.println("ERROR");

    // Envio simple por UART2 (CSV)
    if (ms5803_ok || vcnl4040_ok)
    {
      Serial1.print(pressure_abs, 3);
      Serial1.print(",");
      Serial1.print(proximity);
      Serial1.print(",");
      Serial1.print(lux);
      Serial1.print(",");
      Serial1.println(whiteLight);
    }

    //digitalWrite(LED_BUILT_IN, !digitalRead(LED_BUILT_IN));
  }

  // En esta version NO duermo el micro, para poder probar continuo.
  // Cuando confirmemos que ambos sensores estan OK, lo dejamos listo para sleep.
}
