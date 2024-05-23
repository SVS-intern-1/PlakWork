#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <HTTPClient.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define MQ_PIN 34  // Analog pin connected to the MQ sensor

DHT dht(DHT_PIN, DHT_TYPE);

int counter;

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";

//const char* postmanEndpoint = "https://api-dashboard-staging.siamiot.net/api/v1/poc/device/rawdata";
String URL = "http://192.168.1.37/json_project/json_data.php";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

struct SensorData {
  float temperature;
  float humidity;
  float gas;
};

void setup() {
  Serial.begin(115200);
  dht.begin();

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
    delay(5000);
    //delay(500);
    counter++;
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

  Serial.print("Gas: ");
  Serial.print(data.gas);
  Serial.println(" ppm");

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
  addSensorData(dataArr, "gas", String(data.gas));

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
  } else if (strcmp(type, "gas") == 0) {
    unit = "ppm";  // No unit required for motion
    deviceUUID = "9c794ae7-f900-4ec3-a325-556ca7cb6a9c";
    deviceName = "Smoke Gas Sensor";
    deviceModel = "MQ-2";
    deviceBrand = "Hanwei Electronics";
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
    Serial.println(payload); // Debug print
    
    HTTPClient http;
    http.begin(URL);
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

  data.gas = analogRead(MQ_PIN) * (5.0 / 1023.0); // Convert ADC value to voltage (example conversion)

  // Check if gas sensor reads zero
  if (data.gas == 0.0) {
    Serial.println("Warning: MQ sensor reading is zero, check sensor connection.");
  }

  return data;
}