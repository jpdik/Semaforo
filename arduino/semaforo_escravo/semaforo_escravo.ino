#include <SoftwareSerial.h>
#include <EEPROM.h>

#define MANUTENCAO 0
#define ABERTO 1

SoftwareSerial Serial4(13, 12); // RX, TX

int V = 48;
int Y = 50;
int G = 52;

char json[1024];

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

long obterIDEEPROM();

long ID = obterIDEEPROM();

void setup() {
  pinMode(V, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  Serial4.begin(115200);
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

}
void loop() {
  if (Serial3.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial3.read();


    if (incomingByte == 'S') {
      sprintf(json, "%s", getSemaforos().c_str());
      Serial3.println(json);
    }
    else if (incomingByte == 'C') {
      configurarSemaforo(incomingByte);
    }
    else if (incomingByte == 'T') {
      trocaEstadoSemaforos(incomingByte);
    }
    else if (incomingByte == 'R') {
      reiniciarSemaforos(incomingByte);
    }
    else if (incomingByte == 'D') {
      getStatus(incomingByte);
    }
  }
  semaforo();
}

/*Funções Padrões do Arduino
  Não foi utilizada uma lib externa nos projetos pois o arduino IDE não tem suporte para subdiretórios
  pois ele compila o .ino somente em seu diretório local.
*/
void trocaEstadoSemaforos(char incomingByte) {
  String data = "";
  String S = "";
  int id;
  int estado;

  while (true) {
    if (Serial3.available() > 0) {
      int c = Serial3.read();
      if (c == '\n') {
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
      if (c == '\n') {
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
      data += (char)c;
      if (c == '\n')
        break;
      else
        S += (char)c;
    }
  }
  estatus = S.toInt();
  if (estatus == ID)
    retornaStatus(String(stats));
  else
    retornaStatus(setSemaforo(data, incomingByte));
}

void retornaStatus(String stats) {
  Serial.println(stats);
  Serial3.println(stats);
}

String getSemaforos() {
  String semaforos = "";
  String semaforo;
  for (int i = 2; i <= 4; i++) {
    semaforo = getSemaforo(i);
    if (!semaforo.equals("-1"))
      semaforos += "," + semaforo;
  }
  return ID + semaforos;
}


String getSemaforo(int id) {
  String S = "";    // string to hold input
  int i = 0;
  int c;

  while (i < 10) {
    if (id == 2) {
      Serial4.print('S');
      while (Serial4.available() > 0) {
        c = Serial4.read();
        if (c == '\n' || c == 'S')
          return cleanBuffer(S);
        else if (isDigit(c) || c == ',' || c == ' ')
          S += (char)c;
      }
    }
    else if (id == 3) {
      Serial1.print('S');
      while (Serial1.available() > 0) {
        c = Serial1.read();
        if (c == '\n' || c == 'S')
          return cleanBuffer(S);
        else if (isDigit(c) || c == ',' || c == ' ')
          S += (char)c;
      }
      if (c == '\n') break;
    }
    else if (id == 4) {
      Serial2.print('S');
      while (Serial2.available() > 0) {
        c = Serial2.read();

        if (c == '\n' || c == 'S')
          return cleanBuffer(S);
        else if (isDigit(c) || c == ',' || c == ' ')
          S += (char)c;
      }
    }
    i++;
  }
  return "0";
}

String setSemaforo(String data, char incomingByte) {
  String S = "";
  int i = 0;
  int c;

  Serial4.print(incomingByte);
  Serial4.println(data);

  Serial1.print(incomingByte);
  Serial1.println(data);

  Serial2.print(incomingByte);
  Serial2.println(data);

  if (incomingByte == 'D') {
    while (Serial4.available() > 0) {
      c = Serial4.read();
      if (c == '\n' || c == 'D' || c == "-1")
        break;
      else if (isDigit(c))
        S += (char)c;
    }

    while (Serial1.available() > 0) {
      c = Serial1.read();
      if (c == '\n' || c == 'D' || c == "-1")
        break;
      else if (isDigit(c))
        S += (char)c;
    }

    while (Serial2.available() > 0) {
      c = Serial2.read();
      if (c == '\n' || c == 'D' || c == "-1")
        break;
      else if (isDigit(c))
        S += (char)c;
    }
  }
  Serial.println(S);
  return cleanBuffer(S);
}

String cleanBuffer(String buff) {
  if (buff.equals(""))
    buff = "-1";

  buff.trim();
  return buff;
}

void semaforo() {
  currentMillis = millis();
  if (estado == 0) {
    if (currentMillis - previousMillis >= tempoManutencao) {
      previousMillis = currentMillis;
      digitalWrite(Y, !digitalRead(Y));
      stats = 2;
    }
  }
  else if (estado == 1) {
    if (stats == 0) {
      if (currentMillis - previousMillis >= tempoAberto - tempoWarn) {
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
      if (currentMillis - previousMillis >= tempoFechado) {
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
  else
    stats = 1;
}

void clearFlush(int serial) {
  if (serial == 1 || serial == -1)
    while (Serial1.available() > 0)
      Serial1.read();

  else if (serial == 2 || serial == -1)
    while (Serial2.available() > 0)
      Serial2.read();

  else if (serial == 3 || serial == -1)
    while (Serial3.available() > 0)
      Serial3.read();

  else if (serial == 4 || serial == -1)
    while (Serial4.available() > 0)
      Serial4.read();
}

long obterIDEEPROM()
{
  int addr = 0;

  long four = EEPROM.read(addr);
  long three = EEPROM.read(addr + 1);
  long two = EEPROM.read(addr + 2);
  long one = EEPROM.read(addr + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
