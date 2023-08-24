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
  mb.update(state == LOW);
  if(mb.isSingleClick()) {
    if (buttonType == "PLAY" || buttonType == "TIMER") pcf.digitalWrite(pin, LOW);
    publish(client, buttonType + ".SHORT");
  }
  else if (mb.isDoubleClick()) {
    pcf.digitalWrite(pin, LOW);
    publish(client, buttonType + ".DOUBLE");
  }
  else if (mb.isLongClick()) {
    if (buttonType == "PLAY" || buttonType == "TIMER") pcf.digitalWrite(pin, HIGH);
    publish(client, buttonType + ".LONG"); 
  }
}

void BeoButton::publish(PubSubClient &client, String msg) {
  msg.toCharArray(payload, msg.length() + 1);
  client.publish("beo/eye", payload);
}

void BeoButton::updateLed(PCF8574& pcf, int pin, int state) {
  pcf.digitalWrite(pin, state);  
}
