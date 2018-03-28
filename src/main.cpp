// peticiones /waketime_add?dow=6&hrs=12&min=0
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeAlarms.h>

// funciones para llamar cuando se dispara una alarma
void prenderSensorRiegoManana(){
  Serial.println("Alarm: - turn lights on");
  digitalWrite(2, HIGH);
}

void apagarSensorRiegoManana(){
  Serial.println("Alarm: - turn lights off");
  digitalWrite(2, LOW);
}

const char* ssid = "UPAYAKUWASI";
const char* password = "nosotrxs";

AsyncWebServer server(80);

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });

  SPIFFS.begin();
  server.serveStatic("/",SPIFFS,"/");

  server.on("/waketime_add", HTTP_GET, [](AsyncWebServerRequest *request){
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
    //fijamos el tiempo el Domingo 25 de marzo del 2018 a las 1:40 pm
    setTime(5,39,0,3,25,18);
    //Creamos las alarmas
    Alarm.alarmRepeat(5,40,0,prenderSensorRiegoManana);  // 8:30am cada día
    Alarm.alarmRepeat(5,42,0,apagarSensorRiegoManana);  // 8:30am cada día

  });

  server.begin();
}

void loop(){
}
