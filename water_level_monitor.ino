/*

  IoT-Based Smart Water Level Monitoring System
  Author : Lokesh Prakash Gandhi Gottipalli
  Board : ESP32
  Platform: Blynk IoT (blynk.cloud)
  GitHub : github.com/Lokigottipalli


  HARDWARE CONNECTIONS

  HC-SR04 Ultrasonic Sensor:
    VCC -> 3.3V
    GND -> GND
    TRIG -> GPIO 5
    ECHO -> GPIO 18

  Relay Module (Active LOW):
    VCC -> VIN (5V)
    GND -> GND
    IN -> GPIO 23

  Motor/Pump -> 12V DC Adapter via Relay

  BLYNK VIRTUAL PINS

  V0 -> Gauge Water level % (0-100)
  V1 -> Tank vertical Level indicator (0-100)
  V2 -> Label Tank status (LOW/MEDIUM/FULL)
  V3 -> Button Switch Motor manual control (ON/OFF)
  V4 -> Button Switch Auto mode toggle (ON/OFF)
  V5 -> LED Motor status (ON/OFF)
  V6 -> Value Display Distance in cm (0-100)
  V7 -> LED WiFi status (ON/OFF)

*/

#define BLYNK_TEMPLATE_ID "TMPL3NsEEXlGT"
#define BLYNK_TEMPLATE_NAME "Smart water tank monitor"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN_HERE" // ⚠️ Get from Blynk Console
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ── WiFi credentials ──────────────────────────────────────
char ssid[] = "YOUR_WIFI_NAME"; // ⚠️ Replace with your hotspot
char pass[] = "YOUR_WIFI_PASSWORD"; // ⚠️ Replace with your password

// ── Pin definitions ───────────────────────────────────────
#define TRIG 5
#define ECHO 18
#define RELAY_PIN 23

// ── Tank configuration ────────────────────────────────────
float TANK_HEIGHT = 30.0; // ⚠️ Measure your tank height in cm and update

// ── State variables ───────────────────────────────────────
bool autoMode = true;
long duration;
float distance;
int levelPercent;

BlynkTimer timer;

// ─────────────────────────────────────────────────────────
// Read sensor + send all data to Blynk (called every 2s)
// ─────────────────────────────────────────────────────────
void sendSensor() {

  // ── Measure distance ──────────────────────────────────
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;

  // ── Clamp to tank range ───────────────────────────────
  if (distance < 0) distance = 0;
  if (distance > TANK_HEIGHT) distance = TANK_HEIGHT;

  // ── Calculate water level % ───────────────────────────
  levelPercent = (1 - (distance / TANK_HEIGHT)) * 100;
  if (levelPercent < 0) levelPercent = 0;
  if (levelPercent > 100) levelPercent = 100;

  // ── Send to Blynk ─────────────────────────────────────
  Blynk.virtualWrite(V0, levelPercent);
  Blynk.virtualWrite(V1, levelPercent);
  Blynk.virtualWrite(V6, distance);

  // ── Tank status string ────────────────────────────────
  String status;
  if (levelPercent < 20) status = "LOW";
  else if (levelPercent <= 90) status = "MEDIUM";
  else status = "FULL";
  Blynk.virtualWrite(V2, status);

  Serial.printf("[SENSOR] Distance: %.1f cm | Level: %d%% | Status: %s\n",
                distance, levelPercent, status.c_str());

  // ── Auto mode motor logic ─────────────────────────────
  if (autoMode) {
    if (levelPercent < 20) {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(V5, 255);
      Blynk.logEvent("tank_low", "Tank is Low! Motor ON");
      Serial.println("[AUTO] Level LOW — Motor ON");

    } else if (levelPercent > 90) {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(V5, 0);
      Blynk.logEvent("tank_full", "Tank is Full! Motor OFF");
      Serial.println("[AUTO] Level FULL — Motor OFF");
    }

  } else {
    // ── Manual mode ───────────────────────────────────
    // Note: virtualRead in loop is not reliable. Use V3 handler only.
    // Manual control handled in BLYNK_WRITE(V3)
  }

  // ── WiFi LED always ON when connected ─────────────────
  Blynk.virtualWrite(V7, 255);
}

// ─────────────────────────────────────────────────────────
// V3 — Manual motor control button
// ─────────────────────────────────────────────────────────
BLYNK_WRITE(V3) {
  int state = param.asInt();
  if (autoMode) {
    Serial.println("[V3] Ignored — Auto mode active");
    return;
  }
  if (state) {
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(V5, 255);
    Serial.println("[MANUAL] Motor ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V5, 0);
    Serial.println("[MANUAL] Motor OFF");
  }
}

// ─────────────────────────────────────────────────────────
// V4 — Auto mode toggle
// ─────────────────────────────────────────────────────────
BLYNK_WRITE(V4) {
  autoMode = param.asInt();
  Serial.printf("[MODE] %s\n", autoMode? "Auto mode ON" : "Manual mode ON");
  if (!autoMode) {
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V5, 0);
  }
}

// ─────────────────────────────────────────────────────────
// Setup
// ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n[INIT] Smart Water Tank Monitor starting...");

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // Motor OFF at startup

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensor);

  Serial.println("[INIT] Setup complete — reading every 2 seconds");
}

// ─────────────────────────────────────────────────────────
// Loop
// ─────────────────────────────────────────────────────────
void loop() {
  Blynk.run();
  timer.run();
}
