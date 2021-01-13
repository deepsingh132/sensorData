#include <ESP8266WiFi.h>
#include "WiFiClient.h"
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID "📶"
#define WIFI_PASSWORD "alphaomega1234"

#define FIREBASE_HOST "nodemcu-7e355-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Yx5IXyORTDU0KiJd0Kn32upc1m8vscBq5I0qBzNX"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

unsigned long previousMillis = 0;
const long interval = 60000;


void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  Serial.println("WiFi is down");
  delay(2000);

  WiFi.forceSleepWake();
  delay(1);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.setTimeOffset(28800);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  long v = 0;
  unsigned long currentMillis = millis();

  while (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    v = getSensorData();

    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    String t = timeClient.getFormattedTime();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["timestamp"] = t;
    root["value"] = v;

    String data = Firebase.push("/sensorData", root);
    if (Firebase.failed()) {
      Serial.print("Failed to push data to Firebase");
      Serial.println(Firebase.error());
      return;
    }
    else {
      Serial.print("Successfully pushed data to Firebase");
      Serial.println(data);
    }
  }
}
long getSensorData() {
  int N = 6;
  int avgVal=0,valSum=0;
  int readings[N];
  long long sensorValues;

  for (int i = 0; i < N; i++) {
    sensorValues=random(1000);
    readings[i] = sensorValues;
    //delay(1000);
  }

  for (int j = 0; j < N; j++) {
    valSum = valSum + readings[j];
    Serial.println(readings[j]);
  }

  Serial.println(valSum);
  avgVal = valSum / N;
  Serial.println(avgVal);

  return avgVal;
}
