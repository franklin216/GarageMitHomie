#include <Arduino.h>
#include <Homie.h>
// RFID
#include "MFRC522.h"

HomieNode rfidNode("RFID-1", "sensor");

#define RST_PIN 20 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Global Timer
unsigned long previousMillis = 0;
int interval = 2000;

// Standard Functions

void setup() {
  // RFID and Console
  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  Serial.println("1... ");
  delay(1000);
  Serial.println("3... ");
  Homie_setFirmware("rfid", "1.0.0");
  Serial.println("4... ");
  rfidNode.advertise("validate").settable(verifyHandler);
  Serial.println("5... ");
  Homie.setLoopFunction(loopHandler);
  Homie.setup();
}

void loop() {
  // all loop activity is handled by Homie, to ensure connectivity
  // and prevent blocking activity that could disrupt
  // communication
  Homie.loop();
}

void loopHandler() {
//  Serial.println("7... ");
  // Look for new RFID cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    //Serial.print("scanning");
    delay(50);
    return;
  }

  // scan the cards. Put in a non-blocking delay to avoid duplicate readings
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // do non-blocking thing here

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      Serial.print("found card...");
      delay(50);
      return;
    }

    // Process card
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    }

  }


// RFID: dump a byte array as hex values to Serial, then send to validation routine.
void dump_byte_array(byte *buffer, byte bufferSize) {
  String uid;
  const long interval = 1000;

  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    uid = uid + String(buffer[i], HEX);
  }
  if(uid){
    validate(uid);
  }
}

// validate card UID by sending to server via MQTT
void validate(String uid){
  Serial.print("validating card: ");
  Serial.println(uid);
  rfidNode.setProperty("uid").send(uid);
}

// Receive response from server via MQTT
bool verifyHandler(const HomieRange& range, const String& value) {
    Serial.print("verifyHandler ");
    Serial.println(value);
    delay(250);
    if (value == "granted") {
    Serial.println("card accepted");
    //Homie.setNodeProperty(rfidNode, "validate", "granted", true);
    rfidNode.setProperty("validate").send("granted");
    delay(1000);
    //digitalWrite(PIN_GREEN, LOW);
  } else if (value == "denied") {
    Serial.print("card denied");
    rfidNode.setProperty("validate").send("denied");
    delay(1000);
  } else {
    Serial.println("unexpected response: ");
    rfidNode.setProperty("validate").send("unexpected");
    Serial.print(value);
    return false;
  }
  return true;
}
