// SDA A4
// SCL A5
// Los pines I2C de Arduino Uno trabajan a 5V, hacer divisor de tensión!!

#include <Wire.h>
#include <Adafruit_VCNL4040.h>

Adafruit_VCNL4040 vcnl;

void setup()
{
  Serial.begin(115200);
  Wire.begin(); // En Arduino Uno usa A4 (SDA) y A5 (SCL) por defecto

  Serial.println("Iniciando VCNL4040...");

  if (!vcnl.begin())
  {
    Serial.println("ERROR: No se encontro el VCNL4040. Revisa el cableado.");
    while (1); // Se queda aqui si no encuentra el sensor
  }

  Serial.println("VCNL4040 encontrado!");

  // Configuracion para mayor sensibilidad
  vcnl.setProximityIntegrationTime(VCNL4040_PS_IT_8T);    // Maximo tiempo de integracion
  vcnl.setLEDCurrent(VCNL4040_LED_CURRENT_200MA);          // Maxima corriente LED IR
  vcnl.setAmbientIntegrationTime(VCNL4040_ALS_IT_80MS);    // Integracion luz ambiente

  Serial.println("Configuracion lista.");
  Serial.println("----------------------------");
}

void loop()
{
  uint16_t proximity = vcnl.getProximity(); // 0 a 65535
  float    lux       = vcnl.getLux();       // Lux ambiente

  Serial.print("Proximidad : ");
  Serial.print(proximity);
  Serial.print("  |  Lux : ");
  Serial.println(lux);

  delay(200); // Lectura cada 200ms
}
