#include <DHTesp.h>
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif
DHTesp dht;

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
//#include <DHT.h>
#include <SoftwareSerial.h>
//****#include <BH1750.h>
//#include <Wire.h>
#include <math.h> 
//int BH1750address = 0x23; //setting i2c address

#define DHTPIN 12     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 11  (AM2302)
//DHT dht(DHTPIN, DHTTYPE); 
 
float hum;  //Stores humidity value
float temp; //Stores temperature value

byte buff[2];
//BH1750 lightSensor;
//********int chk;

/* ---------- DO NOT EDIT ANYTHING ABOVE THIS LINE ---------- */

//Only edit the settings in this section

/* WIFI Settings */
// Name of wifi network
const char* ssid = "wi-fi network name";

// Password to wifi network
const char* password = "wi-fi password name"; 

/* Web Updater Settings */
// Host Name of Device
const char* host = "host name";

// Path to access firmware update page (Not Neccessary to change)
const char* update_path = "/firmware";

// Username to access the web update page
const char* update_username = "admin";

// Password to access the web update page
const char* update_password = "Admin";

/* MQTT Settings */
// Topic which listens for commands
char* outTopic = "WORKSHOP_TEMP"; 
char* outTopic2 = "WORKSHOP_HUMIDITY";
char* outTopic4 = "WORKSHOP_2NDFL_LUX1";
//MQTT Server IP Address
const char* server = "host ip";

//Unique device ID 
const char* mqttDeviceID = "MK-SmartHouseDeviceBhMotionRelay"; 


/* ---------- DO NOT EDIT ANYTHING BELOW THIS LINE ---------- */

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all detection has stopped
long unsigned int pause = 100;  

//sensor variables
boolean lockLow = true;
boolean takeLowTime;  


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

//MQTT 
WiFiClient net;
MQTTClient client;

//Time Variable
unsigned long lastMillis = 0;

//Connect to WiFI and MQTT
void connect();

//Setup pins, wifi, webserver and MQTT
void setup() 
{
 
  
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  client.begin(server, net);

  connect();

  MDNS.begin(host);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);

  Serial.begin(115200);
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  delay(1000);
  
  dht.setup(12, DHTesp::DHT22); // Connect DHT sensor to GPIO 12

  
 /* lightSensor.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
 
  delay(1000);*/
//****  Wire.begin(4,5);
//****  Serial.begin(115200);//init Serail band rate
}

//Connect to wifi and MQTT
void connect() 
{
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
  }

  while (!client.connect(mqttDeviceID)) 
  {
    delay(1000);
  }

}

void loop() 
{
  // MQTT Loop
  client.loop();
  delay(10);

  // Make sure device is connected
  if(!client.connected()) 
  {
    connect();
  }

  httpServer.handleClient();

  //Sensor Detection


hum = dht.getHumidity();
  temp= dht.getTemperature();
  String thisStringhum = String(hum, HEX);
  String thisStringtemp = String(temp, HEX);
  //String thisStringco2 = String(co2ppm);
   Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(hum, 1);
  Serial.print("\t\t");
  Serial.print(temp, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temp), 1);
  Serial.print("\t\t");
// Serial.print(dht.computeHeatIndex(temp, hum, false), 1);
// Serial.print("\t\t");
 Serial.println(" ");
  //Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temp), hum, true), 1);
      client.publish(outTopic, thisStringhum); 
      client.publish(outTopic2, thisStringtemp);
      //client.publish(outTopic3, thisStringco2);
      delay(2000);


}
////////////////////////////////
/*
 int i;
  uint16_t val=0;
  BH1750_Init(BH1750address);
  delay(200);

  if(2==BH1750_Read(BH1750address))
  {
    val=((buff[0]<<8)|buff[1])/1.2;
    Serial.print(val,DEC);     
    Serial.println("[lx]"); 
  }
  delay(150); 
 
  
  /*unsigned long start_time = millis();
  uint16_t lux = lightSensor.readLightLevel();//Realizamos una lectura del sensor
  delay(50);
  unsigned long load_time = millis() - start_time;*/
  
////////////////////////////////////////

/*
  String thisStringbh = String(val);
  client.publish(outTopic4, thisStringbh);
  delay(3000);
  Serial.print(" thisStringbh: "+thisStringbh);
  
}

/*
int BH1750_Read(int address) //
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();  
  return i;
}

void BH1750_Init(int address) 
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}
*/

