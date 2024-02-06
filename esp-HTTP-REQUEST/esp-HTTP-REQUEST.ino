#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ClosedCube_HDC1080.h>

// HDC1080  DS18B20
// SCL 22   VCC 3V3
// SDA 21   GND GND
// 3V3 3V3  DATA 4
// GND GND 

#define ONE_WIRE_BUS 4
#define uS_TO_S_FACTOR 1000000ULL  // Converts the factor to multiply with time (in seconds)
#define TIME_TO_SLEEP 300

const char* ssid = "Wickedbotz";
const char* password = "Wickedbotz";
const char* mqtt_server = "192.168.0.10";
const int mqtt_port = 1883;

RTC_DATA_ATTR int bootCount = 0;
uint32_t sleepindicator = *(volatile uint32_t*)0x3FF48018;  //RTC_CNTL_STATE0_REG Register

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);
ClosedCube_HDC1080 hdc1080;

float tempRecip, tempAmb, humidity;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Increment boot count
  bootCount++;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();

  // Setup the time to wake up after sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
}

void loop() {
  // Check if the ESP should go to sleep based on sleepindicator
  bool sleep = (sleepindicator & (1 << 31)) != 0;

  if (!sleep) {
    if (connectToWiFi() && connectToMQTT()) {
      //Call Function to read the sensors
      readSensors(&tempRecip,&tempAmb,&humidity);

      //Public in server the data
      publishSensorData(tempRecip,tempAmb,humidity);

      // Disconnect from MQTT and Wi-Fi
      client.disconnect();
      WiFi.disconnect();
    }

    // Enter deep sleep
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
    Serial.println("This message should not appear");
  }
}

bool connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.println("Connecting to WiFi... Attempt " + String(attempts));
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Check credentials or network availability.");
    return false;
  }

  Serial.println("Connected to WiFi!");
  return true;
}

bool connectToMQTT() {
  Serial.println("Connecting to MQTT server...");
  client.setServer(mqtt_server, mqtt_port);
  if (!client.connect("ESP32Client")) {
    Serial.println("Failed to connect to MQTT server.");
    return false;
  }

  Serial.println("Connected to MQTT server!");
  return true;
}

void publishSensorData(float tempRecip, float tempAmb, float humidity) {
  String mensagem = "Temperatura Recipiente: " + String(tempRecip, 2) + " Celsius\n" + "Humidade relativa: " + String(humidity, 2) + "%\n" + "Temperatura Ambiente: " + String(tempAmb, 2) + " Celsius";

  client.publish("Leitura ->", mensagem.c_str());

  Serial.print("Temperatura Recipiente: " + String(tempRecip, 2) + " ");
  Serial.print("Humidade: " + String(humidity, 2) + " ");
  Serial.println("Temperatura Ambiente: " + String(tempAmb, 2) + " ");
}

void readSensors(float* tempRecip, float* tempAmb, float* humidity) {
  //Start the sensors
  sensors.begin();
  hdc1080.begin(0x40);
  // Request temperature readings from the sensor
  sensors.requestTemperatures();
  sensors.setResolution(12);
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);

  float tempRecipSum = 0.0;
  float tempAmbSum = 0.0;
  float humiditySum = 0.0;

  //Treat variables and make a average
  for (int i = 0; i < 5; i++) {
    float tempRecipValue = sensors.getTempCByIndex(0);
    if (!isnan(tempRecipValue)) {
      tempRecipSum += tempRecipValue;
    }
    
    float tempAmbValue = hdc1080.readTemperature();
    if (!isnan(tempAmbValue)) {
      tempAmbSum += tempAmbValue;
    }

    float humidityValue = hdc1080.readHumidity();
    if (!isnan(humidityValue)) {
      humiditySum += humidityValue;
    }
  }

  // Calculate averages
  *tempRecip = tempRecipSum / 5;
  *tempAmb = tempAmbSum / 5;
  *humidity = humiditySum / 5;
}

void print_wakeup_reason() {
  // Determine the cause of the ESP wake-up
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
