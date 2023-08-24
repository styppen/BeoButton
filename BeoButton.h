/*
  BeoButton.h - Library for IR eye button handling.
  Created by styppen.
*/

#include "Arduino.h"
#include "PCF8574.h"
#include "PubSubClient.h"
#include "MultiButton.h"

class BeoButton
{
  public:
    BeoButton(int pcfPin, String type);
    BeoButton(String type);

    void update(PubSubClient &client, PCF8574& pcf, int state);
    static void updateLed(PCF8574& pcf, int pin, int state);
    
  private:
    String msg;
    char payload[20];
 
    String buttonType;
    int pin;
    MultiButton mb;

    void publish(PubSubClient &client, String msg);
};
