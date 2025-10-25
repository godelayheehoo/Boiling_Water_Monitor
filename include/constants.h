#pragma once
// Project constants
#define WORKING_NAME "BoilBuddy"
#define TEMP_DISPLAY_TIME 5000 //time to display temp or error messages on oled in milliseconds

// Temperature constants
#ifdef SKINNY_ESP32
#define ONE_WIRE_BUS 26  // GPIO pin for DS18B20 data wire (GPIO 26)
#endif
#ifdef WIDE_ESP32
#define ONE_WIRE_BUS 23
#endif

#define TEMPERATURE_PRECISION 12

//OLED constants
#ifdef SKINNY_ESP32
#define SDA_PIN 33
#define SCL_PIN 32
#endif
#ifdef WIDE_ESP32
#define SDA_PIN 21
#define SCL_PIN 22
#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin not used
#define SCREEN_ADDRESS 0x3C  // Common I2C address for SSD1306

#ifdef SKINNY_ESP32
#define RESET_BUTTON_PIN 12  // GPIO pin for the button
#endif
#ifdef WIDE_ESP32
#define RESET_BUTTON_PIN 5
#endif