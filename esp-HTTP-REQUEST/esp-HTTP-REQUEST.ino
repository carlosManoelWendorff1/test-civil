#include <WiFi.h>
#include "Hdc.h"
#include "Therm.h"
#include "TimeFormatter.h"

const char* ssid = "Naxi_2G";
const char* password = "sebastian";
const char* serverName = "192.168.224.75";  // Replace with your server's address
const int serverPort = 5000;                // HTTP

unsigned long currentMillis = 0;

Hdc hdc;
Therm therm_1(13);
Therm therm_2(12);
Therm therm_3(14);

TimeFormatter timeFormatter;

void setup() {
  Serial.begin(115200);
  delay(1000);
  hdc.setup();
  therm_1.begin();
  therm_2.begin();
  therm_3.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  delay(1000);
}

void loop() {
  currentMillis = millis();

  String currentTime = timeFormatter.format(currentMillis);
  Serial.println(currentTime);
  float sensorTemp = hdc.getTemp();
  Serial.println(sensorTemp);
  float sensorHum = hdc.getHum();
  Serial.println(sensorHum);

  float therm_1_value = therm_1.getTemp();
  Serial.println(therm_1_value);
  float therm_2_value = therm_2.getTemp();
  Serial.println(therm_2_value);
  float therm_3_value = therm_3.getTemp();
  Serial.println(therm_3_value);


  String value[5] = {String(sensorTemp), String(sensorHum), String(therm_1_value, 1), String(therm_2_value, 1), String(therm_3_value, 1) };

  for (int i = 0; i < 5; i++) {
    WiFiClient client;
    if (!client.connect(serverName, serverPort)) {
      Serial.println("Connection failed");
      return;
    }
    Serial.println(i);

    String url = "/readings";


    String body = String("{\r\n  \"value\": ") + value[i - 2] + ",\r\n" 
      + "  \"time\": \"" + currentTime + "\",\r\n" + "  \"sensor_id\":" 
      + String(i) + ",\r\n" + "  \"type\": \"temp\"\r\n" + "}";


    String request = String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + serverName + "\r\n" 
      + "Content-Type: application/json\r\n" + "Content-Length: " 
      + body.length() + "\r\n" + "Connection: close\r\n\r\n" + body;

    client.print(request);

    Serial.println("Sending request...");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Timeout");
        return;
      }
    }

    // Read response from the server
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Request sent");
    client.stop();
    delay(5000);
  }
  delay(5000);  // Adjust the delay based on your requirements
}
