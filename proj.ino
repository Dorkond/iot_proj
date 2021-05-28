#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include "inttypes.h"
#include "time.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Time>
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

XT_Wav_Class ForceWithYou(Force);     // create an object of type XT_Wav_Class that is used by 
                                      // the dac audio class (below), passing wav data as parameter.
                                      
XT_Wav_Class Bleep(bleep);
XT_Wav_Class Aleep(alarm);
                                       
XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

long played_times = 0;            // Just a counter to use in the serial monitor
                                      // not essential to playing the sound


void setup() {
  Serial.begin(115200);               // Not needed for sound, just to demo printing to the serial
                                      // Monitor whilst the sound plays, ensure your serial monitor
                                      // speed is set to this speed also.

const int ledPin_1 = 13; // the PWM pin for sound control
const int freq = 2000;
const int ledChannel_0 = 0;
const int resolution = 8; // Other PWM properties to control volume

int potValue = 0; // Volume

  BLEDevice::init("RZD");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Default");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

}




void loop() {
  
  ledcSetup(ledChannel_0, freq, resolution); // Configure Volume PWM function
  ledcAttachPin(ledPin_1, ledChannel_0); 
  

  std::string characteristic = "Test"
  class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      characteristic = value;
      
    }
  };
  // Characteristic is meant to be string "X Y", where X - int, sound volume 0-127 that should be applied to the speaker
  // Y - current time, int, used to configure the alert according to the trains timetable (we used alerts 1 in 1 hour, for example)

  DacAudio.FillBuffer();                // Fill the sound buffer with data
  if(ForceWithYou.Playing==false && played_times == 0){       // Startup sound
    DacAudio.Play(&ForceWithYou);
    played_times++;
    Serial.println("played startup sound");  // Playing status sounds via DAC pins on ESP-32
  }
  if(Bleep.Playing==false && ForceWithYou.Playing==false && played_times % 5000000 == 0 & characteristic != "Test") { // In this state ESP is ready to connect and bleeps sometimes until characteristic changes
    DacAudio.Play(&Bleep);
    Serial.println("Played bleep");  
  }
  // That sector of code is executed only if device is connected to ESP and characteristic had changed
  char *pch = strtok(characteristic, " ");
  potValue = pch
  volume = map(potValue, 0, 4095, 0, 255);
  ledcWrite(ledChannel_0, volume);
  // From now on ESP is a smart resistor, dividing the voltage on the 0-127 scale, where 0 - 0v, 127-3,3v
  char *pch = strtok(NULL, " ");
  setTime(pch); // Configuring time locally on ESP-32 using data from connected phone

  if (Alarm.Playing==false && getMinute() == 0) {
    DacAudio.Play(&Alarm);
    delay(30);
    DacAudio.Play(&Alarm);
    delay(30);
    DacAudio.Play(&Alarm);
    delay(30);
  }
  // From now on ESP is configured to alert bypassers with volume that is safe under local goverment regulations. 
  played_times++; 
}
