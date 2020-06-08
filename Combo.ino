#include <M5StickC.h>
#include <M5Display.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <string>
#include <cstring>
#include <SimpleTimer.h>

//int timeCounter;

SimpleTimer timer;

uint8_t led_count = 15;
long brightnessTime, tiltTime = 0, tiltTime2 = 0;
float b, c = 0;
int battery = 0;

float accX = 0;
float accY = 0;
float accZ = 0;

#define TFT_GREY 0x5AEB
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;

boolean initial = 1;

const char* ssid = "502M@unifi";
const char* password = "AsyRazMan502m";

const uint16_t port = 8090;
const char* host = "192.168.0.179";

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
//      M5.Lcd.println(cstr);
      client.print(cstr);
      delay(1000);
//      client.print(dist);
//      M5.Lcd.println(cdist);      
//      delay(50);
    }
  }
};

// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  Wire.begin(32, 33);
  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);
  pinMode(M5_BUTTON_HOME, INPUT_PULLUP);

  timer.setInterval(300000, getScansAndSend);
  timer.setInterval(60000, batteryLevel);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_37, 0);
  
  M5.Lcd.setRotation(3);
//  M5.Lcd.fillScreen(TFT_GREY);
//  M5.Lcd.setTextColor(TFT_WHITE, TFT_GREY);
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
  if(!client.connect(host, port)) {
//    M5.Lcd.println("Connection to host failed");

    delay(1000);
    return;
  }
//  M5.Lcd.begin();
//  M5.Lcd.setRotation(3);
//  M5.Lcd.setCursor(0, 0);
//  M5.Lcd.println("Connected to server successfully");
 
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
//  M5.Lcd.print("Devices found: ");
//  M5.Lcd.println(foundDevices.getCount());
//  M5.Lcd.println("Scan done!");
//
//  M5.Lcd.println("Disconnection...");
  client.stop();

  pBLEScan->clearResults();
}

void batteryLevel() {
  M5.Lcd.begin();
  M5.Lcd.setCursor(110, 3, 1);
  c = M5.Axp.GetVapsData() * 1.4 / 1000;
  b = M5.Axp.GetVbatData() * 1.1 / 1000;
  //  M5.Lcd.print(b);
  battery = ((b - 3.0) / 1.2) * 100;

  if (c >= 4.5) {
    M5.Lcd.setTextColor(TFT_YELLOW);
    M5.Lcd.print("CHG:");
  }
  else {
    M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.print("BAT:");
  }

  if (battery > 100)
    battery = 100;
  else if (battery < 100 && battery > 9)
    M5.Lcd.print(" ");
  else if (battery < 9)
    M5.Lcd.print("  ");
  if (battery < 10)
    M5.Axp.DeepSleep();

//  if (digitalRead(M5_BUTTON_HOME) == LOW) {
//    while (digitalRead(M5_BUTTON_HOME) == LOW);
//    M5.Axp.DeepSleep(SLEEP_SEC(1));
//  }
  M5.Lcd.print(battery);
  M5.Lcd.print("%");
}

// the loop routine runs over and over again forever
void loop() {
  timer.run();
}
