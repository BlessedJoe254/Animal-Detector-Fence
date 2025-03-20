#include <WiFi.h>
#include <HTTPClient.h>

// PIR Sensor Pin
#define PIR_SENSOR_PIN 13

// WiFi Credentials
const char* ssid = "CYRIL TESLAH";  // Replace with your WiFi SSID
const char* password = "12345678";  // Replace with your WiFi password

// Africa's Talking API Configuration
const char* apiUrl = "https://api.sandbox.africastalking.com/version1/messaging";
const char* apiKey = "atsk_7ab5eb7344b16d5b8bb583e8eb49d0d2a18e7b91ff86389f0887366b4dea08ac587b9eae";
const char* username = "sandbox";
const char* sender = "AFRICASTKNG";
const char* recipientNumber = "+254741384144";  // Replace with the recipient's phone number

// Variables for non-blocking SMS sending
unsigned long lastSmsTime = 0;
const unsigned long smsInterval = 10000;  // 10 seconds

// Variables
bool motionDetected = false;

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Initialize PIR Sensor
    pinMode(PIR_SENSOR_PIN, INPUT);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {  // 10-second timeout
        delay(1000);
        Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi");
    } else {
        Serial.println("\nConnected to WiFi");
    }
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

    // Send SMS only if the interval has passed
    if (millis() - lastSmsTime >= smsInterval) {
        sendSMS("ALERT: An animal has been detected near the fence!");
        lastSmsTime = millis();
    }

    delay(1000);  // Delay to avoid multiple detections
}

// Function to send SMS
void sendSMS(const String& message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        if (http.begin(apiUrl)) {  // Ensure the HTTP client starts successfully
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            http.addHeader("Accept", "application/json");
            http.addHeader("apiKey", apiKey);

            String encodedMessage = urlEncode(message);

            // Prepare POST data
            String postData = "username=" + String(username) +
                             "&to=" + String(recipientNumber) +
                             "&message=" + encodedMessage +
                             "&from=" + String(sender);

            Serial.println("Sending SMS...");
            Serial.println("POST Data: " + postData);  // Debugging: Print POST data

            int httpResponseCode = http.POST(postData);

            if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_CREATED) {
                Serial.println("✅ SMS Sent Successfully!");
                String response = http.getString();
                Serial.println("Response: " + response);  // Debugging: Print API response
            } else {
                Serial.println("❌ Error Sending SMS. HTTP Code: " + String(httpResponseCode));
                String response = http.getString();
                Serial.println("Response: " + response);  // Debugging: Print API response
            }

            http.end();
        } else {
            Serial.println("❌ Failed to initialize HTTP client.");
        }
    } else {
        Serial.println("⚠️ WiFi not connected, unable to send SMS.");
    }
}

// Function to URL encode the message
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