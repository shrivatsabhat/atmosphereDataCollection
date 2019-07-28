/*
 * >>>>Data Collection of Temperature, Humidity and Light Intensity and Upload to ThingSpeak Cloud Server<<<< * 
 * 
 *Download the DHT.h library at https://github.com/markruys/arduino-DHT/archive/master.zip 
 * 
 * Component Used - 
 *  - NODEMCU - ESP8266 Wifi Development Board
 *  - BH1750 - Light Intensity Sensor Module
 *  - DHT11 - Temprature and Humidity Sensor Module
 *  - LED 
 *  
 *Connection:
 *  DHT11 -   
 *  ------------------
 *  OUT    -   Pin D0
 *  __________________
 *  BH1750 -   
 *  ------------------
 *  SCL    -   Pin D1
 *  SDA    -   Pin D2
 *  ADD    -   NC4
 *   
*/

#include "DHT.h"
#include <Wire.h>
#include <ESP8266WiFi.h> // Include the WiFi library
#include <ESP8266WiFiMulti.h> // Include the WiFi-Multi library
#include <ESP8266HTTPClient.h> 

#define LED D7 // D7 pin is assigned for LED light

#define ssid_1 "SSID"    // your Wifi network 1 SSID (name) 
#define pass_1 "PASSWORD"  // your Wifi network 1 password

#define ssid_2 "SSID" // your Wifi network 2 SSID (name) 
#define pass_2 "PASSWORD"  // your Wifi network 2 password

#define ssid_3 "SSID" // open wiFi network

#define TSWriteAPIKey "ABCD1234EFGH67"  //SECRET_WRITE_APIKEY;
#define TSField_1 1 // Temperature Field
#define TSField_2 2 // Humidity Field
#define TSField_3 3 // Light Sensor Field

byte BH1750_Device_Address = 0x23; // I2C address for BH1750 light sensor

float temperature = 0;
float humidity = 0;
unsigned int lux = 0;

DHT dht;
WiFiClient  client;
ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called WifiMulti

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  Wire.begin();
  
  dht.setup(D0);   // D0 pin for nodeMCU is used 
  pinMode(LED,OUTPUT);

  WiFi.mode(WIFI_STA); // set-up a WPA2-PSK network
}

void ConnectToWiFi(void){
  wifiMulti.addAP(ssid_1,pass_1); //  add WiFi networks you want to connect to
  wifiMulti.addAP(ssid_2,pass_2);
  wifiMulti.addAP(ssid_3);
  
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("\nAttempting to connect to WiFi\n");
    while(wifiMulti.run() != WL_CONNECTED){ // Wait for thr WiFi to connect: scan for WiFi networss, and connect to the strongest of the networks above
      Serial.println("Connecting...");
      delay(1000);     
    } 
    Serial.print("\nConnected to " + String(WiFi.SSID()) + "\n"); // Tell us what network we are connected to 
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  }
}

void Write_ThingSpeak_Data(String tempValue, String humiValue, String luxValue){
  if (WiFi.status() != WL_CONNECTED) { //Check WiFi connection status
    Serial.println("\nWiFi Disconected ?!!");
    ConnectToWiFi();
  }
  HTTPClient http;  //Declare an object of class HTTPClient
  String upLink = "http://api.thingspeak.com/update?api_key="+String(TSWriteAPIKey)+"&field"+String(TSField_1)+"=";
  upLink += String(tempValue);  //update field 1 value
  upLink += "&field"+String(TSField_2)+"="+String(humiValue);  // update field 2 value
  upLink += "&field"+String(TSField_3)+"="+String(luxValue); // update field 3 value
  //Serial.println(upLink);
  http.begin(upLink);  //Specify request destination
  int httpCode = http.GET();  //Send the GET request
  http.end();   //Close connection
}

void getTemperatureHumidity(void){
  if (WiFi.status() != WL_CONNECTED) { //Check WiFi connection status
    Serial.println("\nWiFi Disconected ?!!");
    ConnectToWiFi();
  }
  delay(dht.getMinimumSamplingPeriod());  // Delay of amount equal to sampling period 
  humidity = dht.getHumidity();     // Get humidity value 
  //Serial.println("Humidity --> "+String(humidity)+" %"); 
  temperature = dht.getTemperature(); // Get temperature value 
  //Serial.println("Temperature --> "+String(temperature)+" 'C"); 
  //Serial.println(dht.getStatusString());    // Print status of communication 
}

unsigned int BH1750_Read() {
  unsigned int value = 0;
  Wire.beginTransmission(BH1750_Device_Address);
  Wire.requestFrom(BH1750_Device_Address, 2);
  while (Wire.available()) {
    value <<= 8;
    value |= Wire.read();
  }
  Wire.endTransmission();
  return value / 1.2; // Convert to Lux
}

void BH1750_Initialise() {
  Wire.beginTransmission(BH1750_Device_Address);
  Wire.write(0x10); // set resolution to 1 Lux
  Wire.endTransmission();
  delay(200);
}

void getBH1750(void){
  if (WiFi.status() != WL_CONNECTED) { //Check WiFi connection status
    Serial.println("\nWiFi Disconected ?!!");
    ConnectToWiFi();
  }
  BH1750_Initialise();
  lux = BH1750_Read();
  //Serial.println("Light Intensity "+String(lux)+" [lx]");
}

void loop() {
  digitalWrite(LED,LOW);
  getTemperatureHumidity();
  getBH1750();
  Write_ThingSpeak_Data(String(temperature), String(humidity), String(lux));
  digitalWrite(LED,HIGH);
  delay(15000);    //Send a request every 15 seconds
}
