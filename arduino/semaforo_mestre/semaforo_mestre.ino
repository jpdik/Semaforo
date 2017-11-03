#include <SoftwareSerial.h>

SoftwareSerial Serial4(10, 11); // RX, TX

int V = 48;
int Y = 50;
int G = 52;

char incomingByte;

#define ID '1'

char json[1024];

void setup() {
  pinMode(V, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  Serial4.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}
void loop() {
  if (Serial3.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial3.read();

    if (incomingByte == 'N') {
      sprintf(json, "{\"semaforos\": [%c, %c, %c, %c]}", ID, getSemaforo(2), getSemaforo(3), getSemaforo(4));
      Serial3.println(json);
    }

  }
}

char getSemaforo(int id) {
  char S = '0';
  int i = 0;

  Serial4.flush();
  Serial1.flush();
  Serial2.flush();
  Serial3.flush();

  while (i < 3) {
    if (id == 2) {
      Serial4.print('S');
      delay(10);
      if (Serial4.available() > 0) {
        S = Serial4.read();
        Serial4.flush();
        break;
      }
    }
    else if (id == 3) {
      Serial1.print('S');
      delay(10);
      if (Serial1.available() > 0) {
        S = Serial1.read();
        Serial1.flush();
        break;
      }
    }
    else if (id == 4) {
      Serial2.print('S');
      delay(10);
      if (Serial2.available() > 0) {
        S = Serial2.read();
        Serial2.flush();
        break;
      }
    }
    i++;
  }
  if(S == 'S')
    S = '0';
  return S;
}

