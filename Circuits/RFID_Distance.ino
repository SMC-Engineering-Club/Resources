#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // You need the library "MFRC522" by GithubCommunity installed.

int sensorPin = A0;

int greenLED = 5;
int redLED = 6;
int blueLED = 7;

bool systemArmed = true;

byte masterUID[4];
bool masterSet = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  if (EEPROM.read(0) != 255) {
    for (int i = 0; i < 4; i++) {
      masterUID[i] = EEPROM.read(i);
    }
    masterSet = true;
    Serial.println("Master card loaded.");
  } else {
    Serial.println("No master card set. Scan one...");
  }
}

void loop() {

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (!masterSet) {
      for (int i = 0; i < 4; i++) {
        masterUID[i] = rfid.uid.uidByte[i];
        EEPROM.write(i, masterUID[i]);
      }
      masterSet = true;

      Serial.println("Master card saved!");
      delay(1000);
      return;
    }

    bool match = true;
    for (int i=0; i<4; i++) {
      if (rfid.uid.uidByte[i] != masterUID[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      systemArmed = !systemArmed;

      Serial.print("System is now:");
      Serial.println(systemArmed ? "ARMED" : "DISARMED");
    } else {
      Serial.println("Access denied.");
    }

    delay(1000);
  }

  int sensorValue = analogRead(sensorPin);
  sensorValue = (sensorValue + analogRead(sensorPin)) / 2;

  if (!systemArmed) {
    digitalWrite(blueLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
  } else {
    digitalWrite(blueLED, LOW);

    if (sensorValue < 600) {
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, LOW);
    } else {
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, HIGH);
    }
  }

  delay(50);
}
