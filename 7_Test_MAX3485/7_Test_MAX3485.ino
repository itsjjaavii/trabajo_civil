// UART0: Serial (USB)
// UART2: Serial2 (pines configurables)

#define UART2_TX 17   // Cambia por el pin que uses
#define UART2_RX 16   // Cambia por el pin que uses

int i = 0;

void setup() {
  // Inicializa UART0 (monitor serie por USB)
  Serial.begin(115200); //Usa por defecot los del UART0, siendo GPIO1 y 3 respectivamente
  Serial2.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX); // Inicializa UART1 con pines definidos
}

void loop() {
  i++;
  Serial2.println("Mensaje "); // De UART2 a UART20 
  Serial2.println(i);
  Serial2.println("De UART 2 a UART 0, enviado por RS485");
  while (Serial.available()) 
  {
    Serial.write(Serial.read());
  }
  delay(1000); //Detalle: No se puede programar el ESP32 con los modulos alimentados, supongo que porque comparte los pines con el de programacion, da conflicto

  // De UART0 a UART2
  /*while (Serial1.available()) 
  {
    Serial.write(Serial1.read());
  }*/
}
