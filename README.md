# BoilBuddy - Smart Boiling Water Monitor

An ESP32-based IoT device that monitors water temperature and sends notifications when water reaches boiling point. Features a local OLED display, WiFi configuration portal, and dual notification systems through Pushover and Alexa (via VoiceMonkey).

## Features

- **Real-time Temperature Monitoring**: Uses DS18B20 digital temperature sensor for accurate readings
- **OLED Display**: SSD1306 128x64 display shows current temperature and status
- **WiFi Configuration**: Easy setup through WiFiManager captive portal
- **Dual Notifications**: 
  - Pushover mobile notifications
  - Alexa announcements via VoiceMonkey API
- **Configurable Threshold**: Set custom boiling temperature (default: 98.5°C)
- **Stable Detection**: Requires temperature to be stable for 8 seconds before triggering
- **Button Reconfiguration**: Press button to restart WiFi configuration

## Hardware Requirements

### Components
- ESP32 development board
- DS18B20 waterproof temperature sensor
- SSD1306 OLED display (128x64, I2C)
- 4.7kΩ pull-up resistor
- Push button
- Breadboard and jumper wires

### Wiring Diagram

```
ESP32 GPIO Connections:
┌─────────────────┐
│      ESP32      │
├─────────────────┤
│ GPIO 26  ────── DS18B20 Data (with 4.7kΩ pull-up to 3.3V)
│ GPIO 21  ────── OLED SDA
│ GPIO 22  ────── OLED SCL
│ GPIO 32  ────── Button (with internal pull-up)
│ 3.3V     ────── DS18B20 VDD, OLED VCC
│ GND      ────── DS18B20 GND, OLED GND, Button
└─────────────────┘
```

### DS18B20 Sensor Wiring
```
DS18B20 Pinout (looking at flat side):
┌─────────────┐
│  1  2  3    │  1: GND (Black)
└─────────────┘  2: Data (Yellow) → GPIO 26 + 4.7kΩ to 3.3V
                 3: VDD (Red) → 3.3V
```

### OLED Display Wiring
```
SSD1306 OLED:
VCC → 3.3V
GND → GND
SDA → GPIO 21
SCL → GPIO 22
```

## Software Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) installed in VS Code
- Pushover account and API keys (optional)
- VoiceMonkey account and API keys (optional)

### Installation

1. Clone or download this repository
2. Open the project folder in VS Code with PlatformIO extension
3. Install dependencies (automatically handled by PlatformIO):
   - ArduinoJson
   - DallasTemperature
   - OneWire  
   - Adafruit SSD1306
   - Adafruit GFX Library
   - WiFiManager

### Configuration

#### WiFi & Service Configuration
1. Upload the code to your ESP32
2. Connect to the "BoilBuddy-Setup" WiFi network
3. Navigate to `192.168.4.1` in your browser
4. Configure:
   - WiFi credentials
   - Pushover User Key (optional)
   - Pushover API Key (optional)
   - Boiling temperature threshold (default: 100.0°C)
   - VoiceMonkey Group Name (optional)
   - VoiceMonkey API Key (optional)

## Usage

### Initial Setup
1. Power on the device
2. If WiFi credentials aren't saved, it will create a "BoilBuddy-Setup" access point
3. Connect to the access point and configure settings
4. Device will connect to your WiFi and begin monitoring

### Normal Operation
1. Place the DS18B20 sensor in the water you want to monitor
2. The OLED display shows real-time temperature
3. When temperature reaches the threshold and remains stable for 8 seconds:
   - Pushover notification sent (if configured)
   - Alexa announcement made (if VoiceMonkey configured)
   - OLED displays "BOILING!" status

### Reconfiguration
- Press and hold the button (GPIO 32) to restart WiFi configuration mode
- Device will create the setup access point again for new configuration

## API Integration

### Pushover Setup
1. Create account at [pushover.net](https://pushover.net)
2. Note your User Key from the dashboard
3. Create an application to get an API Key
4. Enter both keys in the configuration portal

### VoiceMonkey Setup  
1. Create account at [voicemonkey.io](https://voicemonkey.io)
2. Create a "Monkey" (virtual device)
3. Note the Group Name and API Key
4. Link to your Alexa account
5. Enter credentials in the configuration portal

## Customization

### Modifying Temperature Threshold
- Change via web configuration portal, or
- Modify `boilingThreshold` variable in code (default: 98.5°C)

### Adjusting Stable Time
- Modify `stableTime` constant in code (default: 8000ms)

### Display Timeout
- Modify `TEMP_DISPLAY_TIME` in `constants.h` (default: 5000ms)

## Troubleshooting

### Temperature Sensor Issues
- Verify wiring and 4.7kΩ pull-up resistor
- Check Serial Monitor for "Temperature sensor not found" messages
- Ensure DS18B20 is not damaged

### WiFi Connection Problems
- Press button to restart configuration mode
- Check signal strength and credentials
- Verify router supports 2.4GHz (ESP32 doesn't support 5GHz)

### Display Not Working
- Verify I2C wiring (SDA: GPIO 21, SCL: GPIO 22)
- Check display address (default: 0x3C)
- Ensure adequate power supply (3.3V)

### Notification Issues
- Verify API credentials in configuration
- Check internet connectivity
- Monitor Serial output for HTTP error codes

## Code Structure

```
src/
├── main.cpp              # Main application logic
include/
├── keys.h               # Project name and constants
├── constants.h          # GPIO pins and hardware constants
platformio.ini           # Build configuration and dependencies
```

### Key Functions
- `readTemperature()`: Reads DS18B20 sensor
- `updateDisplay()`: Updates OLED with current temperature
- `sendPushover()`: Sends mobile notifications
- `triggerMonkey()`: Triggers Alexa announcements
- `ButtonHelper`: Handles debounced button input

## Development Notes

- Built with PlatformIO and Arduino framework
- Uses ESP32 DevKit board profile
- Serial monitor at 115200 baud for debugging
- Preferences library stores configuration in NVS flash
- All credentials stored securely (no hardcoded secrets)

## License

This project is open source. Feel free to modify and adapt for your needs.

## Contributing

Contributions welcome! Please test thoroughly with hardware before submitting pull requests.