#include <RBDdimmerESP8266.h>
#include <FirebaseArduino.h>      //firebase library
#include <ESP8266WiFi.h>          //wifi library

#define WIFI_SSID "poi"       //replace SSID with your wifi username
#define WIFI_PASSWORD "poipoi123" //replace PWD with your wifi password

#define PWM2 D2
#define PWM3 D7
#define RELAY2 D3 
#define RELAY3 D0
#define ZC2 D5
#define ZC3 D6                        

#define FIREBASE_HOST "android-test-434dd.firebaseio.com"          //link of api
#define FIREBASE_AUTH "oWXpMShRZ92mYjdviy8YW8TXQXY3ycm2bCxqbLvu"   //database secret

dimmerLampESP8266 dimmer1(PWM2, ZC2); //initialase port for dimmer 
dimmerLampESP8266 dimmer2(PWM3, ZC3);
int valPIR = 0;
int valDIM = 0;

void setup() {
  Serial.begin(115200);
  dimmer1.begin(NORMAL_MODE, ON);
  dimmer2.begin(NORMAL_MODE, ON);
  
  pinMode(PWM2,OUTPUT);
  pinMode(PWM3,OUTPUT);
  pinMode(RELAY2,OUTPUT);
  pinMode(RELAY3,OUTPUT);

  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {           //wait till connected to WiFi
    delay(100);  
    Serial.print("."); 
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   
  //connect to Database
  delay(1000);
}

void loop() {
 getDataLamp2();
 delay(1000);
 getDataLamp3();
 delay(1000);
}

void getDataLamp2(){  // for Lamp 2 - Bed Room
  String pathLamp2 = "lampu/lamp2";
  FirebaseObject object2 = Firebase.get(pathLamp2);
  String lamp2 = object2.getString("status");
  int dimmerLamp2 = object2.getInt("dimmer");
  Serial.println("status lamp2: ");
  Serial.println(lamp2);
  Serial.println("dimmer lamp2: ");
  Serial.println(dimmerLamp2);
  
  if(lamp2 == "ON"){
    digitalWrite(RELAY2, LOW);
    Serial.println("status lamp2:");
    Serial.println(lamp2);
    valDIM = dimmerLamp2;
    Serial.println("status dimer lamp2:");
    Serial.println(dimmerLamp2);
    delay(1000);
  }
  dimmer1.setPower(valDIM);
  delay(1000);
  if(lamp2 == "OFF"){
    digitalWrite(RELAY2, HIGH);
    Serial.println("status lamp2:");
    Serial.println(lamp2);
  }
}

void getDataLamp3(){  // for Lamp 3 - Living Room
  String pathLamp3 = "lampu/lamp3";
  FirebaseObject object3 = Firebase.get(pathLamp3);
  String lamp3 = object3.getString("status");
  int dimmerLamp3 = object3.getInt("dimmer");
  
  if(lamp3 == "ON"){
    digitalWrite(RELAY3, LOW);
    Serial.println("status lamp3:");
    Serial.println(lamp3);
    valDIM = dimmerLamp3;
    Serial.println("status dimmer lamp3:");
    Serial.println(dimmerLamp3);
    delay(1000);
  }
  dimmer2.setPower(valDIM);
  delay(1000);
  if(lamp3 == "OFF"){
    digitalWrite(RELAY3, HIGH); 
    Serial.println("status lamp3:");
    Serial.println(lamp3);
  }
}
