#include <WiFi.h>
//#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "ThingSpeak.h"
#include "pinku.h"

#define SHOWDEBUG

OneWire oneWire(pin_sensor_temp);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient  client;
BlynkTimer ceknotif;

//Milis Setting
const long interval = 15500;
unsigned long before = 0;

float valph, valsuhu;

BLYNK_WRITE(V10) {
  int klik = param.asInt();
  Serial.print("BUTTON 1 :"); Serial.println(klik);

  if (klik)setON();
  else setOFF();

}
void notif() {
  if (valph < 6.50 || valph > 8.00) Blynk.notify("PH is Not 6.50 - 8");
  else if (valsuhu < 20 || valsuhu > 30) Blynk.notify("Temp is Not 20 - 30 Celcius");
}
void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(5000);
  }

  ThingSpeak.begin(client);
  Blynk.begin(myblynk, ssid, pass);
  sensors.begin ();
  ceknotif.setInterval(7000L, notif);
}

void loop() {
  unsigned long now = millis();
  //  valph = random(4, 10);
  //  valsuhu = random(27, 40);
  valph = cekPH();
  valsuhu = cekSuhu();

  #ifdef SHOWDEBUG
    Serial.print("MYPH   : ");Serial.println(valph);
    Serial.print("MYSuhu : ");Serial.println(valsuhu);
  #endif
  
  tampil(valph, valsuhu);
  
  if (now - before >= interval) {
    before = now;
    sendTS();
  }

  delay(20);
  Blynk.run();
  ceknotif.run();
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
void setON()  {
  digitalWrite(relay, LOW);
}
void setOFF()  {
  digitalWrite(relay, HIGH);
}
void tampil(float myph, float mysuhu) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nilai PH   :");
  lcd.print(myph);
  lcd.setCursor(0, 1);
  lcd.print("Nilai Suhu :");
  lcd.print(mysuhu);
  lcd.print((char)223);
  lcd.print("C");
}
float cekSuhu() {
  float val;
  sensors.requestTemperatures();
  val = sensors.getTempCByIndex(0);

  Serial.print("Suhu : "); Serial.println(val);
  return val;
}
float cekPH() {
  float voltage = 0;
  int nilaiAnalog = 0;
  float nilaiPH = 0;

  nilaiAnalog = analogRead(pin_sensor_ph);

  //rumus nilai tegangan
  voltage = nilaiAnalog * (5.0 / 1023);

  //rumus nilai ph air
  nilaiPH = 2.676 * voltage - 0.3130;;
  if (nilaiPH > 9.00) nilaiPH = nilaiPH - 0.02;
  else if (nilaiPH > 6.00) nilaiPH = nilaiPH + 0.02;
  else if (nilaiPH < 4.00) nilaiPH = nilaiPH - 0.01;

  Serial.print("Nilai Analog : ");
  Serial.print(nilaiAnalog);
  Serial.print("      Nilai Voltage : ");
  Serial.print(voltage);
  Serial.print("          Nilai PH : ");
  Serial.print(nilaiPH);

  return nilaiPH;
}
