#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

#define EJECT_PIN 32
#define BUZZ_PIN 27
#define BMP_SDA 21 
#define BMP_SCL 22
// CS: 4, MOSI: 23, CLK: 18, MISO: 19

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

float altitudeInicial;

File dataFile;
float checkAltitude[3] = {0,0,0};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Teste");
  pinMode(EJECT_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(EJECT_PIN, HIGH);
  
  Serial.println("1");
  digitalWrite(BUZZ_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZ_PIN, LOW);
  delay(500);

  if (!mpu.begin()) {
    Serial.println(F("Erro ao iniciar o MPU6050. Verifique as conexões!"));
    digitalWrite(BUZZ_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZ_PIN, LOW);
    delay(500);
    while (1);
  }

  if (!bmp.begin(0x76)) {
    Serial.println(F("Erro ao iniciar o BMP280. Verifique as conexões!"));
    for(int i = 0; i < 2; i++){
      digitalWrite(BUZZ_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZ_PIN, LOW);
      delay(500);
    }
    while (1);
  }

  Serial.println("2");

  if (!SD.begin(4)) {
    Serial.println(F("Erro ao iniciar o cartão SD. Verifique a conexão e o formato do cartão."));
      for(int i = 0; i < 3; i++){
      digitalWrite(BUZZ_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZ_PIN, LOW);
      delay(500);
    }
    while (1);
  }
  

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  

  altitudeInicial = bmp.readAltitude();


  dataFile = SD.open("/altitude_data.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.println("-------------------------------------------------- TESTE --------------------------------------------------");
    dataFile.println("------------------------------------------------- LIGADO --------------------------------------------------");
    dataFile.println("-----------------------------------------------------------------------------------------------------------");
    dataFile.close();
  } else {
    Serial.println(F("Erro ao abrir o arquivo no cartão SD."));
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float altitude = bmp.readAltitude();
  float altitudeNormal = altitude - altitudeInicial;
  float aceX, aceY, aceZ, giroX, giroY, giroZ;
  aceX = a.acceleration.x;
  aceY = a.acceleration.y;
  aceZ = a.acceleration.z;
  giroX = g.gyro.x;
  giroY = g.gyro.y;
  giroZ = g.gyro.z;

  bool recuperarAlt = false;
  bool recuperarGiro = false;

  //checkAltitude mantém as informações de altitude dos últimos 3 segundos, se no decorrer dos 3 segundos a altitude apenas diminuir, acionaremos a recuperação
  checkAltitude[0] = checkAltitude[1];
  checkAltitude[1] = checkAltitude[2];
  checkAltitude[2] = altitude;

  float margemErro = 0.5;
  if (checkAltitude[0] > (checkAltitude[1] + margemErro) && checkAltitude[1] > (checkAltitude[2] + margemErro)) {
    Serial.println("recuperacao pela Altitude");
    recuperarAlt = true;
  }

  //sensGiro é a velocidade em graus por segundo que ativará a recuperação, aumente ou diminua conforme necessidade
  float sensGiro = 5;
  if (((giroX > sensGiro || giroX < -sensGiro) || (giroY > sensGiro || giroY < -sensGiro)) && altitudeNormal > 275){
    Serial.println("recuperacao pelo Giroscopio");
    recuperarGiro = true;
  }

  if(recuperarAlt || recuperarGiro){
    recuperar();
  } else{
    digitalWrite(EJECT_PIN, HIGH);
    digitalWrite(BUZZ_PIN, LOW);
  }

  dataFile = SD.open("/altitude_data.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.println("Altitude: " + String(altitudeNormal) + " metros" + ", Aceleração X: " + String(aceX) + ", Y:" + String(aceY) + ", Z:" + String(aceZ) + "m/s^2" + ", Giroscópio X:" + String(giroX) + ", Y:" + String(giroY) + ", Z:" + String(giroZ) + " deg/s");
    if(recuperarAlt)
      dataFile.println("Recuperação pela altitude");
    if(recuperarGiro)
      dataFile.println("Recuperação pelo giroscópio");
    dataFile.close();
  } else {
    Serial.println(F("Erro ao abrir o arquivo no cartão SD."));
  }

  recuperarAlt = false;
  recuperarGiro = false;

  delay(1000);
}

void recuperar(){
  digitalWrite(EJECT_PIN, LOW);
  digitalWrite(BUZZ_PIN, HIGH);
}
