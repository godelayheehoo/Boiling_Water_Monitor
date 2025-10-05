#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <keys.h>
#include <ArduinoJson.h>


const char* pushover_key = PUSHOVER_KEY;
const char* pushover_api_key = PUSHOVER_API_KEY;

const char* ssid = WIFI_SSID;
const char* password = WIFI_PW;

//function prototypes
void sendPushover(const char* title, const char* message);
void triggerMonkey();

void setup() {
  
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected!");

    //pushover test
    sendPushover("ESP32 Test", "Hello from your ESP32!");

    triggerMonkey();
}

void loop() {
  // Main loop - add your boiling monitor logic here
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
    String url = String("https://api-v2.voicemonkey.io/announcement?token=") + VM_TOKEN + "&device=" + VM_DEVICE + "&text=the%20water%20is%20boiling";
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