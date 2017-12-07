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

void retornaStatus(String stats) {
  Serial3.println(stats);
}

String getSemaforos() {
  String semaforos = "";
  String semaforo;
  for (int i = 2; i <= 4; i++) {
    semaforo = getSemaforo(i);
    if (!semaforo.equals("0"))
      semaforos += "," + semaforo;
  }
  return ID + semaforos;
}

String getSemaforo(int id) {
  String S = "";    // string to hold input
  int i = 0;

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

  clearFLush();

  S.trim();
  return S;
}

String setSemaforo(String data, char incomingByte) {
  String S = "";
  Serial4.print(incomingByte);
  Serial4.println(data);
  if (incomingByte == 'D') {
    while (Serial4.available() > 0) {
      int c = Serial4.read();
      if (c == '\n' || c == 'S')
        break;
      else
        S += (char)c;
    }
  }
  Serial1.print(incomingByte);
  Serial1.println(data);
  if (incomingByte == 'D') {
    while (Serial1.available() > 0) {
      int c = Serial1.read();
      if (c == '\n' || c == 'S')
        break;
      else
        S += (char)c;
    }
  }

  Serial2.print(incomingByte);
  Serial2.println(data);
  if (incomingByte == 'D') {
    while (Serial2.available() > 0) {
      int c = Serial2.read();
      if (c == '\n' || c == 'S')
        break;
      else
        S += (char)c;
    }
  }
  S.trim();
  return S;
}



long detRate(int recpin)  // function to return valid received baud rate
// Note that the serial monitor has no 600 baud option and 300 baud
// doesn't seem to work with version 22 hardware serial library
{
  long baud, rate = 10000, x;
  for (int i = 0; i < 10; i++) {
    x = pulseIn(recpin, LOW);  // measure the next zero bit width
    rate = x < rate ? x : rate;
  }

  if (rate < 12)
    baud = 115200;
  else if (rate < 20)
    baud = 57600;
  else if (rate < 29)
    baud = 38400;
  else if (rate < 40)
    baud = 28800;
  else if (rate < 60)
    baud = 19200;
  else if (rate < 80)
    baud = 14400;
  else if (rate < 150)
    baud = 9600;
  else if (rate < 300)
    baud = 4800;
  else if (rate < 600)
    baud = 2400;
  else if (rate < 1200)
    baud = 1200;
  else
    baud = 0;
  return baud;
}


