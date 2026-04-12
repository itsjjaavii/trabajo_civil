// UART0: Serial (USB)
// UART2: Serial2 (pines configurables)

#define UART2_TX 21   // Cambia por el pin que uses
#define UART2_RX 17   // Cambia por el pin que uses

int i = 0;

void setup() 
{
  // Inicializa UART0 (monitor serie por USB)
  Serial.begin(115200); //Para escribir en pantalla
  Serial1.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX); // Inicializa UART1 con pines definidos
}

void loop()
{
  i++;
  Serial.println("Enviando mensaje");
  Serial1.println("Mensaje "); // De UART2 a UART20 
  Serial1.println(i);
  Serial1.println("Test, enviado por RS485");
  //while (Serial2.available()) 
  //{
  //  Serial.write(Serial2.read());
  //}
  delay(1000);
}
