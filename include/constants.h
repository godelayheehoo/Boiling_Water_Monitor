#pragma once
// Project constants
#define WORKING_NAME "BoilBuddy"
#define TEMP_DISPLAY_TIME 5000 //time to display temp or error messages on oled in milliseconds

// Temperature constantss
#define ONE_WIRE_BUS 26  // GPIO pin for DS18B20 data wire (GPIO 26)
#define TEMPERATURE_PRECISION 12

//OLED constants
#define SDA_PIN 33
#define SCL_PIN 32
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin not used
#define SCREEN_ADDRESS 0x3C  // Common I2C address for SSD1306

#define RESET_BUTTON_PIN 12  // GPIO pin for the button