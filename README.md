# BeoButton

This project is made for Bang & Olufsen IR eye based on PCF8574 I2C.

A small ESP8266 microcontroller is fitted and connected inside the IR eye chassis.
The microcontroller is connected to WiFi and detects when a button on the IR eye. When a button press is detected it interprets whether it is a short or long press and sends an MQTT message.
