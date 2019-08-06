# atmosphereDataCollection
Collect the temperature, humidity and light intensity data and upload to the thing-speak cloud server

Download the necessary librarys : 
  *DHT.h --> https://github.com/markruys/arduino-DHT/archive/master.zip 
  *TingSpeak.h --> https://www.arduinolibraries.info/libraries/thing-speak
  
 Component Used - 
  - *NODEMCU - ESP8266 Wifi Development Board
  - *BH1750 - Light Intensity Sensor Module
  - *DHT11 - Temprature and Humidity Sensor Module
  - *LED 
  
 Connection:
   *DHT11 -   
    OUT    -   Pin D0

   *BH1750 -   
     SCL    -   Pin D1
     SDA    -   Pin D2
     ADD    -   NC4
