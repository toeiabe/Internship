#include <MicroGear.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <sys/time.h>

// constants won't change. They're used here to 
// set pin numbers:
//#define D0 16  // USER LED Wake
#define ledPin  D6        // the number of the LED pin
//Po 123456789
//Gunizuka fenr5717
const char* ssid     = "Gunizuka";
const char* password = "fenr5717";

#define APPID   "ClapClapLight"
#define KEY     "QtHoJZqFaNkiJaC"
#define SECRET  "ZJPigqOIjgBkGtd1pPDr17NKI"

#define ALIAS   "NodeMCU1"
#define TargetWeb "DigitalOUTPUT_HTML_web"

WiFiClient client;
MicroGear microgear(client);


int mode = 0;
double temp_t = 0;

//receive command from website via netpie
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  if(stateStr == "ON") 
  {
    digitalWrite(ledPin, HIGH);
    mode = 1;
    microgear.chat(TargetWeb, "ON");
  } 
  else if (stateStr == "OFF") 
  {
    digitalWrite(ledPin, LOW);
    mode = 0;
    microgear.chat(TargetWeb, "OFF");
  }
  else if (stateStr == "GET")
  {
    if(mode == 0){
      microgear.chat(TargetWeb, "OFF");
    }
    else{
      microgear.chat(TargetWeb, "ON");
    }
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void setup() 
{
    /* Event listener */
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
   delay(250);
   Serial.print(".");
 }

 Serial.println("WiFi connected");  
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());

 microgear.init(KEY,SECRET,ALIAS);
 microgear.connect(APPID);

 pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin,HIGH); // Turn off LED
  }

  void loop() 
  {
    if(microgear.connected()) 
    {
      microgear.loop();
      
      //Serial.println("connect...");
      
      if (Serial.available()){
        int s = Serial.read();
        struct timeval  tv;
        gettimeofday(&tv, NULL);
        double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
        Serial.print(time_in_mill);
        Serial.print("             ");
        Serial.println(s);
        int isToggle = 0;
        if(s && time_in_mill - temp_t <= 400){
          isToggle = 1;
          if(mode == 0){
            mode = 1;
            digitalWrite(ledPin, HIGH);
            microgear.chat(TargetWeb, "ON");
          }
          else{
            mode = 0;
            digitalWrite(ledPin, LOW);
            microgear.chat(TargetWeb, "ON");
          }
        }
        if (isToggle) {
          temp_t = 0;
        } else {
          temp_t = time_in_mill;
        }
      }
    } 
    else 
    {
      Serial.println("connection lost, reconnect...");
      microgear.connect(APPID);
    }
  }
