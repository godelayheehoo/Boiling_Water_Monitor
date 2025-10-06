#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <keys.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Temperature sensor setup
#define ONE_WIRE_BUS 26  // GPIO pin for DS18B20 data wire (GPIO 26)
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin not used
#define SCREEN_ADDRESS 0x3C  // Common I2C address for SSD1306

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* pushover_key = PUSHOVER_KEY;
const char* pushover_api_key = PUSHOVER_API_KEY;

const char* ssid = WIFI_SSID;
const char* password = WIFI_PW;

//constants & variables
float threshold = 98.5;
unsigned long stableStart = 0;
const unsigned long stableTime = 8000; // ms
bool boiling = false;

//function prototypes
void sendPushover(const char* title, const char* message);
void triggerMonkey();
float readTemperature();
void updateDisplay(float temperature);

void setup() {
  
  Serial.begin(115200);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
  } else {
    Serial.println("OLED display initialized");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Boiling Monitor");
    display.println("Initializing...");
    display.display();
  }
  
  // Initialize temperature sensor
  sensors.begin();
  
  // Check if sensor is connected
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 devices");
  
  if (deviceCount == 0) {
    Serial.println("No DS18B20 sensors found! Check wiring:");
    Serial.println("- Data wire to P26 (GPIO 26)");
    Serial.println("- VDD to 3.3V");
    Serial.println("- GND to GND");
    Serial.println("- 4.7kΩ resistor between data and 3.3V");
  } else {
    sensors.setResolution(TEMPERATURE_PRECISION);
    Serial.println("DS18B20 temperature sensor initialized successfully");
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected!");

    //pushover test
    sendPushover("ESP32 Test", "ESP32 is starting up!");
    Serial.println("Pushover test sent");
    // Serial.println("Triggering VoiceMonkey...");
    // triggerMonkey();
    // Serial.println("VoiceMonkey triggered");
}

void loop() {
  // Read and display temperature
  float tempC = readTemperature();
  
  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
    
    // Update OLED display
    updateDisplay(tempC);
    
    // Check for boiling
    if (tempC >= threshold) {
      if (stableStart == 0) stableStart = millis();
      if (!boiling && (millis() - stableStart) >= stableTime) {
        boiling = true;
        Serial.println("🔥 Water is boiling!");
        triggerMonkey();
        sendPushover("Boiling Alert", "The water is boiling!");
      }
    } else {
      stableStart = 0;
      boiling = false;
    }
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  delay(1000);
}

void sendPushover(const char* title, const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.pushover.net/1/messages.json");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "token=";
    postData += pushover_api_key;
    postData += "&user=";
    postData += pushover_key;
    postData += "&title=";
    postData += title;
    postData += "&message=";
    postData += message;

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      Serial.print("Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    } else {
      Serial.print("Error sending message: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

void triggerMonkey() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Using the new VoiceMonkey API v2 format
    String url = String("https://api-v2.voicemonkey.io/announcement?token=") + VM_KEY + "&device=" + VOICE_MONKEY_GROUP + "&text=the%20water%20is%20boiling";
    http.begin(url);
    int code = http.GET();
    Serial.print("VoiceMonkey Response code: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

float readTemperature() {
  sensors.requestTemperatures(); // Send command to get temperatures
  
  // Check if we have any devices
  if (sensors.getDeviceCount() == 0) {
    Serial.println("No devices found during temperature read");
    return DEVICE_DISCONNECTED_C;
  }
  
  float tempC = sensors.getTempCByIndex(0);
  
  // Additional debug info
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Device disconnected or not responding");
  }
  
  return tempC;
}

void updateDisplay(float temperature) {
  display.clearDisplay();
  
  // Title
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Boiling Monitor");
  
  // Temperature - large text
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(temperature, 1);
  display.println(" C");
  
  // Status
  display.setTextSize(1);
  display.setCursor(0, 45);
  if (temperature >= threshold) {
    if (boiling) {
      display.println("BOILING!");
    } else {
      display.println("Heating up...");
    }
  } else {
    display.println("Monitoring");
  }
  
  // Threshold indicator
  display.setCursor(0, 55);
  display.print("Target: ");
  display.print(threshold, 1);
  display.println("C");
  
  display.display();
}