#include <M5StickCPlus.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float accXSq = 0.0F;
float accYSq = 0.0F;
float accZSq = 0.0F;
float magSq = 0.0F;
float curMag = 0.0F;
float maxMag = 0.0F;

int timerSt = millis();

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(80, 15);
  M5.Lcd.println("IMU TEST");

  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
}


/*****************************************
M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
M5.IMU.getAccelData(&accX,&accY,&accZ);
M5.IMU.getAhrsData(&pitch,&roll,&yaw);
M5.IMU.getTempData(&temp);
*****************************************/
void loop() {
  // put your main code here, to run repeatedly:
  
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  magSq = ((pow(accX,2)) + (pow(accY,2)) + (pow(accZ,2)));
  curMag = pow(magSq,0.5);
  
  
  M5.Lcd.setTextSize(2);
  if(curMag > maxMag){
    maxMag = curMag;
  }
  
  M5.Lcd.setCursor(80, 50);
  M5.Lcd.println(maxMag);

  int timerCur = millis();
  int timerDiff = timerCur - timerSt;
  
  if(timerDiff > 20000){
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(SECRET_SSID);
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        Serial.print(".");
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
  
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeField(myChannelNumber, 1, maxMag, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    timerSt = timerCur;
  }
  
  
  
}
