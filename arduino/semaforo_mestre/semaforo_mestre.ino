#include <SoftwareSerial.h>

#define MANUTENCAO 0
#define ABERTO 1

SoftwareSerial Serial4(10, 11); // RX, TX

int V = 48;
int Y = 50;
int G = 52;

int estado = 0; // 0 = manutenção
int stats = 0;
bool primeiro = false;
int tempoAberto = 0;
int tempoWarn = 3000;
int tempoFechado = 0;
int tempoManutencao = 1000;

unsigned long currentMillis;
unsigned long previousMillis = 0;

char incomingByte;

#define ID 1

char json[1024];

void setup() {
  pinMode(V, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial4.begin(9600);

  getSemaforos();
}
void loop() {
  if (Serial3.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial3.read();

    if (incomingByte == 'N') {
      sprintf(json, "[%s]", getSemaforos().c_str());
      Serial.println(json);
      Serial3.println(json);
      Serial3.flush();
    }
    else if (incomingByte == 'C') {
      Serial3.print('1');
      configurarSemaforo(incomingByte);
    }
    else if (incomingByte == 'T') {
      Serial3.print('1');
      trocaEstadoSemaforos(incomingByte);
    }
    else if (incomingByte == 'R') {
      Serial3.print('1');
      reiniciarSemaforos(incomingByte);
    }
    else if (incomingByte == 'D') {
      Serial3.print('1');
      getStatus(incomingByte);
    }
  }
  semaforo();
}

void trocaEstadoSemaforos(char incomingByte) {
  String data = "";
  String S = "";
  int id;
  int estado;

  while (true) {
    if (Serial3.available() > 0) {
      int c = Serial3.read();
      if (c == '\n'){
        estado = S.toInt();
        if (id == ID)
          trocaEstado(estado);
        break;
      }
      if (c == '-') {
        id = S.toInt();
        S = "";
        data += (char)c;
        continue;
      }
      if (c == '|') {
        estado = S.toInt();
        if (id == ID)
          trocaEstado(estado);

        S = "";
      }
      else
        S += (char)c;
      data += (char)c;
    }
  }
  setSemaforo(data, incomingByte);
}

void reiniciarSemaforos(char incomingByte) {
  String data = "";
  String S = "";
  int id;

  while (true) {
    if (Serial3.available() > 0) {
      int c = Serial3.read();
      if (c == '\n'){
        primeiro = S.toInt();
        if (id == ID)
          reiniciar();
        break;
    }
      if (c == '-') {
        id = S.toInt();
        S = "";
        data += (char)c;
        continue;
      }
      if (c == '|') {
        primeiro = S.toInt();
        if (id == ID)
          reiniciar();

        S = "";
      }
      else
        S += (char)c;
      data += (char)c;
    }
  }
    Serial.println(data);
  setSemaforo(data, incomingByte);
}

void configurarSemaforo(char incomingByte) {
  String data = "";
  String S = "";
  int id;
  int aberto;
  int fechado;

  while (true) {
    if (Serial3.available() > 0) {
      int c = Serial3.read();
      if (c == '\n')
        break;
      if (c == ';') {
        id = S.toInt();
        S = "";
        data += (char)c;
        continue;
      }
      if (c == ',') {
        aberto = S.toInt();
        S = "";
      }
      else
        S += (char)c;
      data += (char)c;
    }
  }
  fechado = S.toInt();
  if (id == ID)
    configurar(aberto, fechado);
  else
    setSemaforo(data, incomingByte);
}

void getStatus(char incomingByte) {
  String data = "";
  String S = "";
  int estatus;

  while (true) {
    if (Serial3.available() > 0) {
      int c = Serial3.read();
      if (c == '\n')
        break;
      else
        S += (char)c;
      data += (char)c;
    }
  }
  estatus = S.toInt();
  if (estatus == ID)
    retornaStatus(stats);
  else
    retornaStatus(setSemaforo(data, incomingByte));
}

void retornaStatus(int stats){
  Serial3.println(stats);
}

String getSemaforos() {
  String semaforos = "";
  String semaforo;
  for (int i = 2; i <= 4; i++) {
    semaforo = getSemaforo(i);
    Serial.println(semaforo);
    if (!semaforo.equals("0"))
      semaforos += "," + semaforo;
  }
  return ID + semaforos;
}

String getSemaforo(int id) {
  String S = "";    // string to hold input
  int i = 0;

  Serial1.flush();
  Serial2.flush();
  Serial3.flush();
  Serial4.flush();

  while (i < 3) {

    if (id == 2) {
      Serial4.print('S');
      delay(10);
      while (Serial4.available() > 0) {
        int c = Serial4.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;

      }
      break;
    }
    else if (id == 3) {
      Serial1.print('S');
      delay(10);
      while (Serial1.available() > 0) {
        int c = Serial1.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;
      }
      break;
    }
    else if (id == 4) {
      Serial2.print('S');
      delay(10);
      while (Serial2.available() > 0) {
        int c = Serial2.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;
      }
      break;
    }
    i++;
  }
  if (S.equals("") || S.equals("S"))
    S = "0";
  S.trim();
  return S;
}

int setSemaforo(String data, char incomingByte) {
  String S = "";
  
  if (Serial4.available() > 0) {
    Serial4.print(incomingByte);
    Serial4.println(data);
    if(incomingByte == 'D'){
      while (Serial4.available() > 0) {
        int c = Serial4.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;
      }
    }
  }
  if (Serial1.available() > 0) {
    Serial1.print(incomingByte);
    Serial1.println(data);
    if(incomingByte == 'D'){
      while (Serial1.available() > 0) {
        int c = Serial1.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;
      }
    }
  }

  if (Serial2.available() > 0) {
    Serial2.print(incomingByte);
    Serial2.print(data);
    if(incomingByte == 'D'){
      while (Serial2.available() > 0) {
        int c = Serial2.read();
        if (c == '\n' || c == 'S')
          break;
        else
          S += (char)c;
      }
    }
  }
  S.trim();
  return S.toInt();
}

void semaforo() {
  currentMillis = millis();
  if (estado == 0) {
    if (currentMillis - previousMillis >= tempoManutencao) {
      previousMillis = currentMillis;
      digitalWrite(Y, !digitalRead(Y));
    }
  }
  else if (estado == 1) {
    if (stats == 0) {
      if (currentMillis - previousMillis >= tempoAberto) {
        previousMillis = currentMillis;
        digitalWrite(G, LOW);
        digitalWrite(Y, HIGH);
        stats = 1;
      }
    }
    else if (stats == 1) {
      if (currentMillis - previousMillis >= tempoWarn) {
        previousMillis = currentMillis;
        digitalWrite(Y, LOW);
        digitalWrite(V, HIGH);
        stats = 2;
      }
    }
    else {
      if (currentMillis - previousMillis >= tempoFechado + tempoWarn/2) {
        previousMillis = currentMillis;
        digitalWrite(V, LOW);
        digitalWrite(G, HIGH);
        stats = 0;
      }
    }
  }
}

void trocaEstado(int state) {
  estado = state;
  reiniciar();
}

void desligarTodos() {
  digitalWrite(V, LOW);
  digitalWrite(Y, LOW);
  digitalWrite(G, LOW);
}

void configurar(int aberto, int fechado) {
  tempoAberto = aberto * 1000;
  tempoFechado = fechado * 1000;
}

void reiniciar() {
  desligarTodos();

  previousMillis = currentMillis;

  if (estado == ABERTO)
    if (primeiro == true) {
      digitalWrite(G, HIGH);
      stats = 0;
    }
    else {
      digitalWrite(V, HIGH);
      stats = 2;
    }
}

