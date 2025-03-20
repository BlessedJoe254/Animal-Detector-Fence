#include <WiFi.h>
#include <HTTPClient.h>

// PIR Sensor Pin
#define PIR_SENSOR_PIN 13

// WiFi Credentials (from Code 3)
const char* ssid = "CYRIL TESLAH";  // From Code 3
const char* password = "12345678";  // From Code 3

// Africa's Talking API Configuration (from Code 3)
const char* apiUrl = "https://api.sandbox.africastalking.com/version1/messaging";  // From Code 3
const char* apiKey = "atsk_7ab5eb7344b16d5b8bb583e8eb49d0d2a18e7b91ff86389f0887366b4dea08ac587b9eae";  // From Code 3
const char* username = "sandbox";  // From Code 3
const char* sender = "AFRICASTKNG";  // From Code 3
const char* recipientNumber = "+254741384144";  // From Code 3

// Variables for non-blocking SMS sending
unsigned long lastSmsTime = 0;
const unsigned long smsInterval = 3000;  // 3 seconds

// Variables
bool motionDetected = false;

void connectWifi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }

    Serial.println("Connected to WiFi");
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Initialize PIR Sensor
    pinMode(PIR_SENSOR_PIN, INPUT);

    // Connect to WiFi
    connectWifi();

    Serial.println("System Ready!");
}

void loop() {
    // Read PIR Sensor
    if (digitalRead(PIR_SENSOR_PIN) == HIGH) {
        if (!motionDetected) {
            Serial.println("Motion Detected!");
            motionDetected = true;
        }
    } else {
        motionDetected = false;
    }

    // Send SMS at regular intervals (e.g., every 3 seconds)
    if (millis() - lastSmsTime >= smsInterval) {
        String message = "ALERT: An animal has been detected near the fence!";  // SMS content
        sendSMS(message);
        lastSmsTime = millis();  // Update the last SMS time
    }

    delay(1000);  // Delay to avoid multiple detections
}

// Function to send SMS (from Code 3)
void sendSMS(const String& message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(apiUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("Accept", "application/json");
        http.addHeader("apiKey", apiKey);

        String encodedMessage = urlEncode(message);

        String postData = "username=" + String(username) +
                          "&to=" + String(recipientNumber) +
                          "&message=" + encodedMessage +
                          "&from=" + String(sender);

        Serial.println("Sending SMS...");
        int httpResponseCode = http.POST(postData);

        if (httpResponseCode == HTTP_CODE_OK) {
            Serial.println("✅ SMS Sent Successfully!");
        } else {
            Serial.println("❌ Error Sending SMS: " + String(httpResponseCode));
            String response = http.getString();
            Serial.println("Response: " + response);
        }

        http.end();
    } else {
        Serial.println("⚠️ WiFi not connected, unable to send SMS.");
    }
}

// Function to URL encode the message (from Code 3)
String urlEncode(const String& str) {
    String encodedString = "";
    char c;
    char code0;
    char code1;
    for (unsigned int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == ' ') {
            encodedString += '+';
        } else if (isalnum(c)) {
            encodedString += c;
        } else {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9) {
                code1 = (c & 0xf) - 10 + 'A';
            }
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9) {
                code0 = c - 10 + 'A';
            }
            encodedString += '%';
            encodedString += code0;
            encodedString += code1;
        }
    }
    return encodedString;
}