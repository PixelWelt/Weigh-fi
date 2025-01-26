#include <Arduino.h>
#include <HX711.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// HX711 Pins
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 3;
const int LED_PIN = 2;                  

// HX711 Object
HX711 scale;
AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World");
    Serial.flush();

    pinMode(LED_PIN, OUTPUT);
    // Start HX711
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    // Check if HX711 is ready
    if (scale.is_ready()) {
        Serial.println("HX711 found.");
    } else {
        Serial.println("HX711 not found. Please check connections.");
    }

    // Set tare (zero point)
    Serial.println("Tare... please do not place anything on the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare completed.");
    // that factor worked for me for more information about calibration factor check the datasheet of the load cell or this blogpost https://forum.arduino.cc/t/hx711-calibration/1137872
    scale.set_scale(715.961538462f);
    Serial.println("Calibration factor set.");

    delay(1000);
    WiFi.begin("SSID", "password"); // please update this with your wifi credentials
    Serial.println("\nConnecting");
    digitalWrite(LED_PIN, LOW);
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, HIGH);
    server.on("/getWeight", HTTP_GET, [](AsyncWebServerRequest *request){
        if (scale.is_ready()) {
            float weight = scale.get_units(10); 
            delay(100);
            weight = scale.get_units(10); 
            Serial.print("Weight: ");
            Serial.print(weight, 0);
            Serial.println(" g");
            request->send(200, "text/plain", String(weight, 0) + " g");
        } else {
            Serial.println("HX711 not ready.");
            request->send(500, "text/plain", "HX711 not ready");
        }
    });
    server.begin();
}

void loop(){
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi disconnected. Turning off LED.");
        digitalWrite(LED_PIN, LOW);
        delay(1000); // Delay to avoid spamming Serial Monitor
        return; // Skip the rest of the loop if Wi-Fi is disconnected
    } else {
      digitalWrite(LED_PIN, HIGH);
    }
}
