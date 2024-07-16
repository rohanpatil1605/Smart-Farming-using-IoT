#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char* ssid = "iQOO Z7 Pro 5G";      // Replace with your WiFi SSID
const char* password = "11223344";  // Replace with your WiFi password

const int soilMoisturePin = A0; // Analog pin connected to soil moisture sensor
int soilMoistureValue = 0;

const int servoPin = 13; // Pin connected to servo motor
Servo myservo; // Create a servo object

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Define a structure to store forecast data
struct HourlyForecast {
  String time;
  int chance_of_rain;
};

// Function to fetch and store the forecast data for the next few hours
void fetchForecastForNextFewHours(HourlyForecast (&nextFewHoursForecast)[3]) {
  // Get the current time
  timeClient.update();
  unsigned long currentEpoch = timeClient.getEpochTime();

  // Check WiFi connection
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the URL with the current time
    String url = "http://api.weatherapi.com/v1/forecast.json?key=d7e29f522b8f4549bc9160915241904&q=chennai&dt=" + String(currentEpoch) + "&aqi=yes&alerts=yes";

    // Make the HTTP request
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();

      // Parse the JSON response
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        JsonObject forecast = doc["forecast"];
        JsonArray forecastday = forecast["forecastday"];

        // Extract data for the next few hours
        int forecastCount = 0;
        for (JsonObject day : forecastday) {
          JsonArray hour = day["hour"];
          for (JsonObject hr : hour) {
            // Extract forecast time and chance of rain
            String forecastTime = hr["time"].as<String>();
            int chanceOfRain = hr["chance_of_rain"].as<int>();
            // Store the data
            nextFewHoursForecast[forecastCount].time = forecastTime;
            nextFewHoursForecast[forecastCount].chance_of_rain = chanceOfRain;
            // Move to the next forecast hour
            forecastCount++;
            if (forecastCount >= 3) {
              break;
            }
          }
          if (forecastCount >= 3) {
            break;
          }
        }
      } else {
        Serial.println("Failed to parse JSON");
      }
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end(); // Free resources
  } else {
    Serial.println("WiFi not connected");
  }
}

// Function to get the chance of rain for current time
int chanceOfRainNow(HourlyForecast (&nextFewHoursForecast)[3]) {
  // Retrieve and return the chance of rain for current time
  // You need to implement this based on the current time
  // For demonstration purposes, let's assume it's the first entry in the forecast data
  return nextFewHoursForecast[0].chance_of_rain;
}

// Function to get the chance of rain for 3 hours later
int chanceOfRainIn3Hours(HourlyForecast (&nextFewHoursForecast)[3]) {
  // Retrieve and return the chance of rain for 3 hours later
  // For demonstration purposes, let's assume it's the third entry in the forecast data
  return nextFewHoursForecast[2].chance_of_rain;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  timeClient.begin();
  timeClient.setTimeOffset(19800); // Set time offset to UTC+5:30 for IST (India Standard Time)

  myservo.attach(servoPin); // Attach servo motor

  // Fetch and store the forecast data for the next few hours
  HourlyForecast nextFewHoursForecast[3];
  fetchForecastForNextFewHours(nextFewHoursForecast);

  // Display the forecast data for the next few hours
  for (int i = 0; i < 3; i++) {
    Serial.print("Chance of rain - ");
    Serial.println(nextFewHoursForecast[i].chance_of_rain);
  }
}

void loop() {
  // Read soil moisture sensor value
  soilMoistureValue = analogRead(soilMoisturePin);

  // Fetch forecast data and store it
  HourlyForecast nextFewHoursForecast[3];
  fetchForecastForNextFewHours(nextFewHoursForecast);
  Serial.print("Soil moisture reading  : ");
  Serial.println(soilMoistureValue);
  // Check conditions and control servo motor accordingly
  if (soilMoistureValue < 6000) {
    if (chanceOfRainNow(nextFewHoursForecast) > 50 || chanceOfRainIn3Hours(nextFewHoursForecast) > 50) {
      // Do not rotate servo motor
      Serial.println("No need for watering due to rain forecast.");
    } else {
      // Rotate servo motor by 90 degrees for 5 minutes
      Serial.println("Watering initiated.");
      myservo.write(180);
      delay(3000); // 5 minutes = 300,000 milliseconds
      myservo.write(0); // Return servo to original state
      
    }
  } else {
    // Soil moisture > 3000, do not rotate servo motor
    Serial.println("Soil moisture is sufficient. No need for watering.");
  }

  // Delay for 1 hour
  // delay(3600000); // 1 hour = 3600 seconds * 1000 milliseconds
  delay(3000);
}