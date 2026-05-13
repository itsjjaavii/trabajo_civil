// placa RECEPTORA -> cargar, conectar el usb-c y ver el monitor serial
#define LED_PIN   15

#define RS485_RX  17   // Rx_RS485
#define RS485_TX  21   // Tx_RS485
#define RS485_EN  38   // GPIO_RS485_EN

HardwareSerial RS485Serial(1);

void rs485ReceiveMode() {
  digitalWrite(RS485_EN, LOW);   // modo recibir
  delayMicroseconds(50);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RS485_EN, OUTPUT);

  rs485ReceiveMode();

  Serial.begin(115200);
  delay(2000);

  RS485Serial.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX);

  Serial.println("PLACA RECEPTORA RS485 iniciada");
  Serial.println("Esperando datos...");
}

void loop() {
  if (RS485Serial.available()) {
    String msg = RS485Serial.readStringUntil('\n');
    msg.trim();

    if (msg.length() > 0) {
      Serial.print("Recibido por RS485: ");
      Serial.println(msg);

      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    }
  }
}
