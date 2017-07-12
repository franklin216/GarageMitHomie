#include <Homie.h>



const int GARAGE_LICHT_PIN1 = 12;
const int GARAGE_LICHT_PIN2 = 14;

const int GARAGE_TOR_PIN1 = 13;
const int GARAGE_TOR_PIN2 = 15;

const int GARAGE_OeffungsMS = 1500;

int DOOR1_Millis = 0;
int DOOR2_Millis = 0;
HomieNode garageLichtNode1("garageLicht1", "switch");
HomieNode garageLichtNode2("garageLicht2", "switch");
HomieNode garageDOORNode1("garageTor1", "switch");
HomieNode garageDOORNode2("garageTor2", "switch");

bool lightOnHandlerMain(const HomieRange& range, const String& value, const int node) {
  if (value != "true" && value != "false") return false;
    bool on = (value == "true");
switch (node) {
  case 1:
  digitalWrite(GARAGE_LICHT_PIN1, on ? HIGH : LOW);
  garageLichtNode1.setProperty("on").send(value);
  break;
  case 2:
  digitalWrite(GARAGE_LICHT_PIN2, on ? HIGH : LOW);
  garageLichtNode2.setProperty("on").send(value);
break;
  default:
  break;

}

Homie.getLogger() << "Light is " << (on ? "on" : "off") << endl;

return true;
}


bool garageDoorHandlerMain(const HomieRange& range, const String& value, const int node) {
  if (value != "true") return false;
    bool on = (value == "true");
switch (node) {
  case 1:
  digitalWrite(GARAGE_TOR_PIN1, HIGH);
  garageDOORNode1.setProperty("on").send("true");
  DOOR1_Millis = millis();
  break;
  case 2:
  digitalWrite(GARAGE_TOR_PIN2, HIGH);
  garageDOORNode2.setProperty("on").send("true");
  DOOR2_Millis = millis();
break;
  default:
  break;

}

  Homie.getLogger() << "Door is " << (on ? "open" : "closed") << endl;

  return true;
}
bool lightOnHandler(const HomieRange& range, const String& value) {
  return lightOnHandlerMain(range, value, 1);
}
bool lightOnHandler2(const HomieRange& range, const String& value) {
    return lightOnHandlerMain(range, value, 2);
}
bool garageDoorHandler(const HomieRange& range, const String& value) {
  return garageDoorHandlerMain(range, value, 1);
}
bool garageDoorHandler2(const HomieRange& range, const String& value) {
    return garageDoorHandlerMain(range, value, 2);
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(GARAGE_LICHT_PIN1, OUTPUT);
  pinMode(GARAGE_LICHT_PIN2, OUTPUT);
  pinMode(GARAGE_TOR_PIN1, OUTPUT);
  pinMode(GARAGE_TOR_PIN2, OUTPUT);

  digitalWrite(GARAGE_LICHT_PIN1, LOW);
  digitalWrite(GARAGE_LICHT_PIN2, LOW);

  digitalWrite(GARAGE_TOR_PIN1, LOW);
  digitalWrite(GARAGE_TOR_PIN2, LOW);

  Homie_setFirmware("awesome-relay", "1.0.0");

  garageLichtNode1.advertise("on").settable(lightOnHandler);
  garageLichtNode2.advertise("on").settable(lightOnHandler2);

  garageDOORNode1.advertise("on").settable(garageDoorHandler);
  garageDOORNode2.advertise("on").settable(garageDoorHandler2);


  Homie.setup();
}

void schliesse1(){
  digitalWrite(GARAGE_TOR_PIN1, LOW);
  garageDOORNode1.setProperty("on").send("false");
  DOOR1_Millis = 0;
}

void schliesse2(){
  digitalWrite(GARAGE_TOR_PIN2, LOW);
  garageDOORNode2.setProperty("on").send("false");
  DOOR2_Millis = 0;
}

void checkGarage(){
  if(DOOR1_Millis>0){
    if(millis()-DOOR1_Millis > GARAGE_OeffungsMS){
      schliesse1();
    }
  }

  if(DOOR2_Millis>0){
    if(millis()-DOOR2_Millis > GARAGE_OeffungsMS){
      schliesse2();
    }
  }

}

void loop() {

checkGarage();

  Homie.loop();
}
