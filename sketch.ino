#include <WiFi.h>
#include <HTTPClient.h>
#include "HX711.h"

// HX711 Pins
#define DOUT 16
#define CLK 4

HX711 scale;

// WiFi Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ThingSpeak Write API Key
String apiKey = "9E85DQNFNTLYS9TS" ;

// Weight Limits (kg)
float min_weight = 2.0;
float max_weight = 3.0;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("=================================");
  Serial.println("Smart Load Monitoring System");
  Serial.println("=================================");

  
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wifi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");  
  }

  Serial.println();
  Serial.println("wifi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize HX711
  scale.begin(DOUT, CLK);

  // Calibration factor
  scale.set_scale(420.0);

  // Reset current weight to zero
  scale.tare();

  Serial.println("HX711 Ready");
  Serial.println("---------------------------------");
}

void loop() {

  // Read weight
  float weight = scale.get_units(10);

  // Status values:
  // 0 = UNDERLOAD
  // 1 = NORMAL
  // 2 = OVERLOAD
  int status;

  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.print(" kg");

  if (weight > max_weight) {
    status = 2;
    Serial.print(" --> OVERLOAD!");
  }
  else if (weight < min_weight) {
    status = 0;
    Serial.print(" --> UNDERLOAD!");
  }
  else {
    status = 1;
    Serial.print(" --> NORMAL");
  }

  Serial.println();

  // Upload to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    String url =
      "https://api.thingspeak.com/update?api_key=" +
      apiKey +
      "&field1=" + String(weight, 2) +
      "&field2=" + String(status);

    Serial.println("Uploading to ThingSpeak...");
    Serial.println(url);

    http.begin(url);

    int responseCode = http.GET();

    Serial.print("ThingSpeak Response Code: ");
    Serial.println(responseCode);

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected!");
  }

  Serial.println("---------------------------------");

  delay(15000);
}