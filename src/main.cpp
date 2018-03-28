// peticiones /waketime_add?dow=6&hrs=12&min=0
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeAlarms.h>

const int __Z__ = 0;

#define DOW_INDEX 2
#define HOUR_INDEX 3
#define MIN_INDEX 4
#define INTERVAL_INDEX 5

const char* ssid = "UPAYAKUWASI";
const char* password = "nosotrxs";

AsyncWebServer server(80);

void startAction(){
  Serial.println("Alarm: - turn lights on");
  digitalWrite(2, HIGH);
}

void stopAction(){
  Serial.println("Alarm: - turn lights off");
  digitalWrite(2, LOW);
}

void clearAlarms() {
}

void removeAlarm(AlarmID_t ID) {
  Alarm.free(ID);
}

void save(String name, String data, bool clear) {
  File file = SPIFFS.open(name, clear ? "w" : "a+");
  if (!file) {
    Serial.println("file open failed");
    return;
  }
  file.println(data);
  file.close();
}

void clearData(String name) {
  save(name, "", true);
}

String getData(String name) {
  File file = SPIFFS.open(name, "r");
  String line = "";
  if (!file) Serial.println("file open failed");  // Check for errors
  while (file.available()) {
    // wdt_disable();

    // Read only the very first line
    char c = file.read();
    if (c != '\r') {
      line += String(c);
    } else {
      break;
    }
  }

  yield();
  // wdt_enable(1000);

  return line;
}

void setAlarm(timeDayOfWeek_t dow, int hour, int min, int interval) {
  int downtimeMin = min + interval;
  int downtimeHour = downtimeMin > 59 ? hour + 1 : hour;
  downtimeMin = downtimeMin > 59 ? downtimeHour - 60 : downtimeHour;
  AlarmID_t idUp = Alarm.alarmRepeat(dow, hour, min, 30, startAction);
  AlarmID_t idDown = Alarm.alarmRepeat(dow, downtimeHour, downtimeMin, 30, startAction);

  String m = idUp+","+idDown;
  m += "," + dow;
  m += "," + hour;
  m += "," + min;
  m += "," + interval;

  save("alarms", m, false);
}

void parseAndSetAlarm(String alarmStr) {
  int dow = 0;
  int hour = 0;
  int min = 0;
  int interval = 0;

  int dataIndex = 0;
  String alarmStrLeft = alarmStr.substring(0);
  while (dataIndex < 6) {
    // Index of the data separator
    alarmStrLeft = "";
    int sepIndex = alarmStrLeft.indexOf(",");
    switch (dataIndex++) {
      case DOW_INDEX:
      dow = alarmStrLeft.substring(0, sepIndex).toInt();
      break;

      case HOUR_INDEX:
      hour = alarmStrLeft.substring(0, sepIndex).toInt();
      break;

      case MIN_INDEX:
      min = alarmStrLeft.substring(0, sepIndex).toInt();
      break;

      case INTERVAL_INDEX:
      interval = alarmStrLeft.substring(0, sepIndex).toInt();
      break;

      default:
        break;
    }

    alarmStrLeft = alarmStrLeft.substring(sepIndex + 1);
  }

  clearData("alarms");
}

void startAlarms() {
  String alarms = getData("alarms");
  if (alarms == "") { return; }
  unsigned int beginIndex = 0;
  unsigned int endIndex = 0;
  bool more = true;

  while (more) {
    endIndex = alarms.indexOf("\n");
    String alarm = alarms.substring(beginIndex, endIndex - 2);
    parseAndSetAlarm(alarm);
    beginIndex = endIndex + 1;
    if (endIndex == alarms.length() - 1) {
      more = false;
    }
  }
}

void startWifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
}

void startServer() {
  server.on("/alarms", HTTP_GET, [](AsyncWebServerRequest *request) {

  });

  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {
    int H = request->arg(__Z__).toInt();
    int M = request->arg(1).toInt();
    int MM = request->arg(2).toInt();
    int DD = request->arg(3).toInt();
    int YY = request->arg(4).toInt();
    setTime(H, M, 0, MM, DD, YY);
  });

  server.on("/alarm", HTTP_GET, [](AsyncWebServerRequest *request) {
    timeDayOfWeek_t DOW = (timeDayOfWeek_t)request->arg(__Z__).toInt();
    int H = request->arg(1).toInt();
    int M = request->arg(2).toInt();
    int interval = request->arg(3).toInt();
    setAlarm(DOW, H, M, interval);
  });

  server.begin();
}

void setup(){
  Serial.begin(115200);

  startWifi();
  startServer();
  startAlarms();

  SPIFFS.begin();
  server.serveStatic("/", SPIFFS, "/");
}

void loop() {
  Alarm.delay(100);
}
