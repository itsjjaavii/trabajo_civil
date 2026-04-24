// código de prueba para el MS5803 a través del ESP32S2 Mini 1
#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>

// Pines I2C que estabas usando en tu proyecto
#define I2C_SDA 33
#define I2C_SCL 35

// Prueba primero con ADDRESS_HIGH.
// Si no lo detecta, cambia a ADDRESS_LOW.
MS5803 sensor(ADDRESS_HIGH);

void scanI2C()
{
  Serial.println("Escaneando I2C...");
  byte encontrados = 0;

  for (byte addr = 1; addr < 127; addr++)
  {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("Encontrado dispositivo en 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      encontrados++;
      delay(1);
    }
  }

  if (encontrados == 0)
  {
    Serial.println("No se encontraron dispositivos I2C.");
  }

  Serial.println("----------------------------");
}

void setup()
{
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("=== Prueba MS5803 ===");

  Wire.begin(I2C_SDA, I2C_SCL);
  delay(50);

  scanI2C();

  uint8_t status = sensor.begin(Wire);

  if (status == 0)
  {
    Serial.println("MS5803 inicializado correctamente.");
  }
  else
  {
    Serial.print("Error al inicializar MS5803. Codigo: ");
    Serial.println(status);
    Serial.println("Prueba cambiando ADDRESS_HIGH por ADDRESS_LOW.");
    while (1)
    {
      delay(1000);
    }
  }
}

void loop()
{
  float pressure_mbar = sensor.getPressure(ADC_4096);
  float temp_c = sensor.getTemperature(CELSIUS, ADC_4096);

  Serial.print("Presion (mbar): ");
  Serial.print(pressure_mbar, 3);
  Serial.print(" | Temperatura (C): ");
  Serial.println(temp_c, 3);

  delay(1000);
}
