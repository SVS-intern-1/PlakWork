#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <HTTPClient.h>

#define PIR_PIN 2 // GPIO pin connected to the PIR sensor
#define PROXIMITY_PIN 15 // GPIO pin connected to the IR proximity sensor
#define TRIG_PIN 18 // GPIO pin connected to the trigger pin of the ultrasonic sensor
#define ECHO_PIN 5 // GPIO pin connected to the echo pin of the ultrasonic sensor
#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

int counter;

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";

const char* postmanEndpoint = "https://api-dashboard-staging.siamiot.net/api/v1/poc/device/rawdata";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

struct SensorData {
  float temperature;
  float humidity;
  bool motionDetected;
  bool proximityDetected;
  int distance;
};

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(PIR_PIN, INPUT);
  pinMode(PROXIMITY_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the NTP client
  timeClient.begin();
  timeClient.setTimeOffset(-9); // Set time offset (in seconds, 3600 = UTC+1 for CET)

  // Wait for the time to be synchronized
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
  
    Serial.println("WiFi disconnected");
    delay(10000);

  } else{
    Serial.println("========================================================================================================================");
    Serial.println("Data: "+ String(counter));
    SensorData data = ReadSensors();

    unsigned long epochTime = timeClient.getEpochTime();
    String epochTimeStr = String(epochTime) + "000";

    logSensorData(data, epochTimeStr);
    sendDataToServer(data, epochTimeStr);
    delay(60000);
    counter++;

    // sendHttpPost(data.proximity, "proximity", "89e90746-15ad-4158-a0c6-2bee01a85e07");
  }
}

void logSensorData(const SensorData& data, String timestamp) {
  Serial.print("Epoch time: ");
  Serial.println(timestamp);

  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  //Serial.print("Motion Detected: ");
  //Serial.println(data.motionDetected ? "Yes" : "No");

  Serial.print("Distance: ");
  Serial.print(data.distance);
  Serial.println(" cm");
  Serial.println("========================================================================================================================\n");
}

void sendDataToServer(const SensorData& data, String timestamp) {
  StaticJsonDocument<1024> doc;
  doc["asset_uuid"] = "7d95abe7-a9c2-4014-bf53-4259705e6bce";
  doc["device_lat"] = -3.811731;
  doc["device_lng"] = 19.428835;
  doc["device_controller"] = "ESP32";
  doc["data_timestamp"] = timestamp;

  JsonArray dataArr = doc.createNestedArray("data");
  addSensorData(dataArr, "temperature", String(data.temperature));
  addSensorData(dataArr, "humidity", String(data.humidity));
  addSensorData(dataArr, "motion", data.motionDetected ? "Motion Detected" : "No Motion");
  addSensorData(dataArr, "ultrasonic", String(data.distance));

  serializeAndSendJson(doc);
}

void addSensorData(JsonArray& data, const char* type, String rawData) {
  JsonObject obj = data.createNestedObject();

  // Determine the unit and UUID based on the measurement type
  String deviceName;
  String deviceModel;
  String deviceBrand;
  String unit;
  String deviceUUID;

  if (strcmp(type, "temperature") == 0) {
    unit = "Celsius";
    deviceUUID = "253c80dc-33b7-45f5-8ac7-cea8be73f17b";
    deviceName = "Temperature Sensor";
    deviceModel = "DHT22";
    deviceBrand = "OEM";
  } else if (strcmp(type, "humidity") == 0) {
    unit = "percent(%)";
    deviceUUID = "253c80dc-33b7-45f5-8ac7-cea8be73f17b";
    deviceName = "Humidity Sensor";
    deviceModel = "DHT11";
    deviceBrand = "OEM";
  } else if (strcmp(type, "motion") == 0) {
    unit = "";  // No unit required for motion
    deviceUUID = "eeac0d04-b434-4894-a02b-90b1689c33eb";
    deviceName = "PIR Motion Sensor";
    deviceModel = "HC-SR501";
    deviceBrand = "OEM";
  } else if (strcmp(type, "ultrasonic") == 0) {
    unit = "cm";  // No unit required for motion
    deviceUUID = "4b176ad3-f521-4b5c-9392-95252b290b05";
    deviceName = "Ultrasonic Distance Sensor";
    deviceModel = "Ultrasonic";
    deviceBrand = "Generic";
  } else {
    unit = "unknown";  // Default unit if type is not recognized
    deviceUUID = "Unregcognized Device";
  }

  obj["device_measurement_type"] = type;
  obj["device_uuid"] = deviceUUID;
  obj["device_sn"] = "SS-703";
  obj["device_name"] = deviceName;
  obj["device_model"] = deviceModel;
  obj["device_brand"] = deviceBrand;
  obj["device_unit"] = unit;
  obj["raw_data"] = rawData;
}

void serializeAndSendJson(StaticJsonDocument<1024>& doc) {
  String payload;
  serializeJson(doc, payload);
  Serial.println(payload);

  HTTPClient http;
  http.begin(postmanEndpoint);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  }

  http.end();
}

SensorData ReadSensors() {
  SensorData data;
  // Read temperature and humidity
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();

  // Read PIR sensor value  
  //data.motionDetected = digitalRead(PIR_PIN) == HIGH;

  // Read proximity sensor value
  //data.proximityDetected = digitalRead(PROXIMITY_PIN) == HIGH;

  // Read ultrasonic sensor value
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  data.distance = pulseIn(ECHO_PIN, HIGH) / 58.0; // Convert time to distance in cm

  return data;
}