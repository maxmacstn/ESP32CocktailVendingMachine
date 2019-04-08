#include <Arduino.h>
#include "BluetoothSerial.h"
#include "VendingController.h"
#include "FastLED.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

VendingController vendingController(5,18,19);
BluetoothSerial SerialBT;
bool isDispensing = false;


// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

uint8_t led = 4;


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void dispense(int ml1, int ml2, int ml3){
    Serial.printf("Dispensing.. %d %d %d\n",ml1,ml2,ml3);
    vendingController.dispense(ml1,ml2,ml3);
    isDispensing = true;
}

void doAction(String in){
  Serial.println("in = \'"+ in+"\'");
  if(in == "chk"){
    Serial.println("Send ok");
    SerialBT.println("ok");
  }
  if (in.substring(0,3) == "dsp"){
      int dspVal1 =  getValue(in,' ', 1).toInt();
      int dspVal2 =  getValue(in,' ', 2).toInt();
      int dspVal3 =  getValue(in,' ', 3).toInt();
      for(int i = 1023; i >= 0; i--){
          ledcWrite(ledChannel,i);
      }
      dispense(dspVal1,dspVal2,dspVal3);
  }
}


void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32 Vending Machine"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
      // dispense(1250,833,500);
  pinMode(led,OUTPUT);
    // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO2 to be controlled
  ledcAttachPin(led, ledChannel);
  for(int i = 0; i < 1024; i++){
    ledcWrite(ledChannel,i);
    delay(10);
  }


}

void ledFade(){
  static int cur = 0;
  static bool increase = true;
  if (increase){
    ledcWrite(ledChannel,cur);
    cur +=10;
}
  else{
    ledcWrite(ledChannel,cur);
    cur -=10;
  }
  if (cur >= 1024){
    increase = false;
    cur = 1024;
  }
  if (cur <= 200){
    increase = true;
    cur = 200;
  }

}


void loop() {
  if (Serial.available()) {
    // SerialBT.write(Serial.read());
        doAction(Serial.readString());
        Serial.flush();

  }
  if (SerialBT.available()) {
    String in =  SerialBT.readString();
    in.replace("\n","");
    Serial.println(in);
    SerialBT.flush();
    doAction(in);
  }

  // Serial.println(vendingController.getProgress());
  //     Serial.printf("%d %d %d %d\n", vendingController.dispStopTime[0],vendingController.dispStopTime[1],vendingController.dispStopTime[2],vendingController.dispStartTime);
  //     Serial.println(millis());



  vendingController.run();

  if (isDispensing){
    Serial.printf("Progress = %d\n", vendingController.getProgress());
    SerialBT.println(vendingController.getProgress());
    if (vendingController.getProgress()== 100){
      isDispensing = false;

      for(int i = 0; i < 5; i++){
          vendingController.run();
          ledcWrite(ledChannel,0);
          delay(300);
          ledcWrite(ledChannel,1024);
          delay(300);
      }
    }

  }

  delay(50);
  

}

