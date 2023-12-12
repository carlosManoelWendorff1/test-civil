#include <WiFi.h>
#include "Hdc.h"

const char* ssid = "WickedBotz";
const char* password = "wickedbotz";
const char* serverName = "192.168.0.127"; // Replace with your server's address
const int serverPort = 5000; // HTTP

Hdc hdc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  hdc.setup();
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
  float sensorTemp = hdc.getTemp();
  Serial.println(sensorTemp);
  float sensorHum = hdc.getHum();
  Serial.println(sensorHum);

  WiFiClient client;
  if (!client.connect(serverName, serverPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Create the GET request with the sensor value
  String url = "/meters"; // Customize your payload here
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + serverName + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Sending request...");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Timeout");
      client.stop();
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

  delay(5000); // Adjust the delay based on your requirements
}
