README TODO:


# BoilBuddy - Smart Boiling Water Monitor

An ESP32-based IoT device that monitors water temperature and sends notifications when water reaches boiling point. Features a local OLED display, WiFi configuration portal, and dual notification systems through Pushover and Alexa (via VoiceMonkey).

# Setup Guide for Users

## What You'll Need Before Starting
- Your BoilBuddy device (already built and programmed)
- A smartphone, tablet, or computer with WiFi
- Your home WiFi network name and password
- About 10-15 minutes

## Optional: Set Up Phone Notifications (Pushover)
If you want to receive notifications on your phone when water is boiling:

1. **Download the Pushover app** on your phone from the App Store or Google Play Store. The *app* has a one time cost.  If you're one of my beta testers, feel free to let me know and I'd be happy to send you the necessary funds.
2. **Create a free account** at [pushover.net](https://pushover.net) using the same email
3. **Write down your User Key**: 
   - Log into pushover.net on your computer
   - Your User Key is displayed on the main page (looks like: `u2j3k4l5m6n7o8p9`)
   - Write this down - you'll need it later
4. **Create an application**:
   - Click "Create an Application/API Token" 
   - Give it a name like "BoilBuddy"
   - Write down the API Token (looks like: `a1b2c3d4e5f6g7h8`)

## Optional: Set Up Alexa Announcements (VoiceMonkey)
If you want Alexa to announce when water is boiling:

1. **Create a free account** at [voicemonkey.io](https://voicemonkey.io)
2. **Create a "Monkey"** (this is like a virtual device):
   - Click "Create New Monkey"
   - Give it a name like "BoilBuddy" 
   - Write down the exact name you used
3. **Get your API Key**:
   - Look for "API Key" on your dashboard
   - Write it down (looks like a long string of letters and numbers)
4. **Link to your Alexa**:
   - Follow the instructions on VoiceMonkey to connect it to your Amazon account
   - Make sure your Alexa device can see the new "monkey"

## First Time Setup - Step by Step

### Step 1: Power On Your BoilBuddy
1. **Plug in your BoilBuddy** to a USB power source (phone charger works fine)
2. **Look at the small screen** - it should show "Boiling Monitor" and "Initializing..."
3. **Wait about 10 seconds** for it to fully start up

### Step 2: Enter Setup Mode
Since this is the first time, BoilBuddy doesn't know your WiFi password yet.

1. **Find the button** on your BoilBuddy device
2. **Press and hold the button** for about 2 seconds, then let go
3. **Look at the screen** - it should show something about "Setup Mode" or "Configuration"
4. **Wait about 30 seconds** - BoilBuddy is creating its own WiFi network for setup

### Step 3: Connect Your Phone/Computer to BoilBuddy
1. **On your phone or computer**, go to WiFi settings
2. **Look for a network called "BoilBuddy-Setup"** in the list of available networks
3. **Connect to "BoilBuddy-Setup"** 
   - No password needed for this setup network
   - Don't worry if it says "No Internet" - this is normal

### Step 4: Open the Configuration Page

Depending on your device, you may automatically jump to the setup page. If not:

1. **Open your web browser** (Chrome, Safari, Firefox, etc.)
2. **Type this address**: `192.168.4.1`
3. **Press Enter** - you should see a setup page with the title "BoilBuddy Setup"

If the page doesn't load:
- Make sure you're still connected to "BoilBuddy-Setup" WiFi
- Try `http://192.168.4.1` instead
- Wait a bit longer and try again

### Step 5: Configure Your Settings
You'll see a form with several fields. Fill them out:

#### Required Settings:
1. **WiFi SSID**: Select your home WiFi network from the dropdown, or type the exact name if you don't see it
2. **WiFi Password**: Type your home WiFi password (be careful with capitals and special characters)
3. **Boiling Temp (°C)**: Leave as "100.0" unless you live at high altitude (then use 98 or 99)

#### Optional Settings (you can leave these blank if you want):
4. **Pushover User Key**: If you set up Pushover, paste your User Key here
5. **Pushover API Key**: If you set up Pushover, paste your API Token here  
6. **VoiceMonkey Group Name**: If you set up VoiceMonkey, type the exact name of your "monkey"
7. **VoiceMonkey API Key**: If you set up VoiceMonkey, paste your API Key here

### Step 6: Save and Connect
1. **Double-check your WiFi password** - this is the most common mistake!
2. **Click "Save"** at the bottom of the form
3. **Wait patiently** - BoilBuddy will try to connect to your WiFi (this takes 30-60 seconds)

#### If It Works:
- BoilBuddy will start showing the current temperature
- If you set up notifications, you should get a test message on each communication method you set up.

#### If It Doesn't Work:
- The screen will show "WiFi Failed" or similar
- Go back to Step 2 and try again
- Common fixes:
  - Check your WiFi password for typos
  - Make sure your WiFi is 2.4GHz (not 5GHz only)
  - Move BoilBuddy closer to your router
- email me (my email is written on the device).

## Using Your BoilBuddy

### Daily Use
1. **Fill a pot with water** and place it on the stove
2. **Put the temperature sensor** (the waterproof probe) into the water
3. **Turn on your BoilBuddy** if it's not already on
4. **Turn on your stove** and start heating the water
5. **Watch the screen** - it shows the current temperature in real-time
6. **When water reaches boiling** (usually around 100°C):
   - The screen will show "BOILING!"
   - You'll get a phone notification (if you set up Pushover)
   - Alexa will announce it (if you set up VoiceMonkey)

### Changing Settings Later
If you need to change your WiFi password or other settings:

1. **Press and hold the button** for 2 seconds while BoilBuddy is running
2. **Follow Steps 3-6 above** to reconfigure
3. Your old settings will be replaced with the new ones

## Troubleshooting Common Problems

### "The screen is blank"
- Check that BoilBuddy is plugged in
- Try a different USB cable or power adapter
- The screen might be very dim - look closely in good lighting

### "No temperature showing" or "Error: Could not read temperature"
- Make sure the temperature sensor (probe) is plugged in correctly
- The probe might be damaged - try gently moving the wires
- Check that the probe is actually touching the water

### "Can't connect to BoilBuddy-Setup WiFi"
- Make sure you pressed the button to enter setup mode
- Wait a full minute after pressing the button
- Move your phone/computer closer to BoilBuddy
- Try turning your phone's WiFi off and on again

### "Configuration page won't load"
- Make sure you're connected to "BoilBuddy-Setup" (not your regular WiFi)
- Try typing the address again: `192.168.4.1`
- Try adding `http://` at the beginning: `http://192.168.4.1`
- Close your browser and try again

### "WiFi connection fails"
- Double-check your WiFi password (most common issue!)
- Make sure your WiFi network is 2.4GHz (BoilBuddy can't use 5GHz)
- Move BoilBuddy closer to your router
- Try restarting your router
- Some routers block new devices - check your router settings

### "Not getting phone notifications"
- Make sure you entered both Pushover keys correctly
- Check that your phone has the Pushover app installed and logged in
- Test by sending yourself a message from the Pushover website
- Make sure BoilBuddy is connected to the internet (not just your WiFi)

### "Alexa not announcing"
- Make sure you entered the VoiceMonkey group name exactly as you created it
- Check that your Alexa account is linked to VoiceMonkey
- Try triggering a test announcement from the VoiceMonkey website
- Make sure your Alexa device is online and working

## Need More Help?
If you're still having trouble, write down exactly what's happening and what the screen shows. The more details you can provide, the easier it is to help!



# Dev Notes

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