#include "BeoButton.h"
#include "PCF8574.h"
#include "PubSubClient.h"

BeoButton::BeoButton(int pcfPin, String type) {
  buttonType = type;
  pin = pcfPin;
}

BeoButton::BeoButton(String type) {
  buttonType = type;  
}

void BeoButton::update(PubSubClient &client, PCF8574& pcf, int state) {
  if (state != previousState) {

    if (state == LOW) {
      pressStart = millis();
    }
    else {
      boolean isShortPress = millis() - pressStart < 300;
      boolean isLongPress = !isShortPress && millis() - pressStart > 600;
      
      if (isShortPress) {
        pcf.digitalWrite(pin, LOW);
        publish(client, buttonType + ".SHORT");
      }
      else if (isLongPress){
        pcf.digitalWrite(pin, HIGH);
        publish(client, buttonType + ".LONG");
      }
      Serial.println(buttonType);
    }
  }
  previousState = state;
}

void BeoButton::update(PubSubClient &client, int state) {
  if (state != previousState) {

    if (state == LOW) {
      pressStart = millis();
    }
    else {
      boolean isShortPress = millis() - pressStart < 300;
      boolean isLongPress = !isShortPress && millis() - pressStart > 600;
      
      if (isShortPress) {
        publish(client, buttonType + ".SHORT");
      }
      else if (isLongPress){
        publish(client, buttonType + ".LONG");
      }
      Serial.println(buttonType);
    }
  }
  previousState = state;
}

void BeoButton::publish(PubSubClient &client, String msg) {
  msg.toCharArray(payload, msg.length() + 1);
  client.publish("beo/eye", payload);
}

void BeoButton::updateLed(PCF8574& pcf, int pin, int state) {
  pcf.digitalWrite(pin, state);  
}
