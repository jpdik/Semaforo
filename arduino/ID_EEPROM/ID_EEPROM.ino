#include <EEPROM.h>

#define ID 1

void setup() {
  Serial.begin(9600);
  criarIDEEPROM(ID);
}

void loop() {
}

void criarIDEEPROM(long value)
{
  int addr = 0;
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  EEPROM.write(addr, four);
  EEPROM.write(addr + 1, three);
  EEPROM.write(addr + 2, two);
  EEPROM.write(addr + 3, one);
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
