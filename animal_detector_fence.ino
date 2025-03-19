#include <WiFi.h>
#include <HTTPClient.h>

// PIR Sensor Pin
#define PIR_SENSOR_PIN 13

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";  // Replace with your Wi-Fi SSID
const char* password = "YOUR_WIFI_PASSWORD";  // Replace with your Wi-Fi password

// Africa's Talking Sandbox API Details
const String API_KEY = "YOUR_API_KEY";  // Replace with your Africa's Talking API key
const String USERNAME = "sandbox";  // Use "sandbox" for testing
const String PHONE_NUMBER = "+254798336542";  // Replace with the recipient's phone number

// Variables
bool motionDetected = false;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize PIR Sensor
  pinMode(PIR_SENSOR_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  Serial.println("System Ready!");
}

void loop() {
  // Read PIR Sensor
  if (digitalRead(PIR_SENSOR_PIN) == HIGH) {
    if (!motionDetected) {
      Serial.println("Motion Detected!");
      motionDetected = true;

      // Send SMS via Africa's Talking API
      String message = "ALERT: An animal has been detected near the fence!";
      sendSMS(PHONE_NUMBER, message);
    }
  } else {
    motionDetected = false;
  }

  delay(1000);  // Delay to avoid multiple detections
}

// Function to send SMS using Africa's Talking API
void sendSMS(String phoneNumber, String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // API Endpoint
    String url = "https://api.sandbox.africastalking.com/version1/messaging";
    http.begin(url);

    // Set Headers
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("apiKey", API_KEY);

    // Prepare POST Data
    String postData = "username=" + USERNAME + "&to=" + phoneNumber + "&message=" + message;

    // Send POST Request
    int httpResponseCode = http.POST(postData);

    // Check Response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("SMS Sent Successfully!");
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error Sending SMS. HTTP Code: " + String(httpResponseCode));
    }








    
    http.end();
  } else {
    Serial.println("Wi-Fi Not Connected!");
  }
}