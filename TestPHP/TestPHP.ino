#include <WiFi.h>
#include <HTTPClient.h>

#include <DHT.h> 
#define MQ_PIN 34  // Analog pin connected to the MQ sensor
#define DHTPIN 4
#define DHTTYPE DHT11 
DHT dht11(DHTPIN, DHTTYPE); 

String URL = "http://192.168.1.37//test_project/test_data.php";

const char* ssid = "SIAMVAYUPAK_2G";
const char* password = "SVS#131266";

float temperature = 0;
float humidity = 0;
float gas = 0;

void setup() {
  Serial.begin(115200);

  dht11.begin(); 
  
  connectWiFi();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  Load_DHT11_Data();
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&gas=" + String(gas);
  
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.print("URL : "); Serial.println(URL); 
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------");
  delay(10000);
}


void Load_DHT11_Data() {
  //-----------------------------------------------------------
  temperature = dht11.readTemperature(); //Celsius
  humidity = dht11.readHumidity();
  gas = analogRead(MQ_PIN) * (5.0 / 1023.0); // Convert ADC value to voltage (example conversion)

  // Check if gas sensor reads zero
  if (gas == 0.0) {
    Serial.println("Warning: MQ sensor reading is zero, check sensor connection.");
  }
  //-----------------------------------------------------------
  // Check if any reads failed.
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0;
    humidity = 0;
  }
  //-----------------------------------------------------------
  Serial.printf("Temperature: %.2f °C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Gas: %.2f ppm\n", gas);
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("connected to : "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}