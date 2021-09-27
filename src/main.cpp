#include <Arduino.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define DHTPIN 4
#define DHTTYPE DHT11

const char *WIFI_SSID = "Some SSID";
const char *WIFI_PASSWORD = "Some Password";

const char *MQTT_BROKER_IP = "0.0.0.0";
const char *MQTT_CLIENT_ID = "temperature-humidity";
const char *MQTT_TEMPERATURE_TOPIC = "rooms/0/temperature";
const char *MQTT_HUMIDITY_TOPIC = "rooms/0/humidity";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMqtt(const char *brokerIp, uint16_t port = 1883) {
  Serial.print("Connecting to MQTT Broker at ");
  Serial.print(brokerIp);

  mqttClient.setServer(brokerIp, port);
  mqttClient.connect(MQTT_CLIENT_ID);

  while (!mqttClient.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to MQTT Broker");
}

void setup() {
  Serial.begin(115200);

  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
  connectToMqtt(MQTT_BROKER_IP);

  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Cast values to string
  char temperatureBuffer[50];
  char humidityBuffer[50];

  sprintf(temperatureBuffer, "%f", temperature);
  sprintf(humidityBuffer, "%f", humidity);

  Serial.print("Temperature: ");
  Serial.println(temperatureBuffer);

  Serial.print("Humidity: ");
  Serial.println(humidityBuffer);

  mqttClient.publish(MQTT_TEMPERATURE_TOPIC, temperatureBuffer);
  mqttClient.publish(MQTT_HUMIDITY_TOPIC, humidityBuffer);

  if (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker");

    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("Reconnected to MQTT Broker");
      delay(5000);
    }
  }

  mqttClient.loop();
  
  delay(5000);
}
