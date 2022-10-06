// librerie gestione interfaccia SPI e SD
#include <SPI.h>
#include <SD.h>
// connessione i2c
#include <Wire.h>
// libreria sensore bpm280
#include <Adafruit_BMP280.h>

// creazione oggetto file
File file;
// creo oggetto sensore
Adafruit_BMP280 bmx;
// datalog file
constexpr char logFileName[] = "datalog.txt";

void setup() {
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  // inizial conn i2c
  Wire.begin();
  // verifico conn a bmp280
  if (!bmx.begin(0x76)) {
    digitalWrite(8, HIGH);
    while(1);
  }
  // verifico conn a SD
  if (!SD.begin(10)) {
    digitalWrite(9, HIGH);
    while(1);
  }
  // check esistenza file, in caso lo elimino
  if (SD.exists(logFileName)) {
    SD.remove(logFileName);
    delay(500);
  }
  
  //Default settings from datasheet. 
  bmx.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X16);
  delay(500);
  // creo file datalog.txt
  file = SD.open(logFileName, FILE_WRITE);
  file.println("DATI ALTITUDINE");
  digitalWrite(9, HIGH);
  delay(500);
  digitalWrite(9, LOW);
  file.close();
}


void loop() {
  
  // stampo i numeri nel file
  file = SD.open(logFileName, FILE_WRITE);
  file.print("altitude:  ");
  file.print(bmx.readAltitude(1013.25), DEC);
  file.print("  ");
  file.println(millis(), DEC);
  delay(300);
  file.close();
  
}
