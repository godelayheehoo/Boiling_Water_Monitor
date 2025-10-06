//todo: 
// - button to trigger wifi manager
// - oled to indicate various problems
// - dipswitches to control if pushover/alexa is used?


#include <Arduino.h>
// #include <WiFi.h>
#include <HTTPClient.h>
#include <keys.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>
#include <Preferences.h> 

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

byte RESET_BUTTON_PIN = 32; // GPIO pin for the button


Preferences prefs;

struct ButtonHelper{
  byte pinNumber;
  bool lastState = LOW;
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 100; // milliseconds
  
  ButtonHelper(byte pin) : pinNumber(pin) {
    lastState = LOW;
    lastDebounceTime = 0;
  }

  ButtonHelper() : pinNumber(0) {
    lastState = LOW;
    lastDebounceTime = 0;
  }
  
  void begin(){
    pinMode(pinNumber, INPUT_PULLUP);
  }

  bool justPressed() {
  bool reading = digitalRead(pinNumber);

  // Detect raw change
  if (reading != lastState) {
    lastDebounceTime = millis(); // reset timer on raw change
  }

  // If the input has been stable for > debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // We only update the stored state *after* debounce period
    static bool stableState = LOW; // assume unpressed for INPUT_PULLUP
    if (reading != stableState) {
      stableState = reading;
      if (stableState == HIGH) {  // button pressed (for INPUT_PULLUP)
        Serial.println("Button press confirmed!");
        lastState = reading;
        return true;
      }
    }
  }

  lastState = reading; // keep tracking the raw state
  return false;
}
};

ButtonHelper wifiManagerButton(RESET_BUTTON_PIN); // Properly initialize with pin 32


//constants & variables
float boilingThreshold = 98.5; // Will be loaded from preferences
unsigned long stableStart = 0;
const unsigned long stableTime = 8000; // ms
bool boiling = false;
float lastCheckTime = 0;

// Pushover keys - will be loaded from preferences or environment
String pushover_key_str;
String pushover_api_key_str;

//function prototypes
void sendPushover(const char* title, const char* message);
void triggerMonkey(const String& message);
String urlEncode(const String& str);
float readTemperature();
void updateDisplay(float temperature);



void setup() {
  
  Serial.begin(115200);

  Serial.print("Initial button state low?");
  Serial.println(digitalRead(RESET_BUTTON_PIN)==LOW);
  //setup button
  wifiManagerButton.begin();

  // Initialize preferences
  prefs.begin(WORKING_NAME, false);

  //start wifi manager
  
  
  // UNCOMMENT NEXT LINE TO CLEAR ALL SAVED WIFI CREDENTIALS
  // wifiManager.resetSettings();
  
  




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


    //pushover test
    sendPushover("ESP32 Test", "ESP32 is starting up!");
    Serial.println("Pushover test sent");
    // Serial.println("Triggering VoiceMonkey...");
    // triggerMonkey();
    // Serial.println("VoiceMonkey triggered");
}

