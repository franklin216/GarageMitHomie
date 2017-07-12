#include <Arduino.h>

/*
 * RFID Scanner with audio and lights
 *
 * Communication via WiFi & MQTT using Homie.h
 * The ID will be sent to the MQTT topic.
 * When a success verification occurs, the lights and speaker will be activated
 * depending on the value.
 *
 *
 * Written by Cory Guynn
 * www.InternetOfLEGO.com
 * 2016
 *
 */


// Networking - MQTT using Homie
// https://github.com/marvinroger/homie/tree/master
#include <Homie.h>
HomieNode rfidNode("RFID-1", "sensor");

// Audio
#include "pitches.h"

// RFID
#include "MFRC522.h"
#define RST_PIN 20 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Global Timer
unsigned long previousMillis = 0;
int interval = 2000;
// LEDs and Speaker
//const int PIN_RESET = 0; //D3 WeMos ~ This pin will flash WeMos when held low 5s
//const int PIN_RED = 16;    //D0 WeMos
//const int PIN_GREEN = 15;  //D8 WeMos
//const int PIN_SPEAKER = 5;  //D1 WeMos

// Standard Functions

void setup() {

  // this replaces the traditional "setup()" to ensure connecitvity and handle OTA

  // RFID and Console
  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  Serial.println("1... ");
  // sound beep
  //tone(PIN_SPEAKER,NOTE_C1,250);
  //delay(50);
  //tone(PIN_SPEAKER,NOTE_E1,250);
  //delay(50);
  //tone(PIN_SPEAKER,NOTE_C1,250);
  //delay(50);
  //noTone(PIN_SPEAKER);
Serial.println("2... ");
  // initialize LEDs
  //pinMode(PIN_RED, OUTPUT);
  //pinMode(PIN_GREEN, OUTPUT);
  //digitalWrite(PIN_RED, HIGH);
  //digitalWrite(PIN_GREEN, HIGH);
  delay(1000);
  //digitalWrite(PIN_GREEN, LOW);

Serial.println("3... ");
  Homie_setFirmware("rfid", "1.0.0");
  Serial.println("4... ");
  //Homie.registerNode(rfidNode);
   rfidNode.advertise("validate").settable(verifyHandler);
   Serial.println("5... ");
  Homie.setLoopFunction(loopHandler);
  //rfidNode.subscribe("validate", verifyHandler);
Serial.println("6... ");
  Homie.setup();
}

void loop() {
//  Serial.println("loop... ");
  // all loop activity is handled by Homie, to ensure connectivity and prevent blocking activity that could disrupt communication
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

  // set RFID uid property to be sent via MQTT
  //Homie.setNodeProperty(rfidNode, "uid", uid, true);
rfidNode.setProperty("uid").send(uid);
  // Turn both LEDs on
  //digitalWrite(PIN_RED, HIGH);
  //digitalWrite(PIN_GREEN, HIGH);

  // play sounds
  //tone(PIN_SPEAKER,NOTE_D1,50);
//  delay(20);
//  tone(PIN_SPEAKER,NOTE_D1,50);
//  noTone(PIN_SPEAKER);
  //digitalWrite(PIN_GREEN, LOW);

}

// Receive response from server via MQTT
bool verifyHandler(const HomieRange& range, const String& value) {
    Serial.print("verifyHandler ");
    Serial.println(value);
  //  tone(PIN_SPEAKER,NOTE_C3,100);
    //digitalWrite(PIN_RED, LOW);
    //digitalWrite(PIN_GREEN, LOW);
    delay(250);
  if (value == "granted") {
    Serial.println("card accepted");
    //Homie.setNodeProperty(rfidNode, "validate", "granted", true);
    rfidNode.setProperty("validate").send("granted");
    //digitalWrite(PIN_GREEN, HIGH);
//    tone(PIN_SPEAKER,NOTE_C3,250);
  //  delay(250);
  //  tone(PIN_SPEAKER,NOTE_C4,500);
  //  noTone(PIN_SPEAKER);
    delay(1000);
    //digitalWrite(PIN_GREEN, LOW);
  } else if (value == "denied") {
    Serial.print("card denied");
    //Homie.setNodeProperty(rfidNode, "validate", "denied", true);
      rfidNode.setProperty("validate").send("denied");
    //digitalWrite(PIN_RED, HIGH);
//    tone(PIN_SPEAKER,NOTE_C3,250);
//    delay(250);
    //digitalWrite(PIN_RED, LOW);
//    tone(PIN_SPEAKER,NOTE_G2,500);
  //  delay(250);
  //  noTone(PIN_SPEAKER);
    //digitalWrite(PIN_RED, HIGH);
    delay(1000);
  } else {
    //digitalWrite(PIN_RED, HIGH);
    Serial.println("unexpected response: ");
    //Homie.setNodeProperty(rfidNode, "validate", "unexpected", true);
      rfidNode.setProperty("validate").send("unexpected");
    Serial.print(value);
//    tone(PIN_SPEAKER,NOTE_A2,500);
//    delay(250);
    //digitalWrite(PIN_RED, LOW);
//    tone(PIN_SPEAKER,NOTE_A2,500);
    return false;
  }
    //digitalWrite(PIN_RED, HIGH);
  return true;
}
