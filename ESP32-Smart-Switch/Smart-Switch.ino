#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3j9j2SgjK"
#define BLYNK_TEMPLATE_NAME "Smart Switch ESP32"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Relay pins (Active LOW)
#define RELAY1 23
#define RELAY2 22
#define RELAY3 21
#define RELAY4 19

#define PIR_PIN 33

Preferences prefs;

// ===== STATES =====
bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

// Motion
bool motionEnabled = true;
bool motionActive = false;
unsigned long motionTimeout = 120000;
unsigned long lastMotionTime = 0;
bool lastPirState = LOW;

void applyRelay(uint8_t relay, bool state) {
  static bool lastState[40];   // stores last state for GPIOs

  if (lastState[relay] != state) {
    digitalWrite(relay, state ? LOW : HIGH);
    lastState[relay] = state;
  }
}


void saveState(const char* key, bool state) {
  prefs.putBool(key, state);
}

// ===== BLYNK CALLBACKS =====
BLYNK_CONNECTED() {
  // 🔥 Sync app with ESP state after reboot / reconnect
  Blynk.virtualWrite(V0, relay1State);
  Blynk.virtualWrite(V1, relay2State);
  Blynk.virtualWrite(V2, relay3State);
  Blynk.virtualWrite(V3, relay4State);
  Blynk.virtualWrite(V10, motionEnabled);
}

BLYNK_WRITE(V0) {
  relay1State = param.asInt();
  applyRelay(RELAY1, relay1State);
  saveState("r1", relay1State);
}

BLYNK_WRITE(V1) {
  relay2State = param.asInt();
  applyRelay(RELAY2, relay2State);
  saveState("r2", relay2State);
}

BLYNK_WRITE(V2) {
  relay3State = param.asInt();
  applyRelay(RELAY3, relay3State);
  saveState("r3", relay3State);
}

BLYNK_WRITE(V3) {
  relay4State = param.asInt();
  applyRelay(RELAY4, relay4State);
  saveState("r4", relay4State);
}

BLYNK_WRITE(V10) {
  motionEnabled = param.asInt();
  prefs.putBool("motion", motionEnabled);

  if (motionEnabled) {
    lastMotionTime = millis();
    motionActive = false;
  }
}

BLYNK_WRITE(V11) {
  motionTimeout = param.asInt() * 1000UL;
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(PIR_PIN, INPUT_PULLDOWN); 

  prefs.begin("switch", false);

  // 🔁 RESTORE SAVED STATES
  relay1State = prefs.getBool("r1", false);
  relay2State = prefs.getBool("r2", false);
  relay3State = prefs.getBool("r3", false);
  relay4State = prefs.getBool("r4", false);
  motionEnabled = prefs.getBool("motion", true);

  applyRelay(RELAY1, relay1State);
  applyRelay(RELAY2, relay2State);
  applyRelay(RELAY3, relay3State);
  applyRelay(RELAY4, relay4State);

  Blynk.begin(auth, ssid, pass);
}

// ===== LOOP =====
void loop() {
  Blynk.run();

  // ===== MOTION LOGIC =====
  if (motionEnabled) {
    if (digitalRead(PIR_PIN) == HIGH) {
      lastMotionTime = millis();

      if (!relay1State) {
        relay1State = true;
        applyRelay(RELAY1, true);
        saveState("r1", true);
        Blynk.virtualWrite(V0, 1);
        motionActive = true;
      }
    }

    if (motionActive && relay1State &&
        millis() - lastMotionTime > motionTimeout) {

      relay1State = false;
      applyRelay(RELAY1, false);
      saveState("r1", false);
      Blynk.virtualWrite(V0, 0);
      motionActive = false;
    }
  }

  // ===== PIR STATUS INDICATOR =====
  bool pirState = digitalRead(PIR_PIN);
  if (pirState != lastPirState) {
    Blynk.virtualWrite(V12, pirState ? 255 : 0);
    lastPirState = pirState;
  }
}
