#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// =====================================================
// ESP32-C3 SuperMini Smart Light Controller
// =====================================================

// ---------------- Wi-Fi Access Point -----------------

const char* ssid = "ESP32_SmartLight";
const char* password = "CHANGE_ME_123";

WebServer server(80);
Preferences prefs;

// =====================================================
// GPIO CONFIGURATION FOR ESP32-C3 SUPERMINI
// =====================================================

// RGB LED pins
// Assumes COMMON-ANODE RGB LED
const int redPin   = 3;
const int greenPin = 4;
const int bluePin  = 5;

// Extra LED
const int extraLed = 10;

// =====================================================
// LIGHT SETTINGS
// =====================================================

int red = 255;
int green = 255;
int blue = 255;

int brightness = 255;

bool rgbOn = true;

// Extra LED flash
bool flashMode = false;
bool ledState = false;

unsigned long lastFlash = 0;

// =====================================================
// SAVE SETTINGS
// =====================================================

void saveSettings() {

  prefs.putInt("red", red);
  prefs.putInt("green", green);
  prefs.putInt("blue", blue);
  prefs.putInt("bright", brightness);

  prefs.putBool("rgbOn", rgbOn);
  prefs.putBool("flash", flashMode);
}

// =====================================================
// UPDATE RGB LED
// =====================================================

void updateRGB() {

  // RGB OFF
  if (!rgbOn) {

    // Common-anode RGB:
    // 255 = OFF

    analogWrite(redPin, 255);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 255);

    return;
  }

  // Apply brightness
  int r = (red * brightness) / 255;
  int g = (green * brightness) / 255;
  int b = (blue * brightness) / 255;

  // Common-anode RGB
  // PWM is inverted

  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}

// =====================================================
// WEB PAGE
// =====================================================

void handleRoot() {

  String page = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<style>

body {

  background:#111;
  color:white;

  font-family:Arial;

  text-align:center;

}

.container {

  max-width:420px;

  margin:auto;

  padding:10px;

}

.slider {

  width:100%;

}

button {

  padding:10px 18px;

  margin:5px;

  font-size:16px;

  border:none;

  border-radius:6px;

}

#preview {

  width:100%;

  height:100px;

  border-radius:10px;

  margin-top:10px;

  background:#ffffff;

}

</style>

</head>

<body>

<div class="container">

<h2>ESP32 Smart Light</h2>

Color Picker

<br><br>

<input
  type="color"
  id="color"
  value="#ffffff"
  onchange="sendColor()">

<div id="preview"></div>

<br>

Brightness

<br>

<input
  type="range"
  min="0"
  max="255"
  value="255"
  id="brightness"
  class="slider"
  oninput="sendColor()">

<br><br>

<h3>RGB Control</h3>

<button onclick="rgbOn()">
ON
</button>

<button onclick="rgbOff()">
OFF
</button>

<br><br>

<h3>Extra LED</h3>

<button onclick="ledOn()">
ON
</button>

<button onclick="ledOff()">
OFF
</button>

<button onclick="ledFlash()">
FLASH
</button>

</div>

<script>

// =================================================
// COLOR
// =================================================

function sendColor() {

  var color =
    document.getElementById("color").value;

  var brightness =
    document.getElementById("brightness").value;

  document.getElementById("preview")
    .style.background = color;

  var r =
    parseInt(color.substring(1,3),16);

  var g =
    parseInt(color.substring(3,5),16);

  var b =
    parseInt(color.substring(5,7),16);

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/set?r=" + r +
    "&g=" + g +
    "&b=" + b +
    "&br=" + brightness,
    true
  );

  xhr.send();
}


// =================================================
// RGB ON
// =================================================

function rgbOn() {

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/rgbon",
    true
  );

  xhr.send();
}


// =================================================
// RGB OFF
// =================================================

function rgbOff() {

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/rgboff",
    true
  );

  xhr.send();
}


// =================================================
// EXTRA LED ON
// =================================================

function ledOn() {

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/on",
    true
  );

  xhr.send();
}


// =================================================
// EXTRA LED OFF
// =================================================

function ledOff() {

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/off",
    true
  );

  xhr.send();
}


// =================================================
// EXTRA LED FLASH
// =================================================

function ledFlash() {

  var xhr =
    new XMLHttpRequest();

  xhr.open(
    "GET",
    "/flash",
    true
  );

  xhr.send();
}

</script>

</body>

</html>

)rawliteral";

  server.send(
    200,
    "text/html",
    page
  );
}

// =====================================================
// SET RGB
// =====================================================

