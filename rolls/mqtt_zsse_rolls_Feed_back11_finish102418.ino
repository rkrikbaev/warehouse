




#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <EEPROM.h>

const char ssid[] = "SSE1";
const char pass[] = "terranova";
#define IN1  5 //12
#define IN2  13 //4  //13
#define IN3  4 //13 //4
#define IN4  12 //5
int Steps = 0;
int flagDOWN = 0;
int flagUP = 0;
boolean Direction =  true;// gre
boolean movUP =  false;
boolean movDOWN =  false;
unsigned long last_time;
unsigned long currentMillis ;
long steps_left;
unsigned long time1;
int i=0;
char* outTopic = "RollsFeedBack"; 

char inByte = 0;         // incoming serial byte
const int ButtonUP = 14; // вывод кнопки 0 нажата 1 нет
const int ButtonDOWN =16; // вывод кнопки 0 нажата 1 нет
String payload1 = " ";
WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
uint32_t last_millis; // переменные: последний  millis

uint8_t botton(const int buttonPin){
  if (digitalRead(buttonPin) == 1){ // кнопка не нажата     
     last_millis = millis();
     return 0;}
   delay(30);
   while (digitalRead(buttonPin) == 0);
   delay(30);
   if (last_millis+65 > millis()){ // ложное срабатывание
     //Serial.println(millis()-last_millis);
     last_millis = millis();
     return 0;}
   if (last_millis+300 > millis()){ // короткое нажатие меньше 0.30 сек
     //Serial.println(millis()-last_millis);
     last_millis = millis();
     return 1;}
   //Serial.println(millis()-last_millis);
   last_millis = millis(); // длинное нажатие больше 0.30 сек
   return 2;
};

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
pinMode(IN1, OUTPUT); 
pinMode(IN2, OUTPUT); 
pinMode(IN3, OUTPUT); 
pinMode(IN4, OUTPUT); 
pinMode(ButtonUP, INPUT_PULLUP); // вывод на ввод вверх с подтягивающим резистром
pinMode(ButtonDOWN, INPUT_PULLUP); // вывод на ввод вниз с подтягивающим резистром
 delay(1000);


if(EEPROM.read(0)==111){
  Serial.println(EEPROM.read(0));
  steps_left= EEPROMReadlong(10);
   }
   else {
     EEPROMWritelong(10, 66800);
     EEPROM.write(0,111);
    }
    EEPROM.commit();
  Serial.println(EEPROMReadlong(10));
  
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("192.168.1.74", net);
  client.onMessage(messageReceived);

  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("MK-SmartHouseDeviceBhMotionRelay1", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void loop() 
{
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
//**************************************************STEP*MOTOR***
 long address=10;
   if (Serial.available() > 0) {
  inByte = Serial.read();
  }
  flagDOWN = botton(ButtonDOWN);
  flagUP = botton(ButtonUP);
  if (inByte ==48 || payload1=="0" || flagDOWN==2) {payload1="3"; Direction = false; movDOWN = true; movUP =false; Serial.println("down");}
  if (inByte ==49 || payload1=="1" || flagUP==2 ) {payload1="3"; Direction = true; movUP =true; movDOWN = false; Serial.println("up");}
  
   if(inByte ==50  || payload1=="2")
   {
    payload1="3";
    movUP = false; 
    movDOWN =false;
    Serial.println("stop");
    
   }
  

  //*****************************************************
 while((steps_left >  0 & Direction == 0 & movDOWN ==1) ){
     
  currentMillis = micros();
  if(currentMillis-last_time>=1000){
  i++;
  stepper(1); 
  if(i==2000 && currentMillis-last_time <<100 ){ 
    i=0;    
   //String thisStringFeedback = String(steps_left);
  // client.publish(outTopic, thisStringFeedback);
  // Serial.println("1"); 
   }
   
 
 
  
  time1=time1+micros()-last_time;
  last_time=micros();
  if(digitalRead(ButtonDOWN)==1 && flagDOWN != 2){
  steps_left --;
  }
 
 
   }
    if (Serial.available() > 0) {
  inByte = Serial.read();
  }

   client.loop();
   if(inByte ==50 || payload1== "2" || botton(ButtonDOWN)==1 || botton(ButtonUP)==1 ) {payload1= "3";  movUP = false; movDOWN =false; Serial.println(steps_left);
   
   //Writing first long.
      EEPROMWritelong(address, steps_left);
      EEPROM.commit();
         String thisStringFeedback = String(steps_left);
   client.publish(outTopic, thisStringFeedback);
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   Serial.println("2");
      }
 }

 if(steps_left==0 & EEPROMReadlong(10)!=0){
   //Writing first long.
      EEPROMWritelong(address, steps_left);
      EEPROM.commit();
      String thisStringFeedback = String(steps_left);
   client.publish(outTopic, thisStringFeedback);
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   Serial.println("3");
  }
  //**********************************************************

  
 
  //******************************************************************
 // while((steps_left <  66800 && Direction == 1 && movUP ==1)|| digitalRead(ButtonUP)  ==0 ){
  while((steps_left <  66800 && Direction == 1 && movUP ==1) ){   
  currentMillis = micros();
  if(currentMillis-last_time>=1000){
i++;
 stepper(1);
 if(i==2000  && currentMillis-last_time <<100){ 
    i=0; 
    
  //String thisStringFeedback = String(steps_left);
  // client.publish(outTopic, thisStringFeedback);
  // Serial.println("4");
 } 
 
   
  
  time1=time1+micros()-last_time;
  last_time=micros();
 
  if(digitalRead(ButtonUP)==1 && flagUP !=2){
  steps_left ++;
  }
   }
   if (Serial.available() > 0) {
  inByte = Serial.read();
  }
   
    client.loop();
   if(inByte ==50 || payload1== "2" || botton(ButtonDOWN)==1 || botton(ButtonUP)==1) {payload1 = "3"; movUP = false; movDOWN =false; Serial.println(steps_left);
   
   //Writing first long.
      EEPROMWritelong(address, steps_left);
      EEPROM.commit();
         String thisStringFeedback = String(steps_left);
   client.publish(outTopic, thisStringFeedback);
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   Serial.println("5");
      }
  }  
  
 if(steps_left==66800 & EEPROMReadlong(10)!=66800){
   //Writing first long.
      EEPROMWritelong(address, steps_left);
      EEPROM.commit();
    String thisStringFeedback = String(steps_left);
   client.publish(outTopic, thisStringFeedback);
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   Serial.println("6");
  } 
}

//**************************************************STEP*MOTOR*******************
//********************************************************************************
void messageReceived(String &topic,String &payload) {
 // Serial.println("incoming: " + topic + " - " + payload);
  payload1 =payload;
}


//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

 //This function will return a 4 byte (32bit) long from the eeprom
//at the specified address to address + 3.
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }

      void stepper(int xw){
  for (int x=0;x<xw;x++){
switch(Steps){
   case 0:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   case 1:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, HIGH);
   break; 
   case 2:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   
   break; 
   case 3:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
    
   break; 
   case 4:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 5:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
     case 6:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 7:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
  
   break; 
   default:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
    
   break; 
}
SetDirection();
}
} 
void SetDirection(){
if((Direction==1 && digitalRead(ButtonDOWN)==1) || digitalRead(ButtonUP)  ==0){ Steps++;}
if((Direction==0 && digitalRead(ButtonUP)  ==1) || digitalRead(ButtonDOWN)==0 ){ Steps--; }
if(Steps>7){Steps=0;}
if(Steps<0){Steps=7;}
}
