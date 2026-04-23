#include <Wire.h>
#include <Adafruit_VCNL4040.h>

Adafruit_VCNL4040 vcnl;

void setup() {
  Serial.begin(115200);
  Wire.begin();   // Uno: SDA=A4, SCL=A5

  Serial.println("Iniciando VCNL4040...");

  if (!vcnl.begin()) {
    Serial.println("ERROR: No se encontro el VCNL4040. Revisa el cableado.");
    while (1);
  }

  Serial.println("VCNL4040 encontrado!");

  vcnl.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
  vcnl.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
  vcnl.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);

  Serial.println("Configuracion lista.");
  Serial.println("----------------------------");
}

void loop() {
  uint16_t proximity = vcnl.getProximity();
  uint16_t lux = vcnl.getLux();

  Serial.print("Proximidad: ");
  Serial.print(proximity);
  Serial.print(" | Lux: ");
  Serial.println(lux);

  delay(200);
}
