//todo: 
// - dipswitches to control if pushover/alexa is used?


#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>
#include <Preferences.h> 
#include <constants.h>


// Temperature sensor setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// OLED display setup
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temporary view system
unsigned long tempViewStartTime = 0;
bool tempViewActive = false;
void (*tempViewCallback)() = nullptr;

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
WiFiManager wifiManager;
 
  // Custom fields
WiFiManagerParameter pushoverParam("pushover", "Pushover User Key", "", 32);
WiFiManagerParameter pushoverApiParam("pushover_api", "Pushover API Key", "", 32);
WiFiManagerParameter tempParam("boiltemp", "Boiling Temp (°C)", "100.0", 6); // default 100°C
WiFiManagerParameter voiceMonkeyGroupParam("vm_group", "VoiceMonkey Group Name", "", 64);
WiFiManagerParameter voiceMonkeyKeyParam("vm_key", "VoiceMonkey API Key", "", 128);

//constants & variables
float boilingThreshold = 98.5; // Will be loaded from preferences
unsigned long stableStart = 0;
const unsigned long stableTime = 8000; // ms
bool boiling = false;
float lastCheckTime = 0;

// Pushover keys - will be loaded from preferences or environment
String pushover_key_str;
String pushover_api_key_str;
String vm_group_str;
String vm_key_str;

//function prototypes
void sendPushover(const char* title, const char* message);
void triggerMonkey(const String& message);
String urlEncode(const String& str);
float readTemperature();
void updateDisplay(float temperature);
void processParameters();
void wifiNotConnectedDisplay();
void pushoverFailureDisplay();
void alexaFailureDisplay();
void launchPortalDisplay();

void setup() {
  
  Serial.begin(115200);

  Serial.print("Initial button state low?");
  Serial.println(digitalRead(RESET_BUTTON_PIN)==LOW);
  //setup button
  wifiManagerButton.begin();

  // Initialize preferences
  prefs.begin(WORKING_NAME, false);

  wifiManager.addParameter(&pushoverParam);
  wifiManager.addParameter(&pushoverApiParam);
  wifiManager.addParameter(&tempParam);
  wifiManager.addParameter(&voiceMonkeyGroupParam);
  wifiManager.addParameter(&voiceMonkeyKeyParam);
  
  // UNCOMMENT NEXT LINE TO CLEAR ALL SAVED WIFI CREDENTIALS
  // wifiManager.resetSettings();
  processParameters(); // Load any previously saved parameters
  
  // Try to connect to WiFi using saved credentials (if they exist)
  if (wifiManager.getWiFiIsSaved()) {
    Serial.println("Found saved WiFi credentials, connecting...");
    WiFi.begin(); // Uses saved credentials automatically
    
    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("\n✅ Connected to Wi-Fi: %s\n", WiFi.SSID().c_str());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\n❌ Failed to connect with saved credentials");
    }
  } else {
        // Start temporary view
    tempViewCallback = wifiNotConnectedDisplay;
    tempViewStartTime = millis();
    tempViewActive = true;

    tempViewCallback(); 
    Serial.println("No saved WiFi credentials. Press button to configure.");
  }

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
    
    Serial.println("Resetting settings");
    wifiManager.resetSettings();

    // Start temporary view
    tempViewCallback = wifiNotConnectedDisplay;
    tempViewStartTime = millis();
    tempViewActive = true;
    tempViewCallback();
  

  if (!wifiManager.startConfigPortal("BoilBuddy-Setup")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    Serial.println("Exited WiFiManager");

    processParameters();

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
    // Serial.print("Temperature: ");
    // Serial.print(tempC);
    // Serial.println("°C");
    
    // Update OLED display
    if (tempViewActive && millis() - tempViewStartTime > TEMP_DISPLAY_TIME) {
    tempViewActive = false;
    updateDisplay(tempC);  // restore SD matrix view
}else if (!tempViewActive) {
    updateDisplay(tempC);
}
//do note the inelegance here.
    
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
    if(httpResponseCode!=200){
    Serial.println("Pushover error detected, showing on OLED");
    tempViewCallback = pushoverFailureDisplay;
    tempViewStartTime = millis();
    tempViewActive = true;
    tempViewCallback(); 
   
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
    String url = String("https://api-v2.voicemonkey.io/announcement?token=") +vm_key_str + "&device=" + vm_group_str + "&text=" + encodedMessage;
    http.begin(url);
    int code = http.GET();
    Serial.print("VoiceMonkey Response code: ");
    Serial.println(code);
    Serial.println(http.getString());
    if(code!=200){
      tempViewCallback = alexaFailureDisplay;
      tempViewStartTime = millis();
      tempViewActive = true;
      tempViewCallback(); 
    }
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

void wifiNotConnectedDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WiFi not connected!");
  display.println("Press button to");
  display.println("configure WiFi");
  display.display();
}

void pushoverFailureDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Pushover Error!");
  display.println("Check keys:");
  display.print("Pushover Key: ");
  display.println(pushover_key_str);
  //only print the first five characters of the api key for security
  display.print("Pushover API Key: ");
  display.print(pushover_api_key_str.substring(0,5));
  display.println("...");
  display.display();
}

void alexaFailureDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("VoiceMonkey Error!");
  display.println("Check keys:");
  display.print("VM Group: ");
  display.println(vm_group_str);
  //only print the first five characters of the api key for security
  display.print("VM Key: ");
  display.print(vm_key_str.substring(0,5));
  display.println("...");
  display.display();
}

void launchPortalDisplay(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Launching");
    display.println("Config Portal");
    display.println("@ IP");
    display.println(WiFi.softAPIP());
    display.display();
}


void processParameters(){  // Save WiFiManager parameters to preferences
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

  const char* vmGroup = voiceMonkeyGroupParam.getValue();
  if (strlen(vmGroup) > 0) {
    prefs.putString("vm_group", vmGroup);
    Serial.printf("✅ Saved VoiceMonkey Group: %s\n", vmGroup);
  }

  const char* vmKey = voiceMonkeyKeyParam.getValue();
  if (strlen(vmKey) > 0) {
    prefs.putString("vm_key", vmKey);
    Serial.printf("✅ Saved VoiceMonkey Key: %s\n", vmKey);
  }
  
  // Load keys from preferences
  pushover_key_str = prefs.getString("pushover", "not_set");
  pushover_api_key_str = prefs.getString("pushover_api", "not_set");
  vm_group_str = prefs.getString("vm_group", "not_set");
  vm_key_str = prefs.getString("vm_key", "not_set");
  
  Serial.printf("📱 Loaded Pushover User Key: %s\n", pushover_key_str.c_str());
  Serial.printf("📱 Loaded Pushover API Key: %s\n", pushover_api_key_str.c_str());
  Serial.printf("🎤 Loaded VoiceMonkey Group: %s\n", vm_group_str.c_str());
  Serial.printf("🎤 Loaded VoiceMonkey Key: %s\n", vm_key_str.c_str());

  const char* boilTempStr = tempParam.getValue();
  if (strlen(boilTempStr) > 0) {
    float boilTemp = atof(boilTempStr);
    prefs.putFloat("boiling_temp", boilTemp);
    Serial.printf("✅ Saved boiling temperature: %.1f °C\n", boilTemp);
  }
  
  // Load boiling threshold from preferences (read once)
  boilingThreshold = prefs.getFloat("boiling_temp", 100.0);
  Serial.printf("🎯 Using boiling threshold: %.1f °C\n", boilingThreshold);
}