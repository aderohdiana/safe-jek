/*
   Program  : Safe-Jek v1.0
   Date     : July 2021
*/

// include library essential
#include <SPI.h>
#include <MFRC522.h>
#include "BluetoothSerial.h"

// set pin of ESP32 as variable
#define relay 2
#define button 5
#define RST_PIN 22
#define SS_PIN 21

// configure internal bluetooth ESP32
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

MFRC522 mfrc522(SS_PIN, RST_PIN);
BluetoothSerial SerialBT;

// set variable global
String tagUID1 = "C6 92 C4 F7";
String tagUID2 = "57 1F E8 1B";
String message = "";
char incomingChar;

// main function
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Safe-Jek"); //Bluetooth device name
  pinMode(relay, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println("---- Safe-Jek Start ----");
}

// looping function
void loop() {
  int buttonVal = digitalRead(button);
  Serial.println(buttonVal);

  if (buttonVal == 1) {
    // call bluetooth function
    activeBT();

    // call RFID function
    activeRFID();
  }
  if (buttonVal == 0) {
    message = "off";
    digitalWrite(relay, LOW);
    Serial.println("Relay OFF");
  }
}

// Bluetooth function
void activeBT() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n') {
      message += String(incomingChar);
    }
    else {
      message = "";
    }
    Serial.write(incomingChar);
    if (message == "on") {
      digitalWrite(relay, HIGH);
    }
    else if (message == "off") {
      digitalWrite(relay, LOW);
    }
  }
}

// RFID function
void activeRFID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String tag = "";
  for (byte j = 0; j < mfrc522.uid.size; j++)
  {
    tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[j], HEX));
  }
  tag.toUpperCase();

  if (tag.substring(1) == tagUID1 || tag.substring(1) == tagUID2)
  {
    // If UID of tag is matched.
    digitalWrite(relay, HIGH);
    Serial.println("Relay ON by RFID");
  }
  else {
    digitalWrite(relay, LOW);
  }
}
