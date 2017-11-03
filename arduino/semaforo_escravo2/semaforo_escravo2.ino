#include <SoftwareSerial.h>

SoftwareSerial Serial4(10, 11); // RX, TX

int V = 48;
int Y = 50;
int G = 52;

char incomingByte;

#define ID '2'

void setup() {
  pinMode(V, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  digitalWrite(V, HIGH);
  digitalWrite(Y, HIGH);
  digitalWrite(G, HIGH);

  Serial.begin(9600);
  Serial4.begin(9600);
}
void loop() {
  if (Serial4.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial4.read();

    if (incomingByte == 'S') {
      Serial.println(ID);
      Serial4.print(ID);
    }
  }
}

