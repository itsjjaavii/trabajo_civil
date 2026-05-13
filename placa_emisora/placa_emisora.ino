// placa emisora -> se carga y se desconecta el usb-c
#define LED_PIN   15

#define RS485_RX  17   // Rx_RS485
#define RS485_TX  21   // Tx_RS485
#define RS485_EN  38   // GPIO_RS485_EN

HardwareSerial RS485Serial(1);

unsigned long lastSend = 0;
uint32_t counter = 0;

void rs485ReceiveMode() {
  digitalWrite(RS485_EN, LOW);   // /RE activo, DE desactivado
  delayMicroseconds(50);
}

void rs485TransmitMode() {
  digitalWrite(RS485_EN, HIGH);  // DE activo, /RE desactivado
  delayMicroseconds(50);
}

void rs485SendLine(const String &msg) {
  rs485TransmitMode();

  RS485Serial.println(msg);
  RS485Serial.flush();           // espera a que termine de salir por UART

  delayMicroseconds(300);
  rs485ReceiveMode();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RS485_EN, OUTPUT);

  rs485ReceiveMode();

  Serial.begin(115200);
  delay(1500);

  RS485Serial.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX);

  Serial.println("PLACA EMISORA RS485 iniciada");
}

void loop() {
  if (millis() - lastSend >= 1000) {
    lastSend = millis();

    String msg = "Hola desde placa EMISORA, contador = " + String(counter++);
    rs485SendLine(msg);

    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);

    Serial.print("Enviado: ");
    Serial.println(msg);
  }
}
