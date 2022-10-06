// include librerie per i2c e gestione mpu
#include <Wire.h>
#include <MPU6050_tockn.h>

// libreria gestione servocomando
#include <Servo.h>

// librerie gestione interfaccia SPI e SD
#include <SPI.h>
#include <SD.h>
//##########################################################################

// creo oggetto per mpu
MPU6050 mpu6050(Wire);

// creazione oggetti
Servo servoc;

// creazione oggetto file
File file;

// define
constexpr char logFileName[] = "datalog.txt";

void setup() {
  //pinmode
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  
  //  check connessione SD
  if (!SD.begin(10)) {
    digitalWrite(6, HIGH);
    while(1);
  }
  
  // check esistenza file, in caso lo elimino
  if (SD.exists(logFileName)) {
    SD.remove(logFileName);
    digitalWrite(7, HIGH);
    delay(2000);
  }
  digitalWrite(7, LOW);
  
  digitalWrite(8, HIGH);
  //  creo file in modalità WRITE
  file = SD.open(logFileName, FILE_WRITE);
  file.println("coordinate inclinazione x y z");
  delay(2000);
  digitalWrite(8, LOW);
  delay(1000);

  // inizializzo i2c ed mpu
  digitalWrite(8, HIGH);
  Wire.begin();
  mpu6050.begin();
  // taro mpu
  mpu6050.calcGyroOffsets(true);
  delay(1000);
  digitalWrite(8, LOW);
  file.close();

  // inizializzo pin servo e posizione
  servoc.attach(9);
  servoc.write(30);
  delay(2000);
}

//##########################################################################
// definizione di altre varibili
unsigned long dt, t1=0;
float x, y, z;

void loop() {
  // update del mpu6050
  mpu6050.update();
  digitalWrite(6, HIGH);
  // prendo dati ogni 0.2 sec
  dt = millis() - t1;
  if (dt >= 300) {
    t1 = millis();
    digitalWrite(6, LOW);
    
    // estraggo dati angoli lungo le coordinate
    x = mpu6050.getAngleX();
    y = mpu6050.getAngleY();
    z = mpu6050.getAngleZ();

    // converto dati numerici in stringa
    String coord = String(x,1) + " " + String(y,1) + " " + String(z,1) +" ";

    // stampo i numeri nel file
    file = SD.open(logFileName, FILE_WRITE);
    file.print(coord.c_str());
    file.println(t1, DEC);
    
    // check inclinazione, in caso positivo attivo il servocomando
    if (z > 70 || z < -70) {
      servoc.write(180);
      file.println("APERTURA PARACADUTE");
      delay(50);
    }
    file.close();
  }   
}
