#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHT11_PIN 13
DHT dht(DHT11_PIN, DHT11);

// I2C LCD Address (Change to 0x3F if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor and actuator pins
#define CO2_SENSOR_PIN A0
#define VOC_SENSOR_PIN A1
#define VACUUM_PRESSURE_SENSOR_PIN A2
#define RELAY_FAN_PIN 12
#define BUZZER_PIN 11

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(CO2_SENSOR_PIN, INPUT);
  pinMode(VOC_SENSOR_PIN, INPUT);
  pinMode(VACUUM_PRESSURE_SENSOR_PIN, INPUT);
  pinMode(RELAY_FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_FAN_PIN, LOW);  // Ensure fan is off at start
  digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is off
  
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATIC FUME");
  lcd.setCursor(0, 1);
  lcd.print("EXTRACTOR");
  delay(2000); // Display title for 2 seconds
  lcd.clear();
}

void loop() {
  // Read sensors
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool co2_detected = digitalRead(CO2_SENSOR_PIN); // 1 = BAD, 0 = OK
  bool voc_detected = digitalRead(VOC_SENSOR_PIN); // 1 = BAD, 0 = OK
  bool vacuum_pressure_ok = digitalRead(VACUUM_PRESSURE_SENSOR_PIN); // 1 = OK, 0 = BAD

  // Determine Status
  String co2_status = (co2_detected) ? "BAD" : "OK";
  String voc_status = (voc_detected) ? "BAD" : "OK";
  String vac_status = (vacuum_pressure_ok) ? "OK" : "BAD";
  String temp_status = (temp > 50) ? "BAD" : "OK"; // Adjust threshold if needed

  // Update LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2:");
  lcd.print(co2_status);
  lcd.print(" VOC:");
  lcd.print(voc_status);

  lcd.setCursor(0, 1);
  lcd.print("Vac:");
  lcd.print(vac_status);
  lcd.print(" T:");
  lcd.print(temp_status);

  delay(1000);

  // Check for any BAD conditions to activate fume extraction
  if (co2_detected || voc_detected || !vacuum_pressure_ok || temp > 50) {
    digitalWrite(RELAY_FAN_PIN, HIGH);  // Turn on fume extractor fan
    digitalWrite(BUZZER_PIN, HIGH);     // Turn on buzzer

    delay(500);  // Short beep
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(RELAY_FAN_PIN, LOW);  // Turn off fan
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000);
}
