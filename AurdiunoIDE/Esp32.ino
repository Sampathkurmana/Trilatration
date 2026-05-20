#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==========================================
// 🔧 TOWER B CONFIGURATION
// ==========================================

// ----- YOUR MOBILE HOTSPOT CREDENTIALS -----
const char* WIFI_SSID = "<Your_wifi_name>";           // Your hotspot name
const char* WIFI_PASSWORD = "<your_wifi_password>";    // Your hotspot password

// ----- FIREBASE CONFIGURATION -----
#define FIREBASE_URL "https://triangulation-4be54-default-rtdb.firebaseio.com"

// ----- TOWER A IDENTIFICATION -----
//#define NODE_ID "ESP32_NODE_01"
//#define TOWER_LETTER "A"
//#define TOWER_POSITION_X 0
//#define TOWER_POSITION_Y 0

// ----- TOWER B IDENTIFICATION -----
//#define NODE_ID "ESP32_NODE_02"
//#define TOWER_LETTER "B"
//#define TOWER_POSITION_X 4
//#define TOWER_POSITION_Y 0

// ----- TOWER C IDENTIFICATION -----
#define NODE_ID "ESP32_NODE_03"
#define TOWER_LETTER "C"
#define TOWER_POSITION_X 0
#define TOWER_POSITION_Y 3

// ==========================================
// LED CONFIGURATION
// ==========================================
#define EXTERNAL_LED_PIN 4     // GPIO4 for external LED
#define BUILTIN_LED_PIN 2      // GPIO2 for built-in LED (most ESP32 boards)

// LED negative leg (cathode) connects to GND via 330Ω resistor

// ==========================================
// LED BLINK PATTERN: 3 blinks, then OFF for 1.5 seconds, then repeat
// ==========================================
#define BLINK_DURATION 150      // Each blink ON time (milliseconds)
#define BLINK_PAUSE 150         // Pause between blinks (milliseconds)
#define CYCLE_PAUSE 1500        // Pause after 3 blinks (1.5 seconds)

// ==========================================
// RSSI CALIBRATION
// ==========================================
float RSSI_AT_1M = -42.0;
float PATH_LOSS_N = 2.5;

// ==========================================
// Kalman Filter for RSSI Smoothing
// ==========================================
float kalmanQ = 0.05;
float kalmanR = 0.5;
float kalmanP = 1.0;
float filteredRSSI = -50.0;

float kalmanFilter(float measurement) {
    kalmanP = kalmanP + kalmanQ;
    float kalmanK = kalmanP / (kalmanP + kalmanR);
    filteredRSSI = filteredRSSI + kalmanK * (measurement - filteredRSSI);
    kalmanP = (1 - kalmanK) * kalmanP;
    return filteredRSSI;
}

// ==========================================
// RSSI to Distance Conversion
// ==========================================
float rssiToDistance(float rssi) {
    if (rssi == 0) return -1.0;
    float ratio = (RSSI_AT_1M - rssi) / (10.0 * PATH_LOSS_N);
    return pow(10.0, ratio);
}

// ==========================================
// LED Control - Both LEDs: 3 blinks, then OFF for 1.5 seconds, repeat
// ==========================================
void updateLEDs() {
    static unsigned long lastCycleStart = 0;
    static int blinkCount = 0;
    static int state = 0;  // 0 = LED ON, 1 = LED OFF (between blinks), 2 = cycle pause
    static unsigned long lastStateChange = 0;
    
    unsigned long currentMillis = millis();
    
    // Start new cycle after pause
    if (state == 2) {
        if (currentMillis - lastStateChange >= CYCLE_PAUSE) {
            // Start new blink cycle
            blinkCount = 0;
            state = 0;
            lastStateChange = currentMillis;
            digitalWrite(EXTERNAL_LED_PIN, HIGH);
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            Serial.println("💡 LEDs: Starting new blink cycle (3 blinks)");
        }
        return;
    }
    
    // Handle blinking sequence
    switch(state) {
        case 0: // LED ON during blink
            if (currentMillis - lastStateChange >= BLINK_DURATION) {
                // Turn OFF both LEDs
                digitalWrite(EXTERNAL_LED_PIN, LOW);
                digitalWrite(BUILTIN_LED_PIN, LOW);
                blinkCount++;
                state = 1;
                lastStateChange = currentMillis;
                
                if (blinkCount >= 3) {
                    // All 3 blinks done, start cycle pause
                    state = 2;
                    lastStateChange = currentMillis;
                    Serial.println("💡 LEDs: 3 blinks completed - Pause for 1.5 seconds");
                }
            }
            break;
            
        case 1: // LED OFF between blinks
            if (currentMillis - lastStateChange >= BLINK_PAUSE && blinkCount < 3) {
                // Turn ON both LEDs for next blink
                digitalWrite(EXTERNAL_LED_PIN, HIGH);
                digitalWrite(BUILTIN_LED_PIN, HIGH);
                state = 0;
                lastStateChange = currentMillis;
            }
            break;
    }
}

// ==========================================
// Scan for Mobile Hotspot RSSI
// ==========================================
float getHotspotRSSI() {
    int n = WiFi.scanNetworks(false, false);
    if (n == 0) return -999;
    
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == WIFI_SSID) {
            float rssi = WiFi.RSSI(i);
            WiFi.scanDelete();
            return rssi;
        }
    }
    WiFi.scanDelete();
    return -999;
}

