#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <M5Stack.h>
#include <WiFi.h>
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

char auth[] = "YOUR_AUTH";

#include <Wire.h>
#include <Preferences.h>
#include "imuCalibration.h"
#include "M5Bala.h"
Preferences preferences;
M5Bala m5bala(Wire);

void playMP3(char *filename){
  file = new AudioFileSourceSD(filename);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S(0, 1); // Output to builtInDAC
  out->SetOutputModeMono(true);
  out->SetGain(1.0);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while(mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
  }
}

void setup() {
  M5.begin();   
  WiFi.mode(WIFI_OFF); 
  delay(500);

  Wire.begin();
  Wire.setClock(400000UL);  // Set I2C frequency to 400kHz
  m5bala.begin();
  m5bala.setAngleOffset(-2.2);

  Serial.println("Waiting for connections...");
  Blynk.setDeviceName("Blynk on M5Stack2");
  Blynk.begin(auth);

}

BLYNK_WRITE(V0) {
  int x = param[0].asInt();
  int y = param[1].asInt();
  int v = map(x,0,1023,-255,255);
  int w = map(y,0,1023,-255,255);
  Serial.print("X = ");
  Serial.print(x);
  Serial.print("; Y = ");
  Serial.print(y);
  Serial.print("; V = ");
  Serial.print(v);
  Serial.print("; W = ");
  Serial.println(v);
  m5bala.setMotor(v,v);
}

BLYNK_WRITE(V1) {
    Serial.println("left turn");
    m5bala.setMotor(-255,255);  
    delay(30);
    m5bala.setMotor(0,0);    
}

BLYNK_WRITE(V2) {
    Serial.println("right turn");
    m5bala.setMotor(255,-255);
    delay(30);
    m5bala.setMotor(0,0);    
}

BLYNK_WRITE(V3) {
    playMP3("/ap.mp3");
    Serial.printf("MP3 done\n");
}

void loop() {
  Blynk.run();

}
