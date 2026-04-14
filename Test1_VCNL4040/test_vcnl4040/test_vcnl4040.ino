// Librerias
#include <Wire.h>                  // Para I2C
#include <SparkFun_MS5803_I2C.h>   // Para el sensor de presion
#include <Adafruit_VCNL4040.h>     // Para el sensor de turbidez
#include "libreriaSleep.hpp"

// De la placa civil
#define EN_LDO_1     18
#define EN_LDO_2     16
#define EN_LDO_3     14
#define DIP_1         3
#define DIP_2         5
#define DIP_3         1
#define LED_BUILT_IN 15
#define SDA          33
#define SCL          35

// Para la comunicacion
#define UART2_TX 21
#define UART2_RX 17

// ---- Sensores ----
MS5803            sensorPresion(ADDRESS_HIGH);  // MS5803 en 0x77
Adafruit_VCNL4040 sensorTurbidez;               // VCNL4040 en 0x60 

// ---- Variables globales ----
double   pressure_abs = 0.0;
uint16_t proximity    = 0;      // valor raw de proximidad 
float    lux          = 0.0;   // iluminancia ambiental

void setup()
{
  Wire.begin(SDA, SCL);

  // Modo debug: si el pin 39 esta en LOW, espera 3s para abrir el monitor serial
  pinMode(39, INPUT_PULLUP);
  if (digitalRead(39) == LOW)
  {
    delay(3000);
  }

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);

  pinMode(LED_BUILT_IN, OUTPUT);
  digitalWrite(LED_BUILT_IN, HIGH);

  delay(30); // Espera calibrada con el transmisor

  // ---- Inicializar MS5803 ----
  sensor.reset();
  if (sensor.begin() == 0)
  {
    pressure_abs = sensor.getPressure(ADC_4096);
  }
  else
  {
    pressure_abs = -2.0; // Indica fallo de inicializacion
  }

  // ---- Inicializar VCNL4040 ----
  //  tiempo integracion alto -> mayor sensibilidad
  //  LED current alto      -> mayor alcance optico
  bool vcnlOK = sensorTurbidez.begin();
  if (vcnlOK)
  {
    // Tiempo de integracion del sensor de proximidad: 8T (mayor precision)
    sensorTurbidez.setProximityIntegrationTime(VCNL4040_PS_IT_8T);

    // Corriente del LED IR: 200 mA (maximo, util bajo agua donde hay mas atenuacion)
    sensorTurbidez.setLEDCurrent(VCNL4040_LED_CURRENT_200MA);

    // Tiempo de integracion del sensor de luz ambiente: 80ms
    sensorTurbidez.setAmbientIntegrationTime(VCNL4040_ALS_IT_80MS);

    // Leer valores
    proximity = sensorTurbidez.getProximity();  // 0-65535, mayor valor = mas particulas
    lux       = sensorTurbidez.getLux();        // Iluminancia en lux
  }
  else
  {
    // Indica fallo de inicializacion del VCNL4040
    proximity = 0xFFFF;
    lux       = -1.0;
  }

  // ---- Mostrar en monitor serial ----
  Serial.print("Pressure abs (mbar) = ");
  Serial.println(pressure_abs);
  Serial.print("Proximity (turbidez) = ");
  Serial.println(proximity);
  Serial.print("Lux (ambiente) = ");
  Serial.println(lux);

  // ---- Enviar por UART ----
  // formato CSV: "presion,proximidad,lux\n"
  // ej "1013.25,4200,12.50"
  Serial1.print(pressure_abs);
  Serial1.print(",");
  Serial1.print(proximity);
  Serial1.print(",");
  Serial1.println(lux);

  digitalWrite(LED_BUILT_IN, LOW);
  sleepESP(600);
}

void loop()
{
  // Nada, el flujo ocurre en setup() antes de dormir
}
