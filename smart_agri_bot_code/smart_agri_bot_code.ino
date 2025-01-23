#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "XXXX";
const char* password = "xxxxxx";

ESP8266WebServer server(80);

#define DHTPIN D2           // GPIO Pin for DHT11 sensor
#define DHTTYPE DHT11       // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_MOISTURE_PIN A0  // GPIO Pin for soil moisture sensor
#define RAIN_SENSOR_PIN D5    // GPIO Pin for rain detection sensor
#define RELAY_PIN D6          // GPIO Pin for relay module

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup...");

    dht.begin();
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    WiFi.begin(ssid, password);
    Serial.println("Connecting to Wi-Fi...");
    int retries = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        retries++;
        if (retries > 30) {  // After 30 seconds, timeout
            Serial.println("\nFailed to connect to Wi-Fi");
            return;
        }
    }

    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.begin();
    Serial.println("Web server started");
}

void loop() {
    server.handleClient();

    // Read sensor values
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
    int soilMoisturePercent = map(soilMoisture, 1023, 300, 0, 100);
    int rainDetected = digitalRead(RAIN_SENSOR_PIN);

    // Logic for pump control
    if (soilMoisturePercent < 30 && rainDetected == HIGH) {
        digitalWrite(RELAY_PIN, HIGH); // Turn pump on
    } else {
        digitalWrite(RELAY_PIN, LOW); // Turn pump off
    }

    delay(2000); // Wait 2 seconds before next reading
}

void handleRoot() {
    String html = "<h1>Welcome to Smart Agriculture System</h1>";
    html += "<p>This project is made by the students of Grade 9 from Sairam LeoMuthu Public School</p>";
    html += "<p>In this webserver You can see the real-time conditions around the plant and about the plant</p>";
    html += "<p><a href='/data'>View Sensor Data</a></p>";

    server.send(200, "text/html", html);
}


void handleData() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
    int soilMoisturePercent = map(soilMoisture, 1023, 300, 0, 100);
    int rainDetected = digitalRead(RAIN_SENSOR_PIN);

    String data = "<h1>Sensor Data</h1>";
    data += "<h1>Temperature: " + String(temperature) + " &deg;C</h1>";
    data += "<h1>Humidity: " + String(humidity) + " %</h1>";
    data += "<h1>Soil Moisture: " + String(soilMoisturePercent) + " %</h1>";
    data += "<h1>Rain: " + String(rainDetected == HIGH ? "No Rain" : "Rain Detected") + "</h1>";

    server.send(200, "text/html", data);
}
