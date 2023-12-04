#include <WiFi.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverName = "yourserver.com"; // Replace with your server's address
const int serverPort = 80; // HTTP

void setup() {
  Serial.begin(115200);
  delay(1000);

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
  float sensorValue = analogRead(A0); // Replace with your sensor reading

  WiFiClient client;
  if (!client.connect(serverName, serverPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Create the GET request with the sensor value
  String url = "/your-endpoint?value=" + String(sensorValue); // Customize your payload here
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
