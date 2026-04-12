//Librerias
#include <Wire.h> //Para I2C
#include <SparkFun_MS5803_I2C.h>  //Para el sensor
#include "libreriaSleep.hpp"

//De la placa civil
#define EN_LDO_1 18
#define EN_LDO_2 16
#define EN_LDO_3 14

#define DIP_1 3 
#define DIP_2 5
#define DIP_3 1
#define LED_BUILT_IN 15

#define SDA 33
#define SCL 35

//Para la comunicacion

#define UART2_TX 21   // Cambia por el pin que uses
#define UART2_RX 17   // Cambia por el pin que uses

//Del sensor

MS5803 sensor(ADDRESS_HIGH); //Evaluo en direccion por defecto

double pressure_abs;

void setup()
{
  Wire.begin(33,35);
  pinMode(39,INPUT_PULLUP);
  if(digitalRead(39)==LOW) //PAra debug, lo conecté a vcc para ver las lecturas
  {
    delay(3000);
  }
  // Inicializaciones 
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);
  pinMode(LED_BUILT_IN, OUTPUT);
  digitalWrite(LED_BUILT_IN,1);

  delay(30); //Espera antes de inicializar, calibrado con el del transmisor

  Wire.begin(33,35);
  delay(30);
  sensor.reset();
  if(sensor.begin() == 0)
  {
    pressure_abs = sensor.getPressure(ADC_4096); //Primera lectura, define presion inicial
  }
  else
  {
    pressure_abs = -2; //Significa que no pude inicializar el sensor
  }

  //Muestro lectura en monitor serial
  Serial.print("Pressure abs (mbar)= ");
  Serial.println(pressure_abs);

  //Envio la lectura
  //Serial1.println(pressure_abs);
  Serial1.println(pressure_abs);
  digitalWrite(LED_BUILT_IN,0);
  sleepESP(600); //No es realmente importante, la alimentacion s econtrola desde afuera, pero ahorraria un poco de bateria 
}

void loop()
{
  //Nada, voy a dormir
  /*pressure_abs = sensor.getPressure(ADC_4096); //Primera lectura, define presion inicial
  Serial.print("Pressure abs (mbar)= ");
  Serial.println(pressure_abs);

  //Envio la lectura
  //Serial1.println(pressure_abs);
  Serial1.println(pressure_abs);
  digitalWrite(LED_BUILT_IN,0);
  delay(250);
  //sleepESP(600); //No es realmente importante, la alimentacion s econtrola desde afuera, pero ahorraria un poco de bateria 
  */
}


