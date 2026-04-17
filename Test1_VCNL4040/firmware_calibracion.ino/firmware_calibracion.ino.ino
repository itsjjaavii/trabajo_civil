// Firmware de calibracion 
// Procedimiento:
//   1. sumergir el sensor en agua limpia (0 NTU), espera 30s, luego presiona enter
//   2. sube a 100 NTU, espera 30s, presiona enter  (repetir para 250, 500, 1000)
//   3. copia el output del monitor serial a un archivo .TXT
//   4. Corre calibrateOOBS_adaptado.m con ese archivo

#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <Adafruit_VCNL4040.h>

#define SDA 33
#define SCL 35
#define LED_BUILT_IN 15

MS5803            sensorPresion(ADDRESS_HIGH);
Adafruit_VCNL4040 sensorTurbidez;

bool presionOK  = false;
bool turbidezOK = false;
uint32_t sampleIndex = 0;

void setup()
{
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  pinMode(LED_BUILT_IN, OUTPUT);

  delay(500); // Espera a que abran el monitor serial

  // Encabezado del CSV - nombres compatibles con el script .m
  // columnas: index, time, millis, backscatter, ambient_light, pressure, water_temp
  Serial.println("OpenOBS SN:001");  // El script busca esta linea para el numero de serie
  Serial.println("index,time,millis,backscatter,ambient_light,pressure,water_temp");

  // Inicializar MS5803
  sensorPresion.reset();
  delay(100);
  presionOK = (sensorPresion.begin() == 0);
  if (!presionOK) Serial.println("# ADVERTENCIA: MS5803 no encontrado");

  // Inicializar VCNL4040
  turbidezOK = sensorTurbidez.begin();
  if (turbidezOK) {
    sensorTurbidez.setProximityIntegrationTime(VCNL4040_PS_IT_8T);
    sensorTurbidez.setLEDCurrent(VCNL4040_LED_CURRENT_200MA);
    sensorTurbidez.setAmbientIntegrationTime(VCNL4040_ALS_IT_80MS);
  } else {
    Serial.println("# ADVERTENCIA: VCNL4040 no encontrado");
  }

  if (!presionOK || !turbidezOK) {
    Serial.println("# ERROR: Revisa el cableado. Detenido.");
    while (true);
  }

  Serial.println("# Sensores OK. Iniciando mediciones continuas...");
  Serial.println("# Sumerge en cada estandar y espera ~30s antes de cambiar al siguiente.");
  digitalWrite(LED_BUILT_IN, HIGH);
}

void loop()
{
  // Leer VCNL4040
  uint16_t backscatter   = sensorTurbidez.getProximity();
  uint16_t ambientLight  = (uint16_t)(sensorTurbidez.getLux() * 10.0f);

  // Leer MS5803
  // getPressure y getTemperature comparten la misma conversion interna,
  // hay que llamar a getPressure primero (hace la lectura ADC completa)
  double pressure_mbar = sensorPresion.getPressure(ADC_4096);
  double temp_c        = sensorPresion.getTemperature(CELSIUS, ADC_512);

  // Escalar para almacenar como entero (igual que el OpenOBS-328)
  uint32_t abs_P     = (uint32_t)(pressure_mbar * 100.0); // 0.01 mbar por unidad
  int16_t  water_temp = (int16_t)(temp_c * 100.0);        // 0.01 C por unidad

  // Timestamp en segundos desde arranque (no tenemos RTC, usamos millis)
  uint32_t t_ms  = millis();
  uint32_t t_s   = t_ms / 1000;

  // Imprimir CSV
  // Las lineas que empiezan con '#' son ignoradas por el script .m
  Serial.print(sampleIndex); Serial.print(",");
  Serial.print(t_s);         Serial.print(",");
  Serial.print(t_ms);        Serial.print(",");
  Serial.print(backscatter); Serial.print(",");
  Serial.print(ambientLight);Serial.print(",");
  Serial.print(abs_P);       Serial.print(",");
  Serial.println(water_temp);

  sampleIndex++;

  // LED parpadea con cada lectura para confirmar actividad
  digitalWrite(LED_BUILT_IN, sampleIndex % 2);

  delay(500); // 2 lecturas por segundo - suficiente para calibracion
}
