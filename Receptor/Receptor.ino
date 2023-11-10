/*
Recebe dados do sistema do Alex na 'serial 0' e redireciona para a 'serial 1'
*/
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("inicado!");
}

void loop() {
}

void serialEvent() {
  while (Serial.available()) {
    Serial.print(".");
    char inChar = (char)Serial.read();
    Serial1.print(inChar);
  }
}
