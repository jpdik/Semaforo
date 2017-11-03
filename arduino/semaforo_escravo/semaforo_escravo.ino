int V = 48;
int Y = 50;
int G = 52;

char incomingByte;

#define ID '3'

void setup(){
  pinMode(V, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  digitalWrite(V, HIGH);
  digitalWrite(Y, HIGH);
  digitalWrite(G, HIGH);

  Serial.begin(9600);
  Serial3.begin(9600);
}
void loop(){
  if(Serial3.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial3.read();

    if(incomingByte == 'S'){
      Serial3.print(ID);
    }
  }
}