void loop() {
  if(wifiManagerButton.justPressed()){
    WiFiManager wifiManager;
    Serial.println("Resetting settings");
    wifiManager.resetSettings();
 

    // Custom fields
  WiFiManagerParameter pushoverParam("pushover", "Pushover User Key", "", 32);
  WiFiManagerParameter pushoverApiParam("pushover_api", "Pushover API Key", "", 32);
  WiFiManagerParameter tempParam("boiltemp", "Boiling Temp (°C)", "100.0", 6); // default 100°C
  wifiManager.addParameter(&pushoverParam);
  wifiManager.addParameter(&pushoverApiParam);
  wifiManager.addParameter(&tempParam);

  if (!wifiManager.startConfigPortal("BoilBuddy-Setup")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    Serial.println("Exited WiFiManager");

  // Save WiFiManager parameters to preferences
  const char* pushoverKey = pushoverParam.getValue();
  if (strlen(pushoverKey) > 0) {
    prefs.putString("pushover", pushoverKey);
    Serial.printf("✅ Saved Pushover key: %s\n", pushoverKey);
  }
  
  const char* pushoverApiKey = pushoverApiParam.getValue();
  if (strlen(pushoverApiKey) > 0) {
    prefs.putString("pushover_api", pushoverApiKey);
    Serial.printf("✅ Saved Pushover API key: %s\n", pushoverApiKey);
  }
  
  // Load keys from preferences
  pushover_key_str = prefs.getString("pushover", "not_set");
  pushover_api_key_str = prefs.getString("pushover_api", "not_set");
  
  Serial.printf("📱 Loaded Pushover User Key: %s\n", pushover_key_str.c_str());
  Serial.printf("📱 Loaded Pushover API Key: %s\n", pushover_api_key_str.c_str());

  const char* boilTempStr = tempParam.getValue();
  if (strlen(boilTempStr) > 0) {
    float boilTemp = atof(boilTempStr);
    prefs.putFloat("boiling_temp", boilTemp);
    Serial.printf("✅ Saved boiling temperature: %.1f °C\n", boilTemp);
  }
  
  // Load boiling threshold from preferences (read once)
  boilingThreshold = prefs.getFloat("boiling_temp", 100.0);
  Serial.printf("🎯 Using boiling threshold: %.1f °C\n", boilingThreshold);

  Serial.printf("✅ Connected to Wi-Fi: %s\n", WiFi.SSID().c_str());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected!");

  String alexaMsg = WORKING_NAME;
  alexaMsg += " connected to Alexa";
  triggerMonkey(alexaMsg.c_str());
  String pushoverMsg = WORKING_NAME;
  pushoverMsg += " connected to pushover";
  sendPushover("Pushover Connected", pushoverMsg.c_str());

  }



  // Limit temperature checks to once every second
  if(millis()-lastCheckTime>=1000){
  // Read and display temperature
  float tempC = readTemperature();
  
  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
    
    // Update OLED display
    updateDisplay(tempC);
    
    // Check for boiling
    if (tempC >= boilingThreshold) {
      if (stableStart == 0) stableStart = millis();
      if (!boiling && (millis() - stableStart) >= stableTime) {
        boiling = true;
        Serial.println("🔥 Water is boiling!");
        triggerMonkey("The water is boiling!");
        sendPushover("Boiling Alert", "The water is boiling!");
      }
    } else {
      stableStart = 0;
      boiling = false;
    }
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  lastCheckTime=millis();
}



}

void sendPushover(const char* title, const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    // Check if we have valid keys
    if (pushover_key_str == "not_set" || pushover_api_key_str == "not_set") {
      Serial.println("❌ Pushover keys not configured. Skipping notification.");
      return;
    }
    
    HTTPClient http;
    http.begin("https://api.pushover.net/1/messages.json");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    Serial.printf("Using pushover api key %s & user key %s\n", pushover_api_key_str.c_str(), pushover_key_str.c_str());
    String postData = "token=";
    postData += pushover_api_key_str;
    postData += "&user=";
    postData += pushover_key_str;
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

void triggerMonkey(const String& message) {
  Serial.println("Attempting to trigger VoiceMonkey...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // URL encode the message
    String encodedMessage = urlEncode(message);
    Serial.printf("Original message: %s\n", message.c_str());
    Serial.printf("Encoded message: %s\n", encodedMessage.c_str());
    
    // Using the new VoiceMonkey API v2 format
    String url = String("https://api-v2.voicemonkey.io/announcement?token=") + VM_KEY + "&device=" + VOICE_MONKEY_GROUP + "&text=" + encodedMessage;
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

String urlEncode(const String& str) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encoded += "%20";
    } else if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  
  return encoded;
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
  if (temperature >= boilingThreshold) {
    if (boiling) {
      display.println("BOILING!");
    } else {
      display.println("Heating up...");
    }
  } else {
    display.println("Monitoring");
  }
  
  // Threshold indicator - use global variable
  display.setCursor(0, 55);
  display.print("Target: ");
  display.print(boilingThreshold, 1);
  display.println("C");
  
  display.display();
}