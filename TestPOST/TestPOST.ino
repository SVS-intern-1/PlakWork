#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";
const char* postmanEndpoint  = "https://iotapi.siamvayupak.com/api/v1/poc/device/rawdata";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
    HTTPClient http;  // Declare an object of class HTTPClient

    http.begin(postmanEndpoint);  // Specify request destination
    http.addHeader("Content-Type", "application/json");  // Specify content-type header

    // JSON formatted data
    String jsonData = "{\"message\": \"POSTING from ESP32\"}";
    int httpResponseCode = http.POST(jsonData);  // Send the POST request

    if (httpResponseCode > 0) {
      String response = http.getString();  // Get the response to the request
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();  // Free resources
  }
  delay(10000);  // Send a request every 10 seconds
}
