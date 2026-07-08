#include <LiquidCrystal.h>
#include <DHT.h>
#include <Wire.h>
#include "RTClib.h"

#define DHTPIN 2
#define DHTTYPE DHT11
#define POT_PIN A0
#define GREEN_LED 3
#define RED_LED 4
#define BUZZER 5

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;

unsigned long previousPageTime = 0;
int currentPage = 0;
const unsigned long pageInterval = 3000;

int potValue = analogRead(POT_PIN);
float tempLimit = map(potValue, 0, 1023, 20, 40);

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  dht.begin();
  Wire.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.print("System Booting");
  delay(1500);
  lcd.clear();

  lcd.print("Checking RTC...");
  delay(1000);

  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC FAILED");
    Serial.println("RTC FAILED");
    while (1);
  }

  lcd.clear();
  lcd.print("RTC OK");
  Serial.println("RTC OK");
  delay(1000);

  lcd.clear();
  lcd.print("Checking DHT...");
  delay(1000);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    lcd.clear();
    lcd.print("DHT FAILED");
    Serial.println("DHT FAILED");
    while (1);
  }

  lcd.clear();
  lcd.print("DHT OK");
  Serial.println("DHT OK");
  delay(1000);

  lcd.clear();
  lcd.print("System Ready");
  Serial.println("System Ready");
  delay(1500);
  lcd.clear();
}





void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  DateTime now = rtc.now();

  int potValue = analogRead(POT_PIN);
  float tempLimit = map(potValue, 0, 1023, 20, 40);

  if (isnan(temp) || isnan(hum)) {
    lcd.clear();
    lcd.print("DHT Error");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BUZZER, HIGH);
    delay(2000);
    return;
  }

  if (millis() - previousPageTime >= pageInterval) {
    previousPageTime = millis();
    currentPage++;
    if (currentPage > 2) currentPage = 0;
    lcd.clear();
  }

  if (currentPage == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum :");
    lcd.print(hum, 0);
    lcd.print("%   ");
  }

  else if (currentPage == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Limit:");
    lcd.print(tempLimit, 0);
    lcd.print("C    ");

    lcd.setCursor(0, 1);
    if (temp >= tempLimit) {
      lcd.print("Status:ALARM ");
    } else {
      lcd.print("Status:NORMAL");
    }
  }

  else if (currentPage == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Time ");

    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour());
    lcd.print(":");
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute());
    lcd.print(":");
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second());

    lcd.setCursor(0, 1);
    lcd.print(now.day());
    lcd.print("/");
    lcd.print(now.month());
    lcd.print("/");
    lcd.print(now.year());
    lcd.print("   ");
  }

  if (temp >= tempLimit) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER, LOW);
  }

  Serial.print(now.timestamp());
  Serial.print(", Temp: ");
  Serial.print(temp);
  Serial.print(" C, Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Limit: ");
  Serial.print(tempLimit);
  Serial.println(" C");

  delay(1000);
}