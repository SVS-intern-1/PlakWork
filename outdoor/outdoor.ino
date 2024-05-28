#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <HTTPClient.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define MQ_PIN 34  // Define the analog pin for the gas sensor
#define RAIN_SENSOR_PIN 35

DHT dht(DHT_PIN, DHT_TYPE);

int counter;

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";

String URL = "http://192.168.1.37/iot_project/outdoor.php";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

struct SensorData {
  float temperature;
  float humidity;
  float gas;
  float rain;
  String gas_status;
  String dht_status;
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
  timeClient.setTimeOffset(-9); // Set time offset (in seconds, 9 hours for JST)

  // Wait for the time to be synchronized
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected");
    delay(10000);
  } else {
    Serial.println("========================================================================================================================");
    Serial.println("Data: " + String(counter));
    SensorData data = ReadSensors();

    unsigned long epochTime = timeClient.getEpochTime();
    String epochTimeStr = String(epochTime) + "000";

    logSensorData(data, epochTimeStr);

    // Perform HTTP POST requests
    sendHttpPost(data.temperature, "temperature", data.dht_status);
    sendHttpPost(data.humidity, "humidity", data.dht_status);
    sendHttpPost(data.gas, "gas", data.gas_status);
    counter++;
    delay(600000);
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

  Serial.println("========================================================================================================================");
}

void sendHttpPost(float value, const char* measurementType, String status) {
  String assetType = "Outdoor";
  String assetUUID = "c13c4814-3e85-4459-8470-f9a0746eb798";
  String deviceController = "ESP32";
  String deviceUUID;
  String dataValue;
  String dataUnit;
  String deviceName;
  String deviceModel;
  String deviceBrand;

  if (strcmp(measurementType, "temperature") == 0) {
    deviceUUID = "d5013617-0cd6-4f90-a7ee-0c775b06c404";
    dataValue = String(value);
    dataUnit = "°C";
    deviceName = "Temperature Sensor";
    deviceModel = "DHT11";
    deviceBrand = "OEM";
  } else if (strcmp(measurementType, "humidity") == 0) {
    deviceUUID = "d5013617-0cd6-4f90-a7ee-0c775b06c404";
    dataValue = String(value);
    dataUnit = "%";
    deviceName = "Humidity Sensor";
    deviceModel = "DHT11";
    deviceBrand = "OEM";
  } else if (strcmp(measurementType, "gas") == 0) {
    deviceUUID = "17ce9a27-48ad-49ff-8dd6-c1dd1b99d7d1";
    dataValue = String(value);
    dataUnit = "ppm";
    deviceName = "Smoke Gas Sensor";
    deviceModel = "MQ-2";
    deviceBrand = "Hanwei Electronics";
  }

  String postData = "asset_type=" + assetType + "&asset_uuid=" + assetUUID + "&device_controller=" + deviceController + "&measurement_type=" + measurementType + "&device_uuid=" + deviceUUID + "&device_name=" + deviceName + "&device_model=" + deviceModel + "&device_brand=" + deviceBrand + "&rawdata=" + dataValue + "&device_unit=" + dataUnit + "&device_status=" + status;
  
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.print("URL : "); Serial.println(URL); 
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------\n");

  //delay(5000);
}

SensorData ReadSensors() {
  SensorData data;
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  data.gas = analogRead(MQ_PIN) * (5.0 / 1023.0); // Convert ADC value to voltage (example conversion)
  data.rain = analogRead(RAIN_SENSOR_PIN) * (3.3 / 4095.0); // Convert ADC value to voltage (assuming 3.3V ADC reference)

  // Check if gas sensor reads zero
  if (data.gas == 0.0) {
    data.gas_status = "Gas sensor malfunction";
  } else {
    data.gas_status = "Gas sensor OK";
  }
  //-----------------------------------------------------------
  // Check if any reads failed.
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    data.dht_status = "Temperature and humidity sensor malfunction";
  } else {
    data.dht_status = "DHT sensor OK";
  }

  return data;
}
