/*
Recebe dados do sistema do na 'serial 1' e redireciona para a 'serial 0' para imprimir no computador
*/
String msg = "";
long time = millis();
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("inicado!");
}

void loop() {
  if ((millis() - time >= 1000) && msg != "") {
    Serial.print("Mensagem:");
    Serial.println(msg);
    msg = "";
    time = millis();
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    analisar(Serial1.read());
    time = millis();
  }
}

void analisar(int thisChar) {
  msg += (char)thisChar;
  Serial.print("Recebido: \'");
  Serial.write(thisChar);
  Serial.print("\'  ASCII Value: ");
  Serial.print(thisChar);
  Serial.print(" | ");

  // analyze what was sent:
  if (isAlphaNumeric(thisChar)) {
    Serial.print("it's alphanumeric | ");
  }
  if (isAlpha(thisChar)) {
    Serial.print("it's alphabetic | ");
  }
  if (isAscii(thisChar)) {
    Serial.print("it's ASCII | ");
  }
  if (isWhitespace(thisChar)) {
    Serial.print("it's whitespace | ");
  }
  if (isControl(thisChar)) {
    Serial.print("it's a control character | ");
  }
  if (isDigit(thisChar)) {
    Serial.print("it's a numeric digit | ");
  }
  if (isGraph(thisChar)) {
    Serial.print("it's a printable character that's not whitespace | ");
  }
  if (isLowerCase(thisChar)) {
    Serial.print("it's lower case | ");
  }
  if (isPrintable(thisChar)) {
    Serial.print("it's printable | ");
  }
  if (isPunct(thisChar)) {
    Serial.print("it's punctuation | ");
  }
  if (isSpace(thisChar)) {
    Serial.print("it's a space character | ");
  }
  if (isUpperCase(thisChar)) {
    Serial.print("it's upper case | ");
  }
  if (isHexadecimalDigit(thisChar)) {
    Serial.print("it's a valid hexadecimaldigit (i.e. 0 - 9, a - F, or A - F) | ");
  }
  Serial.println("");
}
