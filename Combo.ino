#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <string>
#include <cstring>
#include <SimpleTimer.h>

SimpleTimer timer;

float b, c = 0;
int battery = 0;

const char* ssid = "502M@unifi";
const char* password = "AsyRazMan502m";

const uint16_t port = 8090;
const char* server = "192.168.0.195";   //server
const char* pc = "192.168.0.179";   //pc

String result = "";

int scanTime = 5;
BLEScan* pBLEScan;

WiFiClient client;

class FindBeacon: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String found = advertisedDevice.getAddress().toString().c_str();
    String dist = (String)-(int)advertisedDevice.getRSSI();    
    
    if (found && dist) {
      char * cstr = new char[found.length() + 1];
      char * cdist = new char[3];
      std::strcpy(cstr, found.c_str());
      std::strcpy(cdist, dist.c_str());
      
      char p = ':';

      int len = 17;
      for (int i = 0; i < len; i++){
        if(cstr[i] == p){
          for(int j = i; j < len; j++){
            cstr[j] = cstr[j + 1];
          }
          len--;
          i--;
        }
      }

      strcat(cstr, cdist);
      M5.Lcd.println(cstr);
      client.print(cstr);
      delay(1000);
    }
  }
};

// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  delay(1000);
  timer.setInterval(300000, getScansAndSend);
  timer.setInterval(60000, batteryLevel);
  
  M5.Lcd.setRotation(3);
  M5.Axp.SetLDO2(false);
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new FindBeacon());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void getScansAndSend(){
  if(!client.connect(pc, port)) {
    delay(1000);
    if(!client.connect(server,port)){
      delay(1000);
      return;
    }
  }
  M5.Lcd.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0);
  M5.Axp.SetLDO2(true);
 
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
//  M5.Lcd.print("Devices found: ");
//  M5.Lcd.println(foundDevices.getCount());
//  M5.Lcd.println("Scan done!");
//
//  M5.Lcd.println("Disconnection...");
  client.stop();

  pBLEScan->clearResults();
  M5.Axp.SetLDO2(false);
}

void batteryLevel() {
  M5.Lcd.begin();
  M5.Lcd.setCursor(110, 3, 1);
  M5.Axp.SetLDO2(true);
  c = M5.Axp.GetVapsData() * 1.4 / 1000;
  b = M5.Axp.GetVbatData() * 1.1 / 1000;
  //  M5.Lcd.print(b);
  battery = ((b - 3.0) / 1.2) * 100;

//  if (c >= 4.5) {
//    M5.Lcd.setTextColor(TFT_YELLOW);
//    M5.Lcd.print("CHG:");
//  }
//  else {
//    M5.Lcd.setTextColor(TFT_GREEN);
//    M5.Lcd.print("BAT:");
//  }
//
//  if (battery > 100)
//    battery = 100;
//  else if (battery < 100 && battery > 9)
//    M5.Lcd.print(" ");
//  else if (battery < 9)
//    M5.Lcd.print("  ");
  if (battery < 10){
//    M5.Axp.DeepSleep();
    for(int i = 0; i < 5; i++){
      digitalWrite(10, LOW);
      delay(500);
      digitalWrite(10, HIGH);
      delay(500);
    }
  }

//  if (digitalRead(M5_BUTTON_HOME) == LOW) {
//    while (digitalRead(M5_BUTTON_HOME) == LOW);
//    M5.Axp.DeepSleep(SLEEP_SEC(1));
//  }
  M5.Lcd.print(battery);
  M5.Lcd.print("%");
  delay(2000);
  M5.Axp.SetLDO2(false);
}

// the loop routine runs over and over again forever
void loop() {
  timer.run();
}
