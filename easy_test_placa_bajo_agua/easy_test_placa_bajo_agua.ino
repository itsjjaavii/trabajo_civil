#define LED_PIN 15   // GPIO15 del ESP32-S2

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Prueba inicial
  digitalWrite(LED_PIN, HIGH);  // LED encendido
  delay(1000);
  digitalWrite(LED_PIN, LOW);   // LED apagado
  delay(1000);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // prende LED
  delay(500);

  digitalWrite(LED_PIN, LOW);   // apaga LED
  delay(500);
}
