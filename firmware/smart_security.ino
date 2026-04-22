#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);


RTC_DS3231 rtc;


#define IR_PIN 27
#define BUZZER_PIN 26
#define BUTTON_PIN 14

bool securityMode = false;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  
  lcd.setCursor(0, 0);
  lcd.print("SMART SYSTEM");
  delay(1500);
  lcd.clear();
}

void loop() {

  DateTime now = rtc.now();

  // BUTTON LOGIC 
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    securityMode = !securityMode;

    Serial.println("BUTTON PRESSED → MODE TOGGLED");
  }

  lastButtonState = buttonState;

  // IR SENSOR
  int ir = digitalRead(IR_PIN);
  bool motion = (ir == LOW);

  // LCD DISPLAY
  lcd.setCursor(0, 0);
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print(securityMode ? "SECURITY MODE " : "NORMAL MODE   ");

  // LOGIC
  if (motion) {

    if (securityMode) {

      Serial.println("🚨 INTRUDER ALERT!");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("INTRUDER ALERT");
      lcd.setCursor(0, 1);
      lcd.print("SECURITY MODE");

      tone(BUZZER_PIN, 4000);
      delay(1000);

    } else {

      Serial.println("Motion detected (NORMAL MODE)");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Motion detected");
      lcd.setCursor(0, 1);
      lcd.print("NORMAL MODE");

      noTone(BUZZER_PIN);
      delay(1000);
    }

  } else {
    noTone(BUZZER_PIN);
  }

  // SERIAL MONITOR DEBUG 
  Serial.println("================================");

  Serial.print("TIME: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  Serial.print("MODE: ");
  Serial.println(securityMode ? "SECURITY MODE" : "NORMAL MODE");

  Serial.print("IR VALUE: ");
  Serial.println(ir);

  Serial.print("BUTTON STATE: ");
  Serial.println(buttonState == LOW ? "PRESSED" : "NOT PRESSED");

  Serial.println("================================");

  delay(500);
}