void handleSet() {

  if (server.hasArg("r"))
    red = server.arg("r").toInt();

  if (server.hasArg("g"))
    green = server.arg("g").toInt();

  if (server.hasArg("b"))
    blue = server.arg("b").toInt();

  if (server.hasArg("br"))
    brightness = server.arg("br").toInt();

  // Safety limits

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  brightness =
    constrain(brightness, 0, 255);

  updateRGB();

  saveSettings();

  server.send(
    200,
    "text/plain",
    "OK"
  );
}

// =====================================================
// RGB ON
// =====================================================

void rgbOnFunc() {

  rgbOn = true;

  updateRGB();

  saveSettings();

  server.send(
    200,
    "text/plain",
    "RGB ON"
  );
}

// =====================================================
// RGB OFF
// =====================================================

void rgbOffFunc() {

  rgbOn = false;

  updateRGB();

  saveSettings();

  server.send(
    200,
    "text/plain",
    "RGB OFF"
  );
}

// =====================================================
// EXTRA LED ON
// =====================================================

void ledOnFunc() {

  flashMode = false;

  ledState = true;

  digitalWrite(
    extraLed,
    HIGH
  );

  saveSettings();

  server.send(
    200,
    "text/plain",
    "ON"
  );
}

// =====================================================
// EXTRA LED OFF
// =====================================================

void ledOffFunc() {

  flashMode = false;

  ledState = false;

  digitalWrite(
    extraLed,
    LOW
  );

  saveSettings();

  server.send(
    200,
    "text/plain",
    "OFF"
  );
}

// =====================================================
// EXTRA LED FLASH
// =====================================================

void ledFlashFunc() {

  flashMode = true;

  lastFlash = millis();

  saveSettings();

  server.send(
    200,
    "text/plain",
    "FLASH"
  );
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  // Serial monitor

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32-C3 Smart Light Controller");
  Serial.println("==============================");


  // =================================================
  // GPIO
  // =================================================

  pinMode(
    redPin,
    OUTPUT
  );

  pinMode(
    greenPin,
    OUTPUT
  );

  pinMode(
    bluePin,
    OUTPUT
  );

  pinMode(
    extraLed,
    OUTPUT
  );


  // Start LEDs OFF

  analogWrite(
    redPin,
    255
  );

  analogWrite(
    greenPin,
    255
  );

  analogWrite(
    bluePin,
    255
  );

  digitalWrite(
    extraLed,
    LOW
  );


  // =================================================
  // PREFERENCES
  // =================================================

  prefs.begin(
    "light",
    false
  );

  red =
    prefs.getInt(
      "red",
      255
    );

  green =
    prefs.getInt(
      "green",
      255
    );

  blue =
    prefs.getInt(
      "blue",
      255
    );

  brightness =
    prefs.getInt(
      "bright",
      255
    );

  rgbOn =
    prefs.getBool(
      "rgbOn",
      true
    );

  flashMode =
    prefs.getBool(
      "flash",
      false
    );


  // =================================================
  // WI-FI ACCESS POINT
  // =================================================

  WiFi.mode(
    WIFI_AP
  );

  bool apStarted =
    WiFi.softAP(
      ssid,
      password
    );

  if (apStarted) {

    Serial.println(
      "WiFi AP started"
    );

    Serial.print(
      "SSID: "
    );

    Serial.println(
      ssid
    );

    Serial.print(
      "Password: "
    );

    Serial.println(
      password
    );

    Serial.print(
      "IP Address: "
    );

    Serial.println(
      WiFi.softAPIP()
    );

  } else {

    Serial.println(
      "ERROR: WiFi AP failed"
    );
  }


  // =================================================
  // WEB SERVER ROUTES
  // =================================================

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/set",
    handleSet
  );

  server.on(
    "/rgbon",
    rgbOnFunc
  );

  server.on(
    "/rgboff",
    rgbOffFunc
  );

  server.on(
    "/on",
    ledOnFunc
  );

  server.on(
    "/off",
    ledOffFunc
  );

  server.on(
    "/flash",
    ledFlashFunc
  );


  // =================================================
  // START SERVER
  // =================================================

  server.begin();

  Serial.println(
    "Web server started"
  );

  Serial.println(
    "Open: http://192.168.4.1"
  );


  // =================================================
  // RESTORE RGB
  // =================================================

  updateRGB();

}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Handle webpage requests

  server.handleClient();


  // =================================================
  // EXTRA LED FLASH
  // =================================================

  if (flashMode) {

    if (
      millis() - lastFlash >= 500
    ) {

      ledState =
        !ledState;

      digitalWrite(
        extraLed,
        ledState
      );

      lastFlash =
        millis();
    }
  }

}
