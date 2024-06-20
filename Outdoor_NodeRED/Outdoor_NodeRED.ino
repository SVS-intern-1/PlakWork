#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define MQ_PIN 34  // Define the analog pin for the gas sensor
#define SEALEVELPRESSURE_HPA (1013.25) // Define sea-level pressure

Adafruit_BME280 bme; // I2C

int counter = 0;

// Initialize the LCD library with the I2C address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";

String URL = "https://partially-unique-haddock.ngrok-free.app/post_outdoor";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

struct SensorData {
  float temperature;
  float humidity;
  float gas;
  String gas_status;
  String bme_status;
  float pressure;
  float altitude;
};

unsigned long previousMillis = 0; // Stores the last time the HTTP POST request was sent
const unsigned long interval = 60000; // Interval for HTTP POST in milliseconds (10 minutes)
unsigned long lastDisplayChange = 0; // Stores the last time the display was changed
const unsigned long displayInterval = 5000; // Interval for display change in milliseconds (5 seconds)
int displayState = 0; // Current state of the display

void setup() {
  Serial.begin(115200);
  bool status;
  
  // default settings
  status = bme.begin(0x76);  // Initialize sensor with I2C address 0x76
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
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
  unsigned long currentMillis = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected");
    delay(10000);
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      Serial.println("========================================================================================================================");
      Serial.println("Data: " + String(counter));
      SensorData data = ReadSensors();

      unsigned long epochTime = timeClient.getEpochTime();
      String epochTimeStr = String(epochTime) + "000";

      logSensorData(data, epochTimeStr);

      createJSONarray(data);

      counter++;
    }

    // Change display content every displayInterval milliseconds
    if (currentMillis - lastDisplayChange >= displayInterval) {
      lastDisplayChange = currentMillis;
      changeDisplay(displayState);
      displayState = (displayState + 1) % 6; // Cycle through states 0 to 5
    }
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

  Serial.print("Pressure: ");
  Serial.print(data.pressure);
  Serial.println(" hPa");

  Serial.print("Gas: ");
  Serial.print(data.gas);
  Serial.println(" ppm");

  Serial.print("Altitude: ");
  Serial.print(data.altitude);
  Serial.println(" m");

  Serial.println("========================================================================================================================");
}

void changeDisplay(int state) {
  SensorData data = ReadSensors();

  lcd.clear();

  switch (state) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(data.temperature);
      lcd.print(" C");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Humidity:");
      lcd.print(data.humidity);
      lcd.print(" %");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Pressure:");
      lcd.print(data.pressure);
      lcd.print(" hPa");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Gas:");
      lcd.print(data.gas);
      lcd.print(" ppm");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("Altitude:");
      lcd.print(data.altitude);
      lcd.print(" m");
      break;
    case 5:
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(data.temperature);
      lcd.print(" H:");
      lcd.print(data.humidity);
      lcd.setCursor(0, 1);
      lcd.print("P:");
      lcd.print(data.pressure);
      lcd.print(" G:");
      lcd.print(data.gas);
      break;
  }
}

void createJSONarray(const SensorData& data) {
  StaticJsonDocument<800> doc;
  JsonArray dataArr = doc.createNestedArray("data");
  addSensorData(dataArr, "temperature", String(data.temperature), data.bme_status);
  addSensorData(dataArr, "humidity", String(data.humidity), data.bme_status);
  addSensorData(dataArr, "pressure", String(data.pressure), data.bme_status);
  addSensorData(dataArr, "gas", String(data.gas), data.gas_status);

  serializeAndSendJson(doc);
}

void addSensorData(JsonArray& dataArr, const char* measurementType, String value, String status) {
  JsonObject obj = dataArr.createNestedObject();

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
    deviceUUID = "1e185cdb-75cd-4918-91b6-9a926160908d";
    dataValue = value;
    dataUnit = "°C";
    deviceName = "Temperature Sensor";
    deviceModel = "BME280";
    deviceBrand = "Adafruit";
  } else if (strcmp(measurementType, "humidity") == 0) {
    deviceUUID = "1e185cdb-75cd-4918-91b6-9a926160908d";
    dataValue = value;
    dataUnit = "%";
    deviceName = "Humidity Sensor";
    deviceModel = "BME280";
    deviceBrand = "Adafruit";
  } else if (strcmp(measurementType, "pressure") == 0) {
    deviceUUID = "1e185cdb-75cd-4918-91b6-9a926160908d";
    dataValue = value;
    dataUnit = "hPa";
    deviceName = "Pressure Sensor";
    deviceModel = "BME280";
    deviceBrand = "Adafruit";
  } else if (strcmp(measurementType, "gas") == 0) {
    deviceUUID = "17ce9a27-48ad-49ff-8dd6-c1dd1b99d7d1";
    dataValue = value;
    dataUnit = "ppm";
    deviceName = "Smoke Gas Sensor";
    deviceModel = "MQ-2";
    deviceBrand = "Hanwei Electronics";
  }

  // Construct JSON payload
  obj["asset_type"] = assetType;
  obj["asset_uuid"] = assetUUID;
  obj["device_controller"] = deviceController;
  obj["measurement_type"] = measurementType;
  obj["device_uuid"] = deviceUUID;
  obj["device_name"] = deviceName;
  obj["device_model"] = deviceModel;
  obj["device_brand"] = deviceBrand;
  obj["rawdata"] = dataValue;
  obj["device_unit"] = dataUnit;
  obj["device_status"] = status;
}

void serializeAndSendJson(StaticJsonDocument<800>& doc) {
  // Serialize JSON to a String
  String jsonPayload;
  serializeJson(doc, jsonPayload);

  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonPayload);
  String payload = http.getString();

  Serial.print("URL : "); Serial.println(URL); 
  Serial.print("Data: "); Serial.println(jsonPayload);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------\n");
}

SensorData ReadSensors() {
  SensorData data;
  data.temperature = bme.readTemperature();
  data.humidity = bme.readHumidity();
  data.pressure = bme.readPressure() / 100.0F; // Convert Pa to hPa
  data.gas = analogRead(MQ_PIN) * (5.0 / 1023.0); // Convert ADC value to voltage (example conversion)
  data.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // Check if gas sensor reads zero
  if (data.gas == 0.0) {
    data.gas_status = "Gas sensor malfunction";
  } else {
    data.gas_status = "Gas sensor OK";
  }

  // Check if any reads failed for BME280 sensor.
  if (isnan(data.temperature) || isnan(data.humidity) || isnan(data.pressure)) {
    Serial.println("Failed to read from BME280 sensor!");
    data.bme_status = "BME280 sensor malfunction";
  } else {
    data.bme_status = "BME280 sensor OK";
  }
  
  return data;
}
