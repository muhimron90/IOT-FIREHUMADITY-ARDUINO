// Internet Of Things
// Author : Kelompok 7




#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"
#include <Servo.h>

// Set these to run example. //
#define FIREBASE_HOST "iot-apps-2k18.firebaseio.com" //name firebase database
#define FIREBASE_AUTH "hVUH40trYhOmooj0JF8yQAp9bHMLAj7fS75H3h***" //Token
#define WIFI_SSID "g4" //BSSID
#define WIFI_PASSWORD "apasih01" //password SSID
#define DHTPIN 0 //on pin D3  
#define DHTYPE DHT11 //tipe module dht 11

//----sensor temperature---//
DHT dht(DHTPIN, DHTYPE); //  DHT11


Servo myServo; // Initial name Servo
int n = 0; //control servo state

//----sensor Flame---//
int flame = 5; //on pin D1 
bool flame1 = 0; //state flame {False}
   
int isFlame = HIGH; // HIGH when FLAME Exposed
int mytimeout = millis() / 1000; 


 
//----FanMOdule l9110---//
 int INA = 14;  // on pin D5 
 int INB = 2;  //on pin D4 
 byte speedfan = 150; //speed fan


//read Gpio or pin, view on NodeMcu Mapping Board

void setup() {
  Serial.begin(115200); 
  pinMode(flame, INPUT); //initial flamesensor on pin d1 or 5 , Flame
  delay(50);
  pinMode(D2, OUTPUT); //initial pin d2 or 4, Buzzer
  delay(50);
  myServo.attach(12);//initial pin d6 or gpio pin, Servo
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  //myServo.write(80);
  delay(0);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if((millis() / 1000) > mytimeout + 3){ // try for less than 4 seconds to connect to WiFi router
      break;
    }
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.set("SERVO STATE", 0);
   delay(200);
}



void loop() {
  
  // set value
  Firebase.setString("Communication", "Connected");

////---manual Controling servo and fan--------
 n=Firebase.getInt("SERVO STATE"); 
   if(n==1){
      servoloop();
    
      return;
    }else{
      fanStop();
      
    }
////-----------------------------------------

  temperature(); //method temperature

  flameDetect(); //method for flame and Buzzer 
  Serial.println(flame1);

 
 

  // set string value
  Firebase.setString("TUGAS", "FIRE-TEMPERATURE IOT");
   Firebase.setString("KEL", "KELOMPOK 7");
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(500);
  
}
//method Flame
void flameDetect() {
  isFlame = digitalRead(flame);
//  n=Firebase.getInt("SERVO STATE"); //use this Declaration , for Automatic 
  if (isFlame == LOW) {
    flame1 = 1;
     digitalWrite(D2, HIGH);
      
    Serial.println("FLAME, FLAME, FLAME");
      
    Firebase.setString("BUZZER", "ON");
    Firebase.setString("FLAME", "FLAME DETECTED");
    Firebase.set("BUZZER STATE", 1);
    Firebase.set("FLAME STATE", flame1);
//   servoloop(); // Auto on Servo and Fan , if flame detected or State 1
   
  } else {
    flame1 = 0;
    digitalWrite(D2, LOW);
    Serial.println("no flame");
     Firebase.setString("BUZZER", "OFF");
    Firebase.setString("FLAME", "NOT DETECTED");
       Firebase.set("BUZZER STATE", 0);
  Firebase.set("FLAME STATE", flame1);
//   fanStop(); //Auto off servo and fan , , if flame Not detected or State 0
  }
}

//method Temperature
void temperature(){
  float h = dht.readHumidity();
  float t = dht.readTemperature(); //ready temp
  delay(500);
 Firebase.setFloat("Humidity", h);
 Firebase.setFloat("Temp", t);
 if (Firebase.failed()) {
      Serial.print("setting /DHT11 failed:");
      Serial.println(Firebase.error());  
      return;
  }
//    Firebase.remove("Humidity"); auto remove data on firebasedatabase
//    Firebase.remove("Temp"); auto remove data on firebasedatabase 
  delay(500);
  return;
}

//Controling Servo Method
void servoloop(){
  int pos;
  //myServo.write(110); // write position Degree on Start
    fanOn();
     delay(105);
for(pos = 45; pos <= 115; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(25);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 115; pos>=45; pos-=1)     // goes from 115 degrees to 0 degrees 
  {                                
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(25);                       // waits 15ms for the servo to reach the position 
  } 
//  myServo.write(servo_pos);
//  if(servoMov){
//    servo_pos += servo_step;
//    if(servo_pos >=120){
//      servo_pos = 120;
//      servoMov = false;
//    }
//  }else{
//    servo_pos -= servo_step;
//     if(servo_pos <= 120){
//      servo_pos = 45;
//      servoMov = true;
//    }
//  }

}

//method Fan on
void fanOn(){
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
  delay(50); 
}
//method Fan OFF
void fanStop(){
 digitalWrite(D2, LOW); //use this if manual Control Fan servo, if auto please disable
  delay(50);  
  digitalWrite(INA, LOW); 
  digitalWrite(INB, LOW);
  delay(50);
}

