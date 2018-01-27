#include <Arduino.h>
#include <Homie.h>
//fff
const int GARAGE_LICHT_PIN1 = 12;
const int GARAGE_LICHT_PIN2 = 14;
const int GARAGE_TOR_PIN1 = 13;
const int GARAGE_TOR_PIN2 = 15;
const int PIN_BUTTON1 = 4;
const int PIN_BUTTON2 = 2;
const int GARAGE_OeffungsMS = 500;

unsigned long buttonDownTime_pin1 = 0;
unsigned long buttonDownTime_pin2 = 0;
byte lastButtonState_pin1 = 0;//1; //2
byte lastButtonState_pin2 = 0;//1; //3
byte buttonPressHandled_pin1 = 0;
byte buttonPressHandled_pin2 = 0;

int DOOR1_Millis = 0;
int DOOR2_Millis = 0;
HomieNode garageLichtNode1("garageLicht1", "switch");
HomieNode garageLichtNode2("garageLicht2", "switch");
HomieNode garageDOORNode1("garageTor1", "switch");
HomieNode garageDOORNode2("garageTor2", "switch");


void setup() {
  Serial.begin(115200);

  pinMode(GARAGE_LICHT_PIN1, OUTPUT);
  pinMode(GARAGE_LICHT_PIN2, OUTPUT);
  pinMode(GARAGE_TOR_PIN1, OUTPUT);
  pinMode(GARAGE_TOR_PIN2, OUTPUT);
  pinMode(PIN_BUTTON1, INPUT);
  pinMode(PIN_BUTTON2, INPUT);
  digitalWrite(GARAGE_LICHT_PIN1, LOW);
  digitalWrite(GARAGE_LICHT_PIN2, LOW);
  digitalWrite(GARAGE_TOR_PIN1, LOW);
  digitalWrite(GARAGE_TOR_PIN2, LOW);
  Homie_setFirmware("garage2", "1.0.0");
  garageLichtNode1.advertise("on").settable(lightOnHandler);
  garageLichtNode2.advertise("on").settable(lightOnHandler2);
  garageDOORNode1.advertise("on").settable(garageDoorHandler);
  garageDOORNode2.advertise("on").settable(garageDoorHandler2);
//setze alle erstmal auf false, damit sie nicht eingeschaltet werden nach Stromausfall
  //garageDOORNode1.setProperty("on").send("false");
  //garageDOORNode2.setProperty("on").send("false");
  //garageLichtNode1.setProperty("on").send("false");
  //garageLichtNode2.setProperty("on").send("false");

  Homie.setLoopFunction(loopHandler);
//Homie.getLogger() << "15... " << endl;
  Homie.setup();
  //Homie.getLogger() << "16... " << endl;
}

void loop() {
  // all loop activity is handled by Homie, to ensure connectivity
  // and prevent blocking activity that could disrupt
  // communication
  Homie.loop();
}

void toggleRelay(const int node) {
  bool on = false;
  switch (node) {
    case PIN_BUTTON1:
      on = digitalRead(GARAGE_LICHT_PIN1) == HIGH;
      digitalWrite(GARAGE_LICHT_PIN1, on ? LOW : HIGH);
      garageLichtNode1.setProperty("on").send(on ? "false" : "true");
    break;
    case PIN_BUTTON2:
      on = digitalRead(GARAGE_LICHT_PIN2) == HIGH;
      digitalWrite(GARAGE_LICHT_PIN2, on ? LOW : HIGH);
      garageLichtNode2.setProperty("on").send(on ? "false" : "true");
    break;
    default:
    break;
  }
	Homie.getLogger() << "Switch " << (node==PIN_BUTTON1 ? "1" : "2")<< " is " << (on ? "off" : "on") << endl;
}


bool lightOnHandlerMain(const HomieRange& range, const String& value, const int node) {
  if (value != "true" && value != "false") return false;
  bool on = (value == "true");
  digitalWrite(node, on ? HIGH : LOW);
  switch (node) {
    case GARAGE_LICHT_PIN1:
		garageLichtNode1.setProperty("on").send(value);
		break;
    case GARAGE_LICHT_PIN2:
		garageLichtNode2.setProperty("on").send(value);
		break;
    case GARAGE_TOR_PIN1:
		garageDOORNode1.setProperty("on").send(value);
		DOOR1_Millis = millis();
		break;
    case GARAGE_TOR_PIN2:
		garageDOORNode2.setProperty("on").send(value);
		DOOR2_Millis = millis();
		break;

	default:
    break;
  }
  Homie.getLogger() << (node==GARAGE_TOR_PIN1||node==GARAGE_TOR_PIN2 ? "Door  " : "Light ") << (node==GARAGE_TOR_PIN1||node==GARAGE_LICHT_PIN1 ? "1" : "2")<< " is " << (on ? "on" : "off") << endl;
  return true;
}


bool lightOnHandler(const HomieRange& range, const String& value) {
  return lightOnHandlerMain(range, value, GARAGE_LICHT_PIN1);
}

bool lightOnHandler2(const HomieRange& range, const String& value) {
    return lightOnHandlerMain(range, value, GARAGE_LICHT_PIN2);
}

bool garageDoorHandler(const HomieRange& range, const String& value) {
	return lightOnHandlerMain(range, value, GARAGE_TOR_PIN1);
}

bool garageDoorHandler2(const HomieRange& range, const String& value) {
	return lightOnHandlerMain(range, value, GARAGE_TOR_PIN2);
}

void buttonHandler(const int node) {
  byte buttonState = digitalRead(node);
  switch(node){
    case PIN_BUTTON1:
      if ( buttonState != lastButtonState_pin1 ) {
        if (buttonState == LOW) {
          buttonDownTime_pin1     = millis();
          buttonPressHandled_pin1 = 0;
        }
        else {
          unsigned long dt = millis() - buttonDownTime_pin1;
          if ( dt >= 90 && dt <= 900 && buttonPressHandled_pin1 == 0 ) {
            toggleRelay(node);
            buttonPressHandled_pin1 = 1;
          }
        }
        lastButtonState_pin1 = buttonState;
      }
    break;
    case PIN_BUTTON2:
      if ( buttonState != lastButtonState_pin2 ) {
        if (buttonState == LOW) {
          buttonDownTime_pin2     = millis();
          buttonPressHandled_pin2 = 0;
        }
        else {
          unsigned long dt = millis() - buttonDownTime_pin2;
          if ( dt >= 90 && dt <= 900 && buttonPressHandled_pin2 == 0 ) {
            toggleRelay(node);
            buttonPressHandled_pin2 = 1;
          }
        }
        lastButtonState_pin2 = buttonState;
      }

    break;
    default:
    break;
  }
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

void loopHandler() {
  buttonHandler(PIN_BUTTON1);
  buttonHandler(PIN_BUTTON2);
  checkGarage();
}
