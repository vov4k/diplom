//Подключение библиотек
#include "DHT.h"
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".

//Обявленеи констант пинов
#define HUMANPIN A0
#define SVETPIN A1
#define WATERPIN A2
#define DHTPIN 2
#define BUTTONPIN 3
#define SVETPIND 4
#define OOPIN_ 5
#define DOPIN_ 6
#define FLAMEPIN 7
#define PSPIN_ 8
#define RST_PIN 9
#define SS_PIN 10
#define DHTTYPE DHT22

//Обявление переменых нужных для работы
long minSecsBetweenEmails = 60;
unsigned long uidDec, uidDecTemp;
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

//Обявленеи модулей
DHT dht(DHTPIN, DHTTYPE);
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  //Инициализация
  Serial.begin(9600);
  dht.begin();
  SPI.begin();
  mfrc522.PCD_Init();

  //Режимы пинов
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



}

void loop() {

  //Проверка активности сигнализации
  if (ps || rr || sigRFID || sigwater) {
    return;
  }

  //Считыванеи температуры, влажности, текущего времени и состояния кнопки для режимов освещения
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  long now = millis();
  if (digitalRead(BUTTONPIN) != button) {
    if (oo_mode) {
      oo_mode = false;
    } else {
      oo_mode = true;
    }
  }

  //Активизация пожарной сигнализации, в случае пожара
  if (Serial.readString() == "CONFIRM") {
    ps = true;
    digitalWrite(OOPIN_, HIGH);
    digitalWrite(DOPIN_, HIGH);
    digitalWrite(PSPIN_, HIGH);
  }
  if (digitalRead(FLAMEPIN) == LOW) {
    if (now > (lastSend + minSecsBetweenEmails * 1000)) {
      Serial.println("FLAME:" + String(temperature));
      lastSend = now;
    }
  }
  
  //Активизация сигнализации в случае протечки
  if (analogRead(WATERPIN) > 200) {
    sigwater = true;
    Serial.println("WATER");
    digitalWrite(PSPIN_, HIGH);
  }

  //Систма сканирования RFID ключей
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

  //Получение информации с датчика движения, в случае отвутствия людей выключаем всё освещение
  if (analogRead(HUMANPIN) > 500) {
    human = true;
  } else {
    human = false;
  }
    if (!human) {
    digitalWrite(OOPIN_, LOW);
    digitalWrite(DOPIN_, LOW);
    return;
  }

  // Отравка текужей температуры и влажности
  if (now > (lastSend_klimat + minSecsBetweenEmails * 1000)) {
    lastSend_klimat = now;
    Serial.println("ANSWERKLIMAT:" + String(temperature) + ":" + String(humidity) + ":");
  }

  //Регулирование освещености по режимам
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


  delay(500);

}
