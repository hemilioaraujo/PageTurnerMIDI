#include <Arduino.h>
#include <BLEMidi.h>
#include <esp_system.h>

const int NOTA_A3 = 69;
const int NOTA_B3 = 70;
const int MIDI_CHANNEL = 0;
const int DEFAULT_VELOCITY = 127;
const int LED_STATUS = 16;
const int BLINK_INTERVAL = 500;
const int BUTTON_DOWN = 13;
const int BUTTON_UP = 15;
const long NOTE_DURATION = 100;
const long REBOOT_HOLD_TIME = 5000;

int lastStateDown = LOW;
int lastStateUp = LOW;
int activeNote = 0;
unsigned long rebootPressTime = 0;
unsigned long noteOnTime = 0;
unsigned long lastBlinkTime = 0;

void initializeBlink();
void handleMidiNote(int note);
boolean buttonPressed(int pin, int& lastState);

void setup() {
  pinMode(LED_STATUS, OUTPUT);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);

  // initializeBlink();

  Serial.begin(115200);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("PageTurner MIDI");
  Serial.println("Waiting for connections...");
  BLEMidiServer.setOnConnectCallback([]() {
    digitalWrite(LED_STATUS, HIGH);
    Serial.println("Connected");
  });
  BLEMidiServer.setOnDisconnectCallback([]() {
    digitalWrite(LED_STATUS, LOW);
    Serial.println("Disconnected");
  });
  //BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library (not much for the server class...)
}

void loop() {
  if (BLEMidiServer.isConnected()) {
    if (digitalRead(BUTTON_DOWN) == LOW && digitalRead(BUTTON_UP) == LOW) {
      if (rebootPressTime == 0) {
        rebootPressTime = millis();
        Serial.println("Reboot: Keep pressing 5s...");
      } else if (millis() - rebootPressTime >= REBOOT_HOLD_TIME) {
        Serial.println("Rebooting...");
        delay(100);
        esp_restart();
      }
    } else {
      rebootPressTime = 0;
    }

    if (buttonPressed(BUTTON_DOWN, lastStateDown)) {
      Serial.println("Button DOWN");
      handleMidiNote(NOTA_A3);
    } else if (buttonPressed(BUTTON_UP, lastStateUp)) {
      Serial.println("Button UP");
      handleMidiNote(NOTA_B3);
    }

    if (activeNote != 0 && (millis() - noteOnTime >= NOTE_DURATION)) {
      BLEMidiServer.noteOff(MIDI_CHANNEL, activeNote, DEFAULT_VELOCITY);
      activeNote = 0;
    }

    delay(5);
  } else {
    if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
      digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
      lastBlinkTime = millis();
    }
  }
}

boolean buttonPressed(int pin, int& lastState) {
  int currentState = digitalRead(pin);
  boolean pressed = false;

  if (currentState != lastState) {
    // Pequeno debounce no estado da transição
    delay(5);
    currentState = digitalRead(pin);

    if (currentState == LOW) {
      pressed = true;
    }
  }
  lastState = currentState;
  return pressed;
}

void handleMidiNote(int note) {
  if (activeNote != 0) {
    BLEMidiServer.noteOff(MIDI_CHANNEL, activeNote, DEFAULT_VELOCITY);
  }
  BLEMidiServer.noteOn(MIDI_CHANNEL, note, DEFAULT_VELOCITY);
  noteOnTime = millis();
  activeNote = note;
}

void initializeBlink() {

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_STATUS, HIGH);
    delay(BLINK_INTERVAL);
    digitalWrite(LED_STATUS, LOW);
    if (i < 2) {
      delay(BLINK_INTERVAL);
    }
  }
}