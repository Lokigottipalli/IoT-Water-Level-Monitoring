# IoT Water Level Monitoring System

Smart water tank monitoring and auto pump control using ESP32 + HC-SR04 + Blynk IoT

![Blynk IoT](https://img.shields.io/badge/Platform-Blynk%20IoT-23C8D2)
![ESP32](https://img.shields.io/badge/Board-ESP32-000000)

### Features
- **Real-time monitoring**: Water level % on mobile app
- **Auto pump control**: Turns ON when level < 20%, OFF when > 90%
- **Dual modes**: Manual ON/OFF or Automatic via Blynk
- **Live status**: Tank LOW / MEDIUM / FULL indicator
- **WiFi + Motor LEDs**: See connection and pump status in app

### Hardware Required
- ESP32 Dev Module
- HC-SR04 Ultrasonic Sensor
- 5V Single Channel Relay Module  
- 12V DC Water Pump + Adapter
- Jumper wires, Breadboard

### Wiring Diagram
| ESP32 GPIO | Component | Pin |
| --- | --- | --- |
| GPIO 5 | HC-SR04 | TRIG |
| GPIO 18 | HC-SR04 | ECHO |
| GPIO 23 | Relay Module | IN |
| 3.3V | HC-SR04 | VCC |
| VIN (5V) | Relay Module | VCC |
| GND | HC-SR04 + Relay | GND |

### Blynk IoT Setup
1. **Create Template** in Blynk.Console: `Smart water tank monitor`
2. **Add Datastreams**:
   - V0: Water Level % `0-100` `Integer`
   - V1: Tank Level `0-100` `Integer`
   - V2: Tank Status `String`
   - V3: Manual Pump `0-1` `Integer` 
   - V4: Auto Mode `0-1` `Integer`
   - V5: Motor Status `0-1` `Integer`
   - V6: Distance cm `0-100` `Double`
   - V7: WiFi Status `0-1` `Integer`
3. **Create Device** from Template → Copy `Auth Token`

### Software Setup
1. **Arduino IDE**: Install ESP32 boards via Boards Manager
2. **Libraries**: Install `Blynk` by Volodymyr Shymanskyy
3. **Code**: Open `water_level_monitor.ino`
4. **Configure**: 
   ```cpp
   #define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"
   char ssid[] = "YOUR_WIFI_NAME";
   char pass[] = "YOUR_WIFI_PASSWORD";
   float TANK_HEIGHT = 30.0; // Change to your tank height in cm
