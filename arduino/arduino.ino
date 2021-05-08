
#include "DHT.h"
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".


#define HUMANPIN A0
#define SVETPIN A1
#define WATERPIN A2
#define DHTPIN 2
#define BUTTONPIN 3
//#define HUMANPIN_ 3
#define SVETPIND 4
#define OOPIN_ 5
#define DOPIN_ 6
#define FLAMEPIN 7
#define PSPIN_ 8
#define RST_PIN 9
#define SS_PIN 10
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

long minSecsBetweenEmails = 60; // 1 min
unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате
long lastSend = -60000;
long lastSend_klimat = -60000;
bool ps = false;
bool rr = false;
bool sigRFID = false;
bool sigwater = false;
bool oo_mode = false;
bool human = false;
bool button;
int time_now = 0;

DHT dht(DHTPIN, DHTTYPE);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {

  Serial.begin(9600);

  dht.begin();
  //pinMode(HUMANPIN_, OUTPUT);
  pinMode(OOPIN_, OUTPUT);
  pinMode(DOPIN_, OUTPUT);
  pinMode(PSPIN_, OUTPUT);

  pinMode(DHTPIN, INPUT);
  pinMode(WATERPIN, INPUT);
  pinMode(SVETPIND, INPUT);
  pinMode(SVETPIN, INPUT);
  pinMode(FLAMEPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);
  button = digitalRead(BUTTONPIN);

  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.

}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  long now = millis();
  if (Serial.readString() == "CONFIRM") {
    ps = true;
    digitalWrite(OOPIN_, HIGH);
    digitalWrite(DOPIN_, HIGH);
    digitalWrite(PSPIN_, HIGH);
  }
  if (analogRead(WATERPIN) > 200) {
    sigwater = true;
    digitalWrite(PSPIN_, HIGH);
  }

  if ( mfrc522.PICC_IsNewCardPresent() &&  mfrc522.PICC_ReadCardSerial()) {
    uidDec = 0;
    // Выдача серийного номера метки.
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      uidDecTemp = mfrc522.uid.uidByte[i];
      uidDec = uidDec * 256 + uidDecTemp;
    }
    if (uidDec != 1920115773) {
      sigRFID = true;
      digitalWrite(PSPIN_, HIGH);
      Serial.println("HUMAN");
    }
  }
  if (digitalRead(BUTTONPIN) != button) {
    if (oo_mode) {
      oo_mode = false;
    } else {
      oo_mode = true;
    }
  }


  if (ps || rr || sigRFID || sigwater) {
    return;
  }


  if (analogRead(HUMANPIN) > 500) {
    human = true;
    //digitalWrite(HUMANPIN_, HIGH);
  } else {
    human = false;
    //digitalWrite(HUMANPIN_, LOW);
  }
  if (!human) {
    digitalWrite(OOPIN_, LOW);
    digitalWrite(DOPIN_, LOW);
    return;
  }


  if (now > (lastSend_klimat + minSecsBetweenEmails * 1000)) {
    lastSend_klimat = now;
    Serial.println("ANSWERKLIMAT:" + String(temperature) + ":" + String(humidity));
  }
  if (oo_mode) {
    analogWrite(OOPIN_, analogRead(SVETPIN) / 4);
  } else {
    digitalWrite(OOPIN_, LOW);
    if (digitalRead(SVETPIND) == HIGH) {
      digitalWrite(DOPIN_, HIGH);
    } else {
      digitalWrite(DOPIN_, LOW);
    }
  }
  if (digitalRead(FLAMEPIN) == LOW) {
    if (now > (lastSend + minSecsBetweenEmails * 1000)) {
      Serial.println("FLAME:" + String(temperature));
      lastSend = now;
    }
  }

  delay(500);

}