// ==========================================
// Send Data to Firebase
// ==========================================
bool sendToFirebase(float rssi, float distanceM, int distanceCm, unsigned long timestamp) {
    HTTPClient http;
    
    // Create JSON payload manually
    String jsonString = "{";
    jsonString += "\"rssi\":" + String(rssi, 1) + ",";
    jsonString += "\"distance_m\":" + String(distanceM, 2) + ",";
    jsonString += "\"distance_cm\":" + String(distanceCm) + ",";
    jsonString += "\"timestamp\":" + String(timestamp);
    jsonString += "}";
    
    String url = String(FIREBASE_URL) + "/nodes/" + NODE_ID + ".json";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.PUT(jsonString);
    
    if (httpCode == 200) {
        http.end();
        return true;
    } else {
        http.end();
        return false;
    }
}

// ==========================================
// Setup Function
// ==========================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Initialize LED pins
    pinMode(EXTERNAL_LED_PIN, OUTPUT);
    pinMode(BUILTIN_LED_PIN, OUTPUT);
    digitalWrite(EXTERNAL_LED_PIN, LOW);
    digitalWrite(BUILTIN_LED_PIN, LOW);
    
    Serial.println("\n========================================");
    Serial.println("🔷 ESP32 TOWER B - Triangulation System");
    Serial.println("========================================");
    Serial.printf("📡 Node: %s | Tower: %s\n", NODE_ID, TOWER_LETTER);
    Serial.printf("📍 Position: (%.1f, %.1f)\n", TOWER_POSITION_X, TOWER_POSITION_Y);
    Serial.println("💡 External LED on GPIO4 + Built-in LED on GPIO2");
    Serial.println("💡 Pattern: 3 blinks, then OFF for 1.5 seconds, repeat");
    Serial.println("========================================\n");
    
    // LED Test - Both LEDs blink together 3 times
    Serial.println("🔆 LED Startup Test:");
    for (int i = 0; i < 3; i++) {
        digitalWrite(EXTERNAL_LED_PIN, HIGH);
        digitalWrite(BUILTIN_LED_PIN, HIGH);
        delay(200);
        digitalWrite(EXTERNAL_LED_PIN, LOW);
        digitalWrite(BUILTIN_LED_PIN, LOW);
        delay(200);
    }
    Serial.println("✅ LED Test Complete\n");
    
    // Connect to WiFi Hotspot
    Serial.printf("📱 Connecting to hotspot: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected to hotspot!");
        Serial.printf("📡 ESP32 IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("📶 WiFi RSSI: %d dBm\n", WiFi.RSSI());
    } else {
        Serial.println("\n❌ Failed to connect!");
        Serial.println("⚠️ Check hotspot is ON and credentials are correct");
        
        // Rapid blink for error indication
        for (int i = 0; i < 20; i++) {
            digitalWrite(EXTERNAL_LED_PIN, HIGH);
            digitalWrite(BUILTIN_LED_PIN, HIGH);
            delay(100);
            digitalWrite(EXTERNAL_LED_PIN, LOW);
            digitalWrite(BUILTIN_LED_PIN, LOW);
            delay(100);
        }
        return;
    }
    
    // Update tower position in Firebase
    String posUrl = String(FIREBASE_URL) + "/tower_positions/" + TOWER_LETTER + ".json";
    String posJson = "{\"x\":" + String(TOWER_POSITION_X) + ",\"y\":" + String(TOWER_POSITION_Y) + "}";
    HTTPClient http;
    http.begin(posUrl);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.PUT(posJson);
    if (httpCode == 200) {
        Serial.println("✅ Tower position updated to Firebase");
    }
    http.end();
    
    Serial.println("\n⚡ REAL-TIME RSSI MONITORING ACTIVE ⚡");
    Serial.println("💡 LED Pattern:");
    Serial.println("   Blink 1: ON→150ms→OFF");
    Serial.println("   Blink 2: ON→150ms→OFF");
    Serial.println("   Blink 3: ON→150ms→OFF");
    Serial.println("   Then OFF for 1.5 seconds");
    Serial.println("   Then repeat cycle\n");
}

// ==========================================
// Main Loop - Real-time with Fixed LED Pattern
// ==========================================
void loop() {
    float rawRSSI = getHotspotRSSI();
    
    if (rawRSSI != -999) {
        // Apply Kalman filter for smoothing
        float smoothRSSI = kalmanFilter(rawRSSI);
        
        // Calculate distance from RSSI
        float distance = rssiToDistance(smoothRSSI);
        int distanceCm = (int)(distance * 100);
        unsigned long timestamp = millis() / 1000;
        
        // Send to Firebase
        if (sendToFirebase(smoothRSSI, distance, distanceCm, timestamp)) {
            static unsigned long lastPrint = 0;
            if (millis() - lastPrint > 2000) {
                lastPrint = millis();
                
                // Determine signal quality for display
                const char* quality;
                if (smoothRSSI > -50) quality = "Excellent 📶📶📶";
                else if (smoothRSSI > -60) quality = "Good 📶📶";
                else if (smoothRSSI > -70) quality = "Fair 📶";
                else quality = "Poor 📶";
                
                Serial.printf("[%.1f] 📡 Tower %s: RSSI=%.1f dBm | 📏 %.2f m (%d cm) | %s\n", millis() / 1000.0, TOWER_LETTER, smoothRSSI, distance, distanceCm, quality);
            }
        }
    } else {
        // No signal detected
        static unsigned long lastWarning = 0;
        if (millis() - lastWarning > 5000) {
            lastWarning = millis();
            Serial.printf("⚠️ Tower %s: Hotspot '%s' not found - Check if hotspot is ON\n", TOWER_LETTER, WIFI_SSID);
        }
        delay(100);
    }
    
    // Update LEDs (both LEDs blink together: 3 blinks, then OFF for 1.5 seconds)
    updateLEDs();
    
    // Small delay for stability
    delay(10);
}
