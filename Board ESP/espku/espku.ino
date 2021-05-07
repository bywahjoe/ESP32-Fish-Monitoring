#include <WiFi.h>
#include "ThingSpeak.h"
#include "pinku.h"

WiFiClient  client;

//Milis Setting
const long interval = 15500;
unsigned long before = 0;

bool notif = false;
float valph, valsuhu;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(5000);
  }
  ThingSpeak.begin(client);
}

void loop() {
  unsigned long now = millis();
  valph = random(4, 10);
  valsuhu = random(27, 40);

  if (now - before >= interval) {
    before = now;
    sendTS();
  }
  delay(20);

}
void sendTS() {
  Serial.print("PH   : "); Serial.println(valph);
  Serial.print("SUHU : "); Serial.println(valsuhu);

  ThingSpeak.setField(1, valph);
  ThingSpeak.setField(2, valsuhu);

  int x = ThingSpeak.writeFields(myid, myapi);
  if (x == 200)    Serial.println("Channel update successful.");
  else  Serial.println("Problem updating channel. HTTP error code " + String(x));

}
