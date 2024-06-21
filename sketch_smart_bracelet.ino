#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RTClib.h>

#define RST_PIN 9
#define SS_PIN 10

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7
MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
RTC_DS3231 rtc;

const int tempPin = A0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  SPI.begin();
  rfid.PCD_Init();
  accel.begin();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // установить время компиляции
    rtc.adjust(DateTime(2024, 6, 15, 12, 0, 0)); // установить конкретное время
  }

  lcd.setCursor(0, 0);
  lcd.print("Smart Bracelet");
  delay(2000);
}

void loop() {
  DateTime now = rtc.now();

  // Температура
  int tempReading = analogRead(tempPin);
  float temperature = (tempReading / 1024.0) * 500.0;

  // Акселерометр
  sensors_event_t event; 
  accel.getEvent(&event);

  // RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("RFID UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    rfid.PICC_HaltA();
  }

  // Вывод на LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(" Temp: ");
  lcd.print(temperature, 1);

  lcd.setCursor(0, 1);
  lcd.print("X:");
  lcd.print(event.acceleration.x, 0); 
  lcd.print(" Y:");
  lcd.print(event.acceleration.y, 0); 
  lcd.print(" Z:");
  lcd.print(event.acceleration.z, 0); 

  delay(1000);
}
