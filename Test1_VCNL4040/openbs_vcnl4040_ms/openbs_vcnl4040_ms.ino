#include <Wire.h>
#include "Sensors.hpp"
#include "libreriaSleep.hpp"

// =========================
// MODO PRUEBA
// 1 = NO duerme, imprime continuo por Serial
// 0 = comportamiento normal, duerme al final
// =========================
#define TEST_NO_SLEEP 1

// De la placa civil
#define EN_LDO_1 18
#define EN_LDO_2 16
#define EN_LDO_3 14

#define DIP_1 3
#define DIP_2 5
#define DIP_3 1

#define I2C_SDA 33
#define I2C_SCL 35

// Para la comunicación
#define UART2_TX 21
#define UART2_RX 17

// Clase de sensores
Sensors sensors(ADDRESS_HIGH);   // Si no detecta el MS5803, prueba ADDRESS_LOW

// Variables de lectura
uint16_t ambientLight = 0;
uint16_t backscatterLight = 0;
float pressure_abs = 0.0f;
float water_temp = 0.0f;

bool vcnl_ok = false;
bool ms_ok = false;

unsigned long lastPrint = 0;
const unsigned long PRINT_PERIOD_MS = 1000;

void readAndSendOnce() {
  sensors.getReadings(
    ambientLight,
    backscatterLight,
    pressure_abs,
    water_temp
  );

  Serial.println("=== LECTURAS ===");
  Serial.print("VCNL4040 init: ");
  Serial.println(vcnl_ok ? "OK" : "ERROR");

  Serial.print("MS5803 init: ");
  Serial.println(ms_ok ? "OK" : "ERROR");

  Serial.print("Ambient light (lux aprox): ");
  Serial.println(ambientLight);

  Serial.print("Backscatter / proximity: ");
  Serial.println(backscatterLight);

  Serial.print("Pressure abs (mbar): ");
  Serial.println(pressure_abs, 3);

  Serial.print("Water temp (C): ");
  Serial.println(water_temp, 3);

  // CSV simple por UART2
  Serial1.print(ambientLight);
  Serial1.print(",");
  Serial1.print(backscatterLight);
  Serial1.print(",");
  Serial1.print(pressure_abs, 3);
  Serial1.print(",");
  Serial1.println(water_temp, 3);

  Serial.println("----------------------------");
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);

  pinMode(39, INPUT_PULLUP);
  if (digitalRead(39) == LOW) {
    delay(3000);
  }

  Wire.begin(I2C_SDA, I2C_SCL);
  delay(30);

  sensors.setMeasurementFlags(0b00001111);
  sensors.begin(vcnl_ok, ms_ok, Wire);

#if TEST_NO_SLEEP
  // Dar tiempo a que el puerto USB CDC quede estable
  delay(2000);
  Serial.println();
  Serial.println("MODO PRUEBA: sin sleep");
#else
  readAndSendOnce();
  Serial.flush();   // vacía el buffer serie antes de dormir
  delay(100);
  sleepESP(600);
#endif
}

void loop() {
#if TEST_NO_SLEEP
  if (millis() - lastPrint >= PRINT_PERIOD_MS) {
    lastPrint = millis();
    readAndSendOnce();
  }
#else
  // Nada, el micro duerme al final de setup()
#endif
}

/*
void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("ESP32-S2 vivo, sin sleep");
}

void loop() {
  Serial.println("heartbeat");
  delay(1000);
}
*/
