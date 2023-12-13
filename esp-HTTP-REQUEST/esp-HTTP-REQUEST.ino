#include <WiFi.h>
#include "Hdc.h"
#include "Therm.h"

const char* ssid = "teste";
const char* password = "123456789";
const char* serverName = "192.168.224.75"; // Replace with your server's address
const int serverPort = 5000; // HTTP

unsigned long currentMillis = 0;

Hdc hdc;
Therm therm_1(13);
Therm therm_2(12);
Therm therm_3(14);

String getFormattedDateTime(unsigned long milliseconds) {
  // Obter o tempo atual em segundos (dividindo por 1000 para converter de ms para s)
  unsigned long seconds = milliseconds / 1000;

  // Calcular os componentes da data e hora
  unsigned long secondsSinceEpoch = 0; // Defina aqui o tempo do Unix Epoch se necessário
  unsigned long epochSeconds = secondsSinceEpoch + seconds;
  unsigned long secondsInDay = 60 * 60 * 24; // Total de segundos em um dia
  unsigned long daysSinceEpoch = epochSeconds / secondsInDay;

  // Calcular os componentes de data e hora específicos
  unsigned long secondsInCurrentDay = epochSeconds % secondsInDay;
  unsigned long currentHour = secondsInCurrentDay / 3600;
  unsigned long currentMinute = (secondsInCurrentDay % 3600) / 60;
  unsigned long currentSecond = (secondsInCurrentDay % 3600) % 60;

  // Calcular ano, mês e dia
  unsigned long years = daysSinceEpoch / 365;
  unsigned long currentYear = 1970 + years;

  unsigned long daysThisYear = daysSinceEpoch - (years * 365);
  bool isLeapYear = (currentYear % 4 == 0 && currentYear % 100 != 0) || (currentYear % 400 == 0);

  unsigned long monthLengths[12] = {31, isLeapYear ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  unsigned long currentMonth = 0;
  unsigned long daysAccumulated = 0;

  for (unsigned long i = 0; i < 12; ++i) {
    if (daysThisYear < daysAccumulated + monthLengths[i]) {
      currentMonth = i + 1;
      break;
    }
    daysAccumulated += monthLengths[i];
  }

  unsigned long currentDay = daysThisYear - daysAccumulated + 1;

  // Criar a string formatada correspondente à data e hora
  String formattedDateTime = String(currentYear) + "-" + (currentMonth < 10 ? "0" : "") + String(currentMonth) + "-" +
                             (currentDay < 10 ? "0" : "") + String(currentDay) + "T" +
                             (currentHour < 10 ? "0" : "") + String(currentHour) + ":" +
                             (currentMinute < 10 ? "0" : "") + String(currentMinute) + ":" +
                             (currentSecond < 10 ? "0" : "") + String(currentSecond);

  return formattedDateTime;
}

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

  String currentTime = getFormattedDateTime(currentMillis);

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


  String value[3] = {String(therm_1_value, 1), String(therm_2_value, 1), String(therm_3_value, 1)};

  for (int i = 2; i < 5; i++) {
    WiFiClient client;
    if (!client.connect(serverName, serverPort)) {
      Serial.println("Connection failed");
      return;
    }
    Serial.println(i);

    String url = "/readings";


    String body = String("{\r\n  \"value\": ") + value[i - 2] + ",\r\n" +
                  "  \"time\": \"" + currentTime + "\",\r\n" +
                  "  \"sensor_id\":" + String(i) + ",\r\n" +
                  "  \"type\": \"temp\"\r\n" +
                  "}";


    String request = String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + serverName + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + body.length() + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     body;

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
  WiFiClient client_controltemp;
    if (!client_controltemp.connect(serverName, serverPort)) {
      Serial.println("Connection failed");
      return;
    }

    String url = "/readings";


    String body = String("{\r\n  \"value\": ") + sensorTemp + ",\r\n" +
                  "  \"time\": \"" + currentTime + "\",\r\n" +
                  "  \"sensor_id\":" + 1 + ",\r\n" +
                  "  \"type\": \"temp\"\r\n" +
                  "}";


    String request = String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + serverName + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + body.length() + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     body;

    client_controltemp.print(request);

    Serial.println("Sending request...");
    unsigned long timeout = millis();
    while (client_controltemp.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Timeout");
        return;
      }
    }

    // Read response from the server
    while (client_controltemp.available()) {
      String line = client_controltemp.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Request sent");
    client_controltemp.stop();
    delay(5000);
   WiFiClient client_controlHum;
    if (!client_controlHum.connect(serverName, serverPort)) {
      Serial.println("Connection failed");
      return;
    }

    String url_Hum = "/readings";


    String body_Hum = String("{\r\n  \"value\": ") + sensorHum + ",\r\n" +
                  "  \"time\": \"" + currentTime + "\",\r\n" +
                  "  \"sensor_id\":" + 1 + ",\r\n" +
                  "  \"type\": \"hum\"\r\n" +
                  "}";


    String request_Hum = String("POST ") + url_Hum + " HTTP/1.1\r\n" +
                     "Host: " + serverName + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + body_Hum.length() + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     body_Hum;

    client_controlHum.print(request_Hum);

    Serial.println("Sending request...");
    unsigned long timeout_Hum = millis();
    while (client_controlHum.available() == 0) {
      if (millis() - timeout_Hum > 5000) {
        Serial.println("Timeout");
        return;
      }
    }

    // Read response from the server
    while (client_controlHum.available()) {
      String line = client_controlHum.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Request sent");
    client_controlHum.stop();
    delay(5000);
  delay(5000); // Adjust the delay based on your requirements
}
