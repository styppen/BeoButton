/*
 * This project is made for Bang & Olufsen IR eye (I2C based).
 * When a button is pressed an MQTT message is sent to the MQTT broker. 
 * The subscribers pickup the message and act according to the button pressed.
 * 
 * The code can distinguish between short and long pressed for every button.
 * When buttons PLAY and TIMER are pressed, the LED on the button is enabled.
 */

#include "Arduino.h"
#include "PCF8574.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "BeoButton.h"

// B&O IR Eye I2C pin mapping
#define TIMER_BUTTON_PIN 0
#define UP_BUTTON_PIN    1
#define DOWN_BUTTON_PIN  2
#define PLAY_BUTTON_PIN  3
#define PLAY_LED_PIN     6
#define TIMER_LED_PIN    7
#define DEVICE_ADDRESS   0x20
#define MAX_MSG_LEN 128

// Set i2c address
PCF8574 pcf(DEVICE_ADDRESS);

// Wifi configuration
const char* ssid = "Grenier";
const char* password = "geslogeslo";

// MQTT Configuration
const IPAddress serverIPAddress(192, 168, 1, 30);
const char *topic = "beo/eye";

WiFiClient espClient;
PubSubClient client(espClient);

BeoButton btnTimer(TIMER_LED_PIN, "TIMER");
BeoButton btnUp("UP");
BeoButton btnDown("DOWN");
BeoButton btnPlay(PLAY_LED_PIN, "PLAY");

void setup()
{
  Serial.begin(115200);

  pcf.pinMode(TIMER_BUTTON_PIN, INPUT_PULLUP);
  pcf.pinMode(UP_BUTTON_PIN,    INPUT_PULLUP);
  pcf.pinMode(DOWN_BUTTON_PIN,  INPUT_PULLUP);
  pcf.pinMode(PLAY_BUTTON_PIN,  INPUT_PULLUP);

  pcf.pinMode(TIMER_LED_PIN, OUTPUT);
  pcf.pinMode(PLAY_LED_PIN, OUTPUT);

  pcf.begin();

  connectWifi();

  client.setServer(serverIPAddress, 1883);
  client.setCallback(callback);
  
  Serial.println("I'm alive");
}

void loop()
{

  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  
  // read all button states
  uint8_t timerVal = pcf.digitalRead(TIMER_BUTTON_PIN);
  uint8_t upVal    = pcf.digitalRead(UP_BUTTON_PIN);
  uint8_t downVal  = pcf.digitalRead(DOWN_BUTTON_PIN);
  uint8_t playVal  = pcf.digitalRead(PLAY_BUTTON_PIN);

  btnTimer.update(client, pcf, timerVal);
  btnUp.update(client, pcf, upVal);
  btnDown.update(client, pcf, downVal);
  btnPlay.update(client, pcf, playVal);
  
  delay(40);
}

void connectWifi() {
  delay(10);
  // Connecting to a WiFi network
  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  
  boolean current = HIGH;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    BeoButton::updateLed(pcf, TIMER_LED_PIN, current);
    current = !current;
    delay(250);
  }

  BeoButton::updateLed(pcf, TIMER_LED_PIN, HIGH);
  Serial.println("");
  Serial.print("WiFi connected on IP address ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  String clientId = "BeoEye1";
  boolean current = HIGH;

  while (!client.connected()) {
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());  
      
    if (client.connect(clientId.c_str())) {
      
      Serial.println("MQTT connected");
      client.publish(topic, "Hello from BeoEye");
      // ... and resubscribe
      client.subscribe(topic);
    }
    else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      BeoButton::updateLed(pcf, PLAY_LED_PIN, current);
      current = !current;
      delay(500);
    }
  }
  BeoButton::updateLed(pcf, PLAY_LED_PIN, HIGH);
}

void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength) {
  // copy payload to a static string
  static char message[MAX_MSG_LEN+1];
  if (msgLength > MAX_MSG_LEN) {
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char *)msgPayload, msgLength);
  message[msgLength] = '\0';
  
  Serial.printf("topic %s, message received: %s\n", topic, message);

  if (strcmp(message, "TIMER.LED.ON") == 0) {
    Serial.println("TIMER.ON command received");
    BeoButton::updateLed(pcf, TIMER_LED_PIN, LOW);
  }
  else if (strcmp(message, "TIMER.LED.OFF") == 0) {
    Serial.println("TIMER.OFF command received");
    BeoButton::updateLed(pcf, TIMER_LED_PIN, HIGH);
  }
  else if (strcmp(message, "PLAY.LED.ON") == 0) {
    BeoButton::updateLed(pcf, PLAY_LED_PIN, LOW);
  }
  else if (strcmp(message, "PLAY.LED.OFF") == 0) {
    BeoButton::updateLed(pcf, PLAY_LED_PIN, HIGH);
  }
}
