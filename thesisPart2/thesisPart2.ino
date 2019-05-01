#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define WIFI_SSID "poi"       //replace SSID with your wifi username
#define WIFI_PASSWORD "poipoi123" //replace PWD with your wifi password

#define LED1 D9 // pin0
#define LED4 D4 // pin2
#define PIR4 D8 // pin15                      

#define FIREBASE_HOST "android-test-434dd.firebaseio.com"          //link of api
#define FIREBASE_AUTH "oWXpMShRZ92mYjdviy8YW8TXQXY3ycm2bCxqbLvu"   //database secret

#define NTP_OFFSET   7 * 3600     // UTC +7 Jakarta
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "id.pool.ntp.org" // pool ntp Indonesia

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

int valPIR = 0;
int valDIM = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED1,OUTPUT);
  pinMode(LED4,OUTPUT);            //define pinmodes
  pinMode(PIR4, INPUT);

  digitalWrite(D10,HIGH);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {           //wait till connected to WiFi
    delay(100);  
    Serial.print("."); 
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);             //connect to Database
  delay(1000);

  timeClient.begin();
}

void loop() {
 timeClient.update();
 delay(500);
 getDataLamp1();
 delay(500);
 setPIRLamp4();
 delay(500);
}

void callLamp(String lamp1){
  if (lamp1 == "ON"){
    Serial.println("INI ON");
    digitalWrite(LED1, LOW);
    
  } else if(lamp1 == "OFF"){
    Serial.println("INI OFF");
    digitalWrite(LED1, HIGH);
  }
}

void getDataLamp1(){   // for Lamp1 - Terrace
  String pathLamp1 = "lampu/lamp1";
  FirebaseObject object1 = Firebase.get(pathLamp1);
  String lamp1 = object1.getString("status");
  int onHourLamp1 = object1.getInt("onHour");
  int onMinLamp1 = object1.getInt("onMin");
  int offHourLamp1 = object1.getInt("offHour");
  int offMinLamp1 = object1.getInt("offMin");
  String timerStatusOn = object1.getString("timerStatusOn");
  String timerStatusOff = object1.getString("timerStatusOff");
  int durationLamp1 = object1.getInt("durationOn");
  Serial.println("status lamp1: ");
  Serial.println(lamp1);
  callLamp(lamp1);

  //TIMER ON
  if(lamp1 == "OFF" && timerStatusOn == "ON" && onHourLamp1 == timeClient.getHours() && onMinLamp1 == timeClient.getMinutes()){
    String afterStatus = pathLamp1 + "/status";
    Firebase.setString(afterStatus, "ON");
    String afterTimerStatus = pathLamp1 + "/timerStatusOn";
    Firebase.setString(afterTimerStatus, "OFF");
    callLamp(lamp1);
  }
  
  //TIMER OFF
  if(lamp1 == "ON" && timerStatusOff == "ON" && offHourLamp1 == timeClient.getHours() && offMinLamp1 == timeClient.getMinutes()){
    String afterStatus = pathLamp1 + "/status";
    Firebase.setString(afterStatus, "OFF");
    String afterTimerStatus = pathLamp1 + "/timerStatusOff";
    Firebase.setString(afterTimerStatus, "OFF");
    callLamp(lamp1);
  }
}

void setPIRLamp4(){   // for Lamp 4 - Closet Room
  String pathLamp4 = "lampu/lamp4";
  FirebaseObject object4 = Firebase.get(pathLamp4);
  String lamp4 = object4.getString("status");
  String pirStatus = object4.getString("pirStatus");
  
  if(pirStatus == "ON"){
      valPIR = digitalRead(PIR4);
      if(valPIR == HIGH){
        digitalWrite(LED4, LOW);
        String path = pathLamp4 + "/status";
        Firebase.setString(path, "ON"); 
      }
      
      if(valPIR == LOW){
        digitalWrite(LED4, HIGH);
        String path = pathLamp4 + "/status";
        Firebase.setString(path, "OFF"); 
      }
  }
  if(pirStatus == "OFF"){
    digitalWrite(LED4, HIGH);
    String path = pathLamp4 + "/status";
    Firebase.setString(path, "OFF"); 
  }
}
